#include <stdlib.h>

#include <iostream>

#include "Server.hpp"
#include "utils/utils.hpp"

#define ERROR -1

static bool isValidPort(int port) {
  return (port > 0 && port <= 65535);
}

static bool isValidPassword(const std::string& password) {
  return !password.empty();
}

int main(int argc, char** argv) {
  if (argc != 3) {
    printError("Usage: ./ircserv <port> <password>");
    return ERROR;
  }

  int port = atoi(argv[1]);
  if (!isValidPort(port)) {
    printError("Invalid port. Port must be between 1 and 65535.");
    return ERROR;
  }
  std::string password = argv[2];
  if (!isValidPassword(argv[2])) {
    printError("Invalid password. Password cannot be empty.");
    return ERROR;
  }

  Server server(port, password);
  server.run();
  return 0;
}
