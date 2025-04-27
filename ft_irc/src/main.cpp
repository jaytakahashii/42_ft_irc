#include <stdlib.h>

#include <cstdlib>
#include <iostream>

#include "Server.hpp"
#include "utils/utils.hpp"

static bool isValidPort(int port) {
  return (port > 0 && port <= 65535);
}

static bool isValidPassword(const std::string& password) {
  return !password.empty();
}

int main(int argc, char** argv) {
  if (argc != 3) {
    printError("Usage: ./ircserv <port> <password>");
    return EXIT_FAILURE;
  }

  int port = atoi(argv[1]);
  if (!isValidPort(port)) {
    printError("Invalid port. Port must be between 1 and 65535.");
    return EXIT_FAILURE;
  }
  std::string password = argv[2];

  if (!isValidPassword(password)) {
    printError("Invalid password.");
    return EXIT_FAILURE;
  }

  Server server(port, password);
  server.run();
  return 0;
}
