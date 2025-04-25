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

#include "Client.hpp"
#include "Parser.hpp"
#include "commands/JoinCommand.hpp"
#include "commands/KickCommand.hpp"
#include "commands/NickCommand.hpp"
#include "commands/PartCommand.hpp"
#include "commands/PassCommand.hpp"
#include "commands/PingCommand.hpp"
#include "commands/PrivmsgCommand.hpp"
#include "commands/QuitCommand.hpp"
#include "commands/TopicCommand.hpp"
#include "commands/UserCommand.hpp"
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
  for (std::map<int, Client*>::iterator it = clients.begin();
       it != clients.end(); ++it)
    delete it->second;

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

  // recv : ソケットからデータを受信
  int bytesRead = recv(clientFd, buffer, sizeof(buffer) - 1, 0);
  if (bytesRead <= 0) {
    _removeClient(index);  // クライアントが切断された場合
    return;
  }

  // 受信したデータを文字列として扱う
  buffer[bytesRead] = '\0';

  // クライアントのソケットに対応するClientオブジェクトを取得
  Client* client = clients[clientFd];  // 一旦ポインタを取得 (別名)
  client->getReadBuffer() += buffer;   // char* -> std::string

  _processClientBuffer(client);  // クライアントのバッファを処理
}

// クライアントのバッファを処理する
void Server::_processClientBuffer(Client* client) {
  // TODO : 現時点では簡易的な実装
  size_t pos;
  while ((pos = client->getReadBuffer().find("\n")) != std::string::npos) {
    std::string line = client->getReadBuffer().substr(0, pos);
    client->getReadBuffer().erase(0, pos + 1);
    // コマンドをパースして実行
    commandS cmd = _parser.parse(line);
    if (cmd.name == "")
      continue;                      // 無効なコマンドは無視
    _commandDispatch(cmd, *client);  // コマンドをディスパッチ
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
  // TODO : 他のコマンドもここに追加
}

std::string Server::getServerName() const {
  return _serverName;
}

std::string Server::getServerPassword() const {
  return _password;
}
