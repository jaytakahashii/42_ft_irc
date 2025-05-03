#include "Server.hpp"

#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

#include "Channel.hpp"  // Ensure the Channel class is fully defined
#include "Client.hpp"
#include "Parser.hpp"
#include "commands/JoinCommand.hpp"
#include "commands/KickCommand.hpp"
#include "commands/ModeCommand.hpp"
#include "commands/NickCommand.hpp"
#include "commands/OperCommand.hpp"
#include "commands/PartCommand.hpp"
#include "commands/PassCommand.hpp"
#include "commands/PingCommand.hpp"
#include "commands/PrivmsgCommand.hpp"
#include "commands/QuitCommand.hpp"
#include "commands/SquitCommand.hpp"
#include "commands/TopicCommand.hpp"
#include "commands/UserCommand.hpp"
#include "numericsReplies/400-499.hpp"
#include "utils/utils.hpp"

// Serverのコンストラクタ
Server::Server(int port, std::string password)
    : _serverName("irc.42tokyo.jp"),
      _port(port),
      _password(password),
      channels(std::map<std::string, Channel*>()),
      clients(std::map<int, Client*>()) {
  _setupServerSocket();
  _addCommandHandlers();
}

Server::~Server() {
  // pollfdのクリーンアップ
  for (size_t i = 0; i < _pollfds.size(); ++i)
    close(_pollfds[i].fd);

  // クライアントとチャンネルのクリーンアップ
  for (std::map<int, Client*>::const_iterator it = clients.begin();
       it != clients.end(); ++it) {
    close(it->second->getFd());
    delete it->second;
  }

  for (std::map<std::string, Channel*>::const_iterator it = channels.begin();
       it != channels.end(); ++it) {
    delete it->second;
  }

  // ソケットのクリーンアップ
  close(_serverSocket);
}

// 参考 : https://research.nii.ac.jp/~ichiro/syspro98/server.html
void Server::_setupServerSocket() {
  // AF_INET : IPv4 (IPv6 : AF_INET6)
  // SOCK_STREAM : TCP
  // 0 : Any protocol (usually TCP)
  _serverSocket = socket(AF_INET, SOCK_STREAM, 0);  // Create socket

  // Set the socket to non-blocking mode
  // F_SETFL : Set file descriptor flags
  // O_NONBLOCK : Non-blocking mode
  fcntl(_serverSocket, F_SETFL, O_NONBLOCK);

  sockaddr_in addr;
  std::memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;          // IPv4
  addr.sin_addr.s_addr = INADDR_ANY;  // Any address
  addr.sin_port = htons(_port);       // Port number

  // Bind the socket to the address and port
  bind(_serverSocket, (sockaddr*)&addr, sizeof(addr));  // register socket

  listen(_serverSocket, SOMAXCONN);  // Listen for incoming connections

  std::cout << "Server listening on port " << _port << std::endl;

  pollfd serverPollFd;
  serverPollFd.fd = _serverSocket;
  serverPollFd.events = POLLIN;  // POLLIN : Readable
  _pollfds.push_back(serverPollFd);
}

void Server::run() {
  while (true) {
    if (poll(&_pollfds[0], _pollfds.size(), -1) == -1)
      continue;

    for (size_t i = 0; i < _pollfds.size(); ++i) {
      if (_pollfds[i].revents & POLLIN) {
        if (_pollfds[i].fd == _serverSocket)
          _handleNewConnection();
        else
          _handleClientActivity(i);
      }
    }
  }
}

void Server::_handleNewConnection() {
  // 新しいクライアントの接続を受け入れる
  int clientFd = accept(_serverSocket, NULL, NULL);

  // クライアントのソケットを非ブロッキングモードに設定
  fcntl(clientFd, F_SETFL, O_NONBLOCK);

  struct pollfd clientPollFd;
  clientPollFd.fd = clientFd;
  clientPollFd.events = POLLIN;  // POLLIN : Readable
  clientPollFd.revents = 0;
  _pollfds.push_back(clientPollFd);  // pollfdに追加

  // クライアントのソケットを管理するためのClientオブジェクトを作成
  clients[clientFd] = new Client(clientFd);
  std::cout << "New client connected: " << clientFd << std::endl;
}

// クライアントからのデータを受信し、処理する
// index : pollfdsのインデックス (クライアントのソケット)
void Server::_handleClientActivity(size_t index) {
  char buffer[512];

  // clientFd : pollfdsのfd (クライアントのソケット)
  int clientFd = _pollfds[index].fd;
  Client* client = clients[clientFd];  // 一旦ポインタを取得 (別名)

  // recv : ソケットからデータを受信
  int bytesRead = recv(clientFd, buffer, sizeof(buffer) - 1, 0);
  if (bytesRead <= 0) {
    _removeClient(index);
    return;
  }

  // 受信したデータを文字列として扱う
  buffer[bytesRead] = '\0';

  client->getReadBuffer() += buffer;  // char* -> std::string

  _processClientBuffer(client);  // クライアントのバッファを処理
}

void Server::_processClientBuffer(Client* client) {
  std::string& buf = client->getReadBuffer();
  size_t pos;

  while ((pos = buf.find("\r\n")) != std::string::npos) {
    std::string line = buf.substr(0, pos);
    buf.erase(0, pos + 2);

    if (line.empty())
      continue;

    commandS cmd = _parser.parseCommand(line);
    if (cmd.name.empty())
      continue;  // 無効なコマンドはスキップ

    _commandDispatch(cmd, *client);
  }
}

// クライアントを削除する (leaks防止)
void Server::_removeClient(size_t index) {
  int clientFd = _pollfds[index].fd;
  std::cout << "Client disconnected: " << clientFd << std::endl;
  close(clientFd);                           // Close the socket
  delete clients[clientFd];                  // Delete the Client object
  clients.erase(clientFd);                   // Remove from map
  _pollfds.erase(_pollfds.begin() + index);  // Remove from pollfds
}

void Server::_commandDispatch(const commandS& cmd, Client& client) {
  // debug用の出力
  std::cout << "Dispatching command: " << cmd.name << " from client "
            << client.getFd() << std::endl;

  if (_commandHandlers.find(cmd.name) != _commandHandlers.end()) {
    _commandHandlers[cmd.name]->execute(cmd, client, *this);
    return;
  }

  // std::string msg = irc::numericReplies::ERR_UNKNOWNCOMMAND(cmd.name);

  std::string msg = ":server 421 " + client.getNickname() + " " + cmd.name +
                    " :Unknown command\r\n";
  send(client.getFd(), msg.c_str(), msg.size(), 0);
}

void Server::_addCommandHandlers() {
  _commandHandlers["PASS"] = new PassCommand();
  _commandHandlers["NICK"] = new NickCommand();
  _commandHandlers["USER"] = new UserCommand();
  _commandHandlers["JOIN"] = new JoinCommand();
  _commandHandlers["PART"] = new PartCommand();
  _commandHandlers["PRIVMSG"] = new PrivmsgCommand();
  _commandHandlers["PING"] = new PingCommand();
  _commandHandlers["QUIT"] = new QuitCommand();
  _commandHandlers["KICK"] = new KickCommand();
  _commandHandlers["TOPIC"] = new TopicCommand();
  _commandHandlers["MODE"] = new ModeCommand();
  _commandHandlers["OPER"] = new OperCommand();
  _commandHandlers["SQUIT"] = new SquitCommand();
  // TODO : 他のコマンドもここに追加
}

std::string Server::getServerName() const {
  return _serverName;
}

std::string Server::getServerPassword() const {
  return _password;
}

bool Server::isAlreadyUsedNickname(const std::string& nickname) const {
  for (std::map<int, Client*>::const_iterator it = clients.begin();
       it != clients.end(); ++it) {
    if (it->second && it->second->getNickname() == nickname) {
      return true;
    }
  }
  return false;
}

void Server::sendAllClients(const std::string& message) const {
  for (std::map<int, Client*>::const_iterator it = clients.begin();
       it != clients.end(); ++it) {
    if (it->second->isRegistered())
      it->second->sendMessage(message);
  }
}

void Server::removeClientFromAllChannels(Client& client) {
  for (std::map<std::string, Channel*>::iterator it = channels.begin();
       it != channels.end(); ++it) {
    Channel* channel = it->second;
    if (channel->hasClient(&client)) {
      channel->removeClient(&client);
      if (channel->getClientCount() == 0) {
        delete channel;
        channels.erase(it->first);
      }
    }
  }
}

void Server::deleteAllChannels() {
  for (std::map<std::string, Channel*>::iterator it = channels.begin();
       it != channels.end(); ++it) {
    delete it->second;
  }
  channels.clear();
}

void Server::killServer() {
  std::string msg = irc::numericReplies::ERR_RESTRICTED();
  sendAllClients(msg);

  sleep(5);  // 5秒待機

  for (std::map<int, Client*>::iterator it = clients.begin();
       it != clients.end(); ++it) {
    _removeClient(it->second->getFd());
  }

  deleteAllChannels();
  close(_serverSocket);
  exit(0);
}

bool Server::hasChannel(const std::string& channelName) const {
  for (std::map<std::string, Channel*>::const_iterator it = channels.begin();
       it != channels.end(); ++it) {
    if (ircEqual(it->first, channelName, true)) {
      return true;
    }
  }
  return false;
}

static bool isInvalidChannelChar(char c) {
  // forbidden: space, ^G, comma, NULL, BELL, CR, LF
  return (c == ' ' || c == 0x07 || c == ',' || c == 0x00 || c == 0x0D ||
          c == 0x0A);
}

bool Server::isValidChannelName(const std::string& name) const {
  /**
   * Should be started with a '&', '#', '!', or '+'
   * Should be between 1 and 50 characters long
   * should not contain spaces, ^G(ASCII7), comma (,), NULL, BELL, CR, LF
   * Names are case insensitive (e.g. #channel and #Channel are the same)
   */
  if (name.length() < 1 || name.length() > 50) {
    return false;
  }

  if (name[0] == '!' && name.length() >= 6) {
    for (int i = 1; i <= 5; ++i) {
      if (!std::isupper(name[i]) && !std::isdigit(name[i])) {
        return false;
      }
    }
    for (std::size_t i = 6; i < name.length(); ++i) {
      if (isInvalidChannelChar(name[i])) {
        return false;
      }
    }
    return true;
  }

  if (name[0] != '&' && name[0] != '#' && name[0] != '+') {
    return false;
  }

  for (std::size_t i = 1; i < name.length(); ++i) {
    if (isInvalidChannelChar(name[i])) {
      return false;
    }
  }
  return true;
}

bool Server::isValidChannelKey(const std::string& channelKey) const {
  /**
   * * Should be 1-15 characters long
   * * Should not contain NUL, CR, LR, " ", "@"
   */
  if (channelKey.empty() || channelKey.size() > 15) {
    return false;
  }

  for (std::size_t i = 0; i < channelKey.size(); ++i) {
    char c = channelKey[i];
    if (c == '\0' || c == '\r' || c == '\n' || c == ' ' || c == '@') {
      return false;
    }
  }
  return true;
}
