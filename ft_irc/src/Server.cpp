#include "Server.hpp"

#include <fcntl.h>
#include <netinet/in.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <iostream>

Server::Server(int port, const std::string& password)
    : _port(port), _password(password), _serverSocket(-1) {
  setupSocket();
}

Server::~Server() {
  close(_serverSocket);
}

void Server::setupSocket() {
  // ソケット作成〜bind〜listen
}

void Server::run() {
  while (true) {
    int pollCount = poll(&_pollFds[0], _pollFds.size(), -1);
    if (pollCount == -1) {
      perror("poll");
      break;
    }

    for (size_t i = 0; i < _pollFds.size(); ++i) {
      if (_pollFds[i].revents & POLLIN) {
        if (_pollFds[i].fd == _serverSocket) {
          handleNewConnection();
        } else {
          handleClientMessage(_pollFds[i].fd);
        }
      }
    }
  }
}

void Server::handleNewConnection() {
  // accept(), pollfd追加, Clientインスタンス作成
}

void Server::handleClientMessage(int clientFd) {
  // recv(), パース, コマンド処理
}

void Server::removeClient(int clientFd) {
  // pollfdから削除、Client解放
}
