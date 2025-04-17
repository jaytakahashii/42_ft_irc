#include "Server.hpp"

#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

#include "Client.hpp"
#include "CommandDispatcher.hpp"
#include "Parser.hpp"
#include "color.hpp"

Server::Server(int port, std::string password) : _parser(new Parser()) {
  _state.host = "localhost";
  _state.port = port;
  _state.password = password;
  _state.channels = std::map<std::string, Channel*>();
  _state.clients = std::map<int, Client*>();
  setupServerSocket();
  _dispatcher = new CommandDispatcher(_state);
}

Server::~Server() {
  for (size_t i = 0; i < _pollfds.size(); ++i)
    close(_pollfds[i].fd);
  for (std::map<int, Client*>::iterator it = _state.clients.begin();
       it != _state.clients.end(); ++it)
    delete it->second;
  delete _parser;
  delete _dispatcher;
}

// 参考 : https://research.nii.ac.jp/~ichiro/syspro98/server.html
void Server::setupServerSocket() {
  // AF_INET : IPv4 (IPv6 : AF_INET6)
  // SOCK_STREAM : TCP
  // 0 : Any protocol (usually TCP)
  _serverSocket = socket(AF_INET, SOCK_STREAM, 0);  // Create socket

  // Set the socket to non-blocking mode
  // F_SETFL : Set file descriptor flags
  // O_NONBLOCK : Non-blocking mode
  fcntl(_serverSocket, F_SETFL, O_NONBLOCK);

  sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;           // IPv4
  addr.sin_addr.s_addr = INADDR_ANY;   // Any address
  addr.sin_port = htons(_state.port);  // Port number

  // Bind the socket to the address and port
  bind(_serverSocket, (sockaddr*)&addr, sizeof(addr));  // register socket

  listen(_serverSocket, SOMAXCONN);  // Listen for incoming connections

  std::cout << BOLDWHITE "🎵 Server listening on port " RESET << _state.port
            << std::endl;

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
          handleNewConnection();
        else
          handleClientActivity(i);
      }
    }
  }
}

void Server::handleNewConnection() {
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
  _state.clients[clientFd] = new Client(clientFd);
  std::cout << "👶 New client connected: " << clientFd << std::endl;
}

// クライアントからのデータを受信し、処理する
// index : pollfdsのインデックス (クライアントのソケット)
void Server::handleClientActivity(size_t index) {
  char buffer[512];

  // clientFd : pollfdsのfd (クライアントのソケット)
  int clientFd = _pollfds[index].fd;

  // recv : ソケットからデータを受信
  int bytesRead = recv(clientFd, buffer, sizeof(buffer) - 1, 0);
  if (bytesRead <= 0) {
    removeClient(index);  // クライアントが切断された場合
    return;
  }

  // 受信したデータを文字列として扱う
  buffer[bytesRead] = '\0';

  // クライアントのソケットに対応するClientオブジェクトを取得
  Client* client = _state.clients[clientFd];  // 一旦ポインタを取得 (別名)
  client->getReadBuffer() += buffer;          // char* -> std::string

  // 簡易的な改行終端検出
  size_t pos;
  while ((pos = client->getReadBuffer().find("\n")) != std::string::npos) {
    std::string line = client->getReadBuffer().substr(0, pos);
    client->getReadBuffer().erase(0, pos + 1);
    SCommand cmd = _parser->parse(line);
    _dispatcher->dispatch(cmd, *client);
  }
}

// クライアントを削除する (leaks防止)
void Server::removeClient(size_t index) {
  int clientFd = _pollfds[index].fd;
  std::cout << "Client disconnected: " << clientFd << std::endl;
  close(clientFd);                           // Close the socket
  delete _state.clients[clientFd];           // Delete the Client object
  _state.clients.erase(clientFd);            // Remove from map
  _pollfds.erase(_pollfds.begin() + index);  // Remove from pollfds
}
