#include "PollServer.hpp"

#include <arpa/inet.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

PollServer::PollServer(int port) : _port(port), _server_fd(-1) {
}

PollServer::~PollServer() {
  closeAllClients();
}

bool PollServer::setupSocket() {
  _server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (_server_fd == -1) {
    std::cerr << "❌ Socket creation failed: " << strerror(errno) << std::endl;
    return false;
  }

  int opt = 1;
  setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  struct sockaddr_in server_addr;
  std::memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(_port);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(_server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) ==
      -1) {
    std::cerr << "❌ Bind failed: " << strerror(errno) << std::endl;
    return false;
  }

  if (listen(_server_fd, MAX_CLIENTS) == -1) {
    std::cerr << "❌ Listen failed: " << strerror(errno) << std::endl;
    return false;
  }

  struct pollfd server_pollfd;
  server_pollfd.fd = _server_fd;
  server_pollfd.events = POLLIN;
  server_pollfd.revents = 0;
  _fds.push_back(server_pollfd);

  std::cout << "✅ Listening on port " << _port << "..." << std::endl;
  return true;
}

void PollServer::run() {
  if (!setupSocket())
    return;

  while (true) {
    int activity = poll(&_fds[0], _fds.size(), -1);
    if (activity < 0) {
      std::cerr << "❌ Poll failed: " << strerror(errno) << std::endl;
      break;
    }

    for (size_t i = 0; i < _fds.size(); ++i) {
      if (_fds[i].revents & POLLIN) {
        if (_fds[i].fd == _server_fd) {
          handleNewConnection();
        } else {
          handleClientMessage(i);
        }
      }
    }
  }

  closeAllClients();
}

void PollServer::handleNewConnection() {
  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);
  int client_fd =
      accept(_server_fd, (struct sockaddr*)&client_addr, &client_len);
  if (client_fd == -1) {
    std::cerr << "❌ Accept failed: " << strerror(errno) << std::endl;
    return;
  }

  std::cout << "🟢 New connection: " << inet_ntoa(client_addr.sin_addr) << ":"
            << ntohs(client_addr.sin_port) << std::endl;

  struct pollfd client_pollfd;
  client_pollfd.fd = client_fd;
  client_pollfd.events = POLLIN;
  client_pollfd.revents = 0;
  _fds.push_back(client_pollfd);
}

void PollServer::handleClientMessage(size_t index) {
  char buffer[BUFFER_SIZE];
  std::memset(buffer, 0, BUFFER_SIZE);

  int bytes = recv(_fds[index].fd, buffer, BUFFER_SIZE - 1, 0);
  if (bytes <= 0) {
    std::cout << "🔴 Client disconnected: fd " << _fds[index].fd << std::endl;
    close(_fds[index].fd);
    _fds.erase(_fds.begin() + index);
    return;
  }

  std::cout << "📩 Received from fd " << _fds[index].fd << ": " << buffer;

  if (std::strncmp(buffer, "exit", 4) == 0) {
    std::cout << "👋 Exit received from fd " << _fds[index].fd << std::endl;
    close(_fds[index].fd);
    _fds.erase(_fds.begin() + index);
  }
}

void PollServer::closeAllClients() {
  for (size_t i = 0; i < _fds.size(); ++i) {
    close(_fds[i].fd);
  }
  _fds.clear();
}
