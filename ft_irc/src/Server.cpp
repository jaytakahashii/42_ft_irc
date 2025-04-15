#include "Server.hpp"

Server::Server(int port) : _port(port), _serverSocket(-1) {
}

Server::~Server() {
  if (_serverSocket != -1)
    close(_serverSocket);
}

void Server::initSocket() {
  _serverSocket = socket(AF_INET, SOCK_STREAM, 0);  // ソケット作成
  if (_serverSocket == -1)
    throw std::runtime_error("socket() failed");

  setNonBlocking(_serverSocket);

  int opt = 1;
  setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(_port);

  if (bind(_serverSocket, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    throw std::runtime_error("bind() failed");

  if (listen(_serverSocket, SOMAXCONN) < 0)
    throw std::runtime_error("listen() failed");

  struct pollfd serverPollFd = {_serverSocket, POLLIN, 0};
  _pollFds.push_back(serverPollFd);

  std::cout << "[Server] Listening on port " << _port << std::endl;
}

void Server::setNonBlocking(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags == -1)
    throw std::runtime_error("fcntl(F_GETFL) failed");
  if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
    throw std::runtime_error("fcntl(F_SETFL) failed");
}

void Server::acceptNewClient() {
  int clientFd = accept(_serverSocket, NULL, NULL);
  if (clientFd < 0)
    return;

  setNonBlocking(clientFd);

  struct pollfd clientPollFd = {clientFd, POLLIN, 0};
  _pollFds.push_back(clientPollFd);

  std::cout << "[Server] New client connected: FD = " << clientFd << std::endl;
}

void Server::handleClientData(int clientFd) {
  char buffer[1024];
  ssize_t bytesReceived = recv(clientFd, buffer, sizeof(buffer) - 1, 0);

  if (bytesReceived <= 0) {
    std::cout << "[Server] Client disconnected: FD = " << clientFd << std::endl;
    for (size_t i = 1; i < _pollFds.size(); ++i) {
      if (_pollFds[i].fd == clientFd) {
        removeClient(i);
        break;
      }
    }
  } else {
    buffer[bytesReceived] = '\0';
    std::cout << "[Client " << clientFd << "] " << buffer << std::endl;
    // 今後：ここでParser→Command処理へ渡す
  }
}

void Server::removeClient(int index) {
  close(_pollFds[index].fd);
  _pollFds.erase(_pollFds.begin() + index);
}

void Server::run() {
  initSocket();

  while (true) {
    int pollCount = poll(_pollFds.data(), _pollFds.size(), -1);
    if (pollCount < 0) {
      perror("poll");
      break;
    }

    for (size_t i = 0; i < _pollFds.size(); ++i) {
      if (_pollFds[i].revents & POLLIN) {
        if (_pollFds[i].fd == _serverSocket) {
          acceptNewClient();
        } else {
          handleClientData(_pollFds[i].fd);
        }
      }
    }
  }
}
