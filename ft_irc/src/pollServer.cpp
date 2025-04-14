#include "pollServer.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

void run_poll_server(int port) {
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    std::cerr << "❌ Socket creation failed: " << strerror(errno) << std::endl;
    return;
  }

  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  struct sockaddr_in server_addr;
  std::memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) ==
      -1) {
    std::cerr << "❌ Bind failed: " << strerror(errno) << std::endl;
    close(server_fd);
    return;
  }

  if (listen(server_fd, MAX_CLIENTS) == -1) {
    std::cerr << "❌ Listen failed: " << strerror(errno) << std::endl;
    close(server_fd);
    return;
  }

  std::cout << "✅ Listening on port " << port << "..." << std::endl;

  std::vector<struct pollfd> fds;

  struct pollfd server_pollfd;
  server_pollfd.fd = server_fd;
  server_pollfd.events = POLLIN;
  server_pollfd.revents = 0;
  fds.push_back(server_pollfd);

  char buffer[BUFFER_SIZE];

  while (true) {
    int activity = poll(&fds[0], fds.size(), -1);
    if (activity < 0) {
      std::cerr << "❌ Poll failed: " << strerror(errno) << std::endl;
      break;
    }

    for (size_t i = 0; i < fds.size(); ++i) {
      if (fds[i].revents & POLLIN) {
        if (fds[i].fd == server_fd) {
          struct sockaddr_in client_addr;
          socklen_t client_len = sizeof(client_addr);
          int client_fd =
              accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
          if (client_fd == -1) {
            std::cerr << "❌ Accept failed: " << strerror(errno) << std::endl;
            continue;
          }

          std::cout << "🟢 New connection: " << inet_ntoa(client_addr.sin_addr)
                    << ":" << ntohs(client_addr.sin_port) << std::endl;

          struct pollfd client_pollfd;
          client_pollfd.fd = client_fd;
          client_pollfd.events = POLLIN;
          client_pollfd.revents = 0;
          fds.push_back(client_pollfd);
        } else {
          std::memset(buffer, 0, BUFFER_SIZE);
          int bytes = recv(fds[i].fd, buffer, BUFFER_SIZE - 1, 0);
          if (bytes <= 0) {
            std::cout << "🔴 Client disconnected: fd " << fds[i].fd
                      << std::endl;
            close(fds[i].fd);
            fds.erase(fds.begin() + i);
            --i;
          } else {
            std::cout << "📩 Received from fd " << fds[i].fd << ": " << buffer;

            if (std::strncmp(buffer, "exit", 4) == 0) {
              std::cout << "👋 Exit received from fd " << fds[i].fd
                        << std::endl;
              close(fds[i].fd);
              fds.erase(fds.begin() + i);
              --i;
            }
          }
        }
      }
    }
  }

  for (size_t i = 0; i < fds.size(); ++i) {
    close(fds[i].fd);
  }
}
