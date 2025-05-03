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

#include "Channel.hpp"
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
#include "utils/color.hpp"
#include "utils/utils.hpp"

// ------------------------------
// Constructor / Destructor
// ------------------------------

Server::Server(const int port, const std::string password)
    : _serverName(SERVER_NAME),
      _port(port),
      _password(password),
      channels(std::map<std::string, Channel*>()),
      clients(std::map<int, Client*>()) {
  _setupServerSocket();

  pollfd serverPollFd;
  serverPollFd.fd = _serverSocket;
  serverPollFd.events = POLLIN;
  _pollfds.push_back(serverPollFd);

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

// ------------------------------
// Initialization Methods
// ------------------------------

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

// 参考 : https://research.nii.ac.jp/~ichiro/syspro98/server.html
void Server::_setupServerSocket() {
  _serverSocket = socket(AF_INET, SOCK_STREAM, ENY_PROTOCOL);
  if (_serverSocket == ERROR) {
    printError("socket() failed");
    exit(EXIT_FAILURE);
  }

  if (fcntl(_serverSocket, F_SETFL, O_NONBLOCK) == ERROR) {
    printError("fcntl() failed");
    exit(EXIT_FAILURE);
  }

  sockaddr_in addr;
  std::memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(_port);

  if (bind(_serverSocket, (sockaddr*)&addr, sizeof(addr)) == ERROR) {
    printError("bind() failed");
    close(_serverSocket);
    exit(EXIT_FAILURE);
  }

  if (listen(_serverSocket, SOMAXCONN) == ERROR) {
    printError("listen() failed");
    close(_serverSocket);
    exit(EXIT_FAILURE);
  }

  std::cout << BOLDWHITE "Server listening on port " RESET << _port
            << std::endl;
}

// ------------------------------
// Connection / Activity Handlers
// ------------------------------

void Server::_handleNewConnection() {
  int clientFd = accept(_serverSocket, NULL, NULL);
  if (clientFd == ERROR) {
    printError("accept() failed");
    return;
  }

  if (fcntl(clientFd, F_SETFL, O_NONBLOCK) == ERROR) {
    printError("fcntl() failed");
    close(clientFd);
    return;
  }

  struct pollfd clientPollFd;
  clientPollFd.fd = clientFd;
  clientPollFd.events = POLLIN;
  clientPollFd.revents = 0;
  _pollfds.push_back(clientPollFd);

  clients[clientFd] = new Client(clientFd);
  std::cout << GREEN "New client connected: " << clientFd << RESET << std::endl;
}

void Server::_handleClientActivity(int clientFd) {
  char buffer[BUFFER_SIZE];
  Client* client = clients[clientFd];

  int bytesRead = recv(clientFd, buffer, sizeof(buffer) - 1, 0);
  if (bytesRead <= 0) {
    _removeClient(clientFd);
    return;
  }
  buffer[bytesRead] = '\0';
  client->getReadBuffer() += buffer;
  _processClientBuffer(client);
}

void Server::_processClientBuffer(Client* client) {
  std::string& buf = client->getReadBuffer();
  size_t pos;

  while ((pos = buf.find("\r\n")) != std::string::npos) {
    std::string line = buf.substr(0, pos);
    buf.erase(0, pos + 2);  // "\r\n"を削除
    if (line.empty())
      continue;
    commandS cmd = _parser.parseCommand(line);
    if (cmd.name.empty())
      continue;
    _commandDispatch(cmd, *client);
  }
}

void Server::_commandDispatch(const commandS& cmd, Client& client) {
  std::cout << BOLDCYAN << cmd.name << " from client " << client.getFd()
            << RESET << std::endl;

  if (_commandHandlers.find(cmd.name) != _commandHandlers.end()) {
    _commandHandlers[cmd.name]->execute(cmd, client, *this);
    return;
  }

  std::string msg =
      irc::numericReplies::ERR_UNKNOWNCOMMAND(client.getNickname(), cmd.name);
  ft_send(client.getFd(), msg);
}

// ------------------------------
// Client Utilities / Channel Utilities
// ------------------------------

void Server::_removeClient(int clientFd) {
  std::cout << GREEN "Client disconnected: " << clientFd << RESET << std::endl;
  close(clientFd);
  removeClientFromAllChannels(*clients[clientFd]);
  delete clients[clientFd];
  clients.erase(clientFd);
  for (size_t i = 0; i < _pollfds.size(); ++i) {
    if (_pollfds[i].fd == clientFd) {
      _pollfds.erase(_pollfds.begin() + i);
      break;
    }
  }
}

void Server::removeClientFromAllChannels(Client& client) {
  for (std::map<std::string, Channel*>::iterator it = channels.begin();
       it != channels.end();) {
    Channel* channel = it->second;
    if (channel->hasClient(&client)) {
      channel->removeClient(&client);
      if (channel->getClientCount() == 0) {
        delete channel;
        it = channels.erase(it);
        continue;
      }
    }
    ++it;
  }
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

void Server::deleteAllChannels() {
  for (std::map<std::string, Channel*>::iterator it = channels.begin();
       it != channels.end(); ++it) {
    delete it->second;
  }
  channels.clear();
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

// ------------------------------
// Server Info
// ------------------------------

std::string Server::getServerName() const {
  return _serverName;
}

std::string Server::getServerPassword() const {
  return _password;
}

// ------------------------------
// Lifecycle
// ------------------------------

void Server::run() {
  while (true) {
    if (poll(&_pollfds[0], _pollfds.size(), NO_LIMIT) == ERROR) {
      continue;  // エラーが発生した場合はスキップ
    }

    for (size_t i = 0; i < _pollfds.size(); ++i) {
      if (_pollfds[i].revents & POLLIN) {
        if (_pollfds[i].fd == _serverSocket) {
          _handleNewConnection();
        } else {
          _handleClientActivity(_pollfds[i].fd);
        }
      }
    }
  }
}

void Server::killServer(int exitCode) {
  for (std::map<int, Client*>::iterator it = clients.begin();
       it != clients.end(); ++it) {
    std::string msg =
        irc::numericReplies::ERR_RESTRICTED(it->second->getNickname());
    it->second->sendMessage(msg);
  }

  std::cout << BOLDRED "Server is shutting down..." RESET << std::endl;
  sleep(3);  // 3秒待機

  for (std::map<int, Client*>::iterator it = clients.begin();
       it != clients.end(); ++it) {
    _removeClient(it->second->getFd());
  }
  deleteAllChannels();
  close(_serverSocket);
  exit(exitCode);
}
