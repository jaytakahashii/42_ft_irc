#include <stdlib.h>

#include <cstdlib>
#include <iostream>

#include "Server.hpp"
#include "utils/utils.hpp"

static bool isValidPort(int port) {
  return (port > 0 && port <= 65535);
}

static bool isPrintable(char c) {
  return (c >= 32 && c <= 126);
}

static bool isValidCharacter(char c) {
  return (c != '*' && c != '?' && c != ' ');
}

static bool isValidPassword(const std::string& password) {
  /**
   * Check if the password is valid.
   * A valid password must be at least 8 characters long.
   * A valid password must be at most 20 characters long.
   * A valid password must not contain non-printable characters.
   * A valid password must not contain '*', '?', or ' ' (space).
   */
  if (password.length() < 8 || password.length() > 20) {
    printError("Password must be between 8 and 20 characters long.");
    return false;
  }

  for (size_t i = 0; i < password.length(); ++i) {
    if (!isPrintable(password[i])) {
      printError("Password must contain only printable characters.");
      return false;
    }
    if (!isValidCharacter(password[i])) {
      printError("Password must not contain '*', '?' or ' ' (space).");
      return false;
    }
  }
  return true;
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
    return EXIT_FAILURE;
  }

  Server server(port, password);
  server.run();

  return 0;
}
