#include <iostream>

#include "Server.hpp"

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Usage: ./ft_irc <port>" << std::endl;
    return ERROR;
  }
  int port = atoi(argv[1]);
  Server server(port);
  server.run();
  return 0;
}
