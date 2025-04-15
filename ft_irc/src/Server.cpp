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

Server::Server(int port)
    : _port(port), _parser(new Parser()), _dispatcher(new CommandDispatcher()) {
  setupServerSocket();
}

Server::~Server() {
  for (size_t i = 0; i < _pollfds.size(); ++i)
    close(_pollfds[i].fd);
  for (std::map<int, Client*>::iterator it = _clients.begin();
       it != _clients.end(); ++it)
    delete it->second;
  delete _parser;
  delete _dispatcher;
}

void Server::setupServerSocket() {
  _serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  fcntl(_serverSocket, F_SETFL, O_NONBLOCK);
  sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(_port);
  bind(_serverSocket, (sockaddr*)&addr, sizeof(addr));
  listen(_serverSocket, SOMAXCONN);

  pollfd serverPollFd;
  serverPollFd.fd = _serverSocket;
  serverPollFd.events = POLLIN;
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
  int clientFd = accept(_serverSocket, NULL, NULL);
  fcntl(clientFd, F_SETFL, O_NONBLOCK);
  _pollfds.push_back((pollfd){clientFd, POLLIN, 0});
  _clients[clientFd] = new Client(clientFd);
  std::cout << "New client connected: " << clientFd << std::endl;
}

void Server::handleClientActivity(size_t index) {
  char buffer[512];
  int clientFd = _pollfds[index].fd;
  int bytesRead = recv(clientFd, buffer, sizeof(buffer) - 1, 0);
  if (bytesRead <= 0) {
    removeClient(index);
    return;
  }
  buffer[bytesRead] = '\0';
  Client* client = _clients[clientFd];
  client->getReadBuffer() += buffer;

  // 簡易的な改行終端検出
  size_t pos;
  while ((pos = client->getReadBuffer().find("\n")) != std::string::npos) {
    std::string line = client->getReadBuffer().substr(0, pos);
    client->getReadBuffer().erase(0, pos + 1);
    Command cmd = _parser->parse(line);
    _dispatcher->dispatch(cmd, *client);
  }
}

void Server::removeClient(size_t index) {
  int clientFd = _pollfds[index].fd;
  std::cout << "Client disconnected: " << clientFd << std::endl;
  close(clientFd);
  delete _clients[clientFd];
  _clients.erase(clientFd);
  _pollfds.erase(_pollfds.begin() + index);
}
