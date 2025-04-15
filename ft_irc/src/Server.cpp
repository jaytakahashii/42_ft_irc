#include "Server.hpp"

#include <cstring>

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
  struct sockaddr_in clientAddr;
  socklen_t clientLen = sizeof(clientAddr);

  int clientFd =
      accept(_serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
  if (clientFd < 0) {
    std::cerr << "accept() failed: " << strerror(errno) << std::endl;
    return;
  }

  std::cout << "New client connected: FD = " << clientFd << std::endl;

  // pollfdに追加
  struct pollfd pfd;
  pfd.fd = clientFd;
  pfd.events = POLLIN;
  _pollFds.push_back(pfd);

  // Clientインスタンスの生成・登録
  try {
    _clients[clientFd] = new Client(clientFd);
  } catch (const std::exception& e) {
    std::cerr << "Client allocation failed: " << e.what() << std::endl;
    close(clientFd);
    return;
  }
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
  int clientFd = _pollFds[index].fd;
  std::cout << "Client disconnected: FD = " << clientFd << std::endl;

  // ソケットクローズ
  close(clientFd);

  // pollfdリストから削除
  _pollFds.erase(_pollFds.begin() + index);

  // Clientオブジェクトの削除とマップからの除去
  std::map<int, Client*>::iterator it = _clients.find(clientFd);
  if (it != _clients.end()) {
    delete it->second;
    _clients.erase(it);
  }
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
