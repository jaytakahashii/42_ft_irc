#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "Server.hpp"

int main(int argc, char** argv) {
  if (argc != 3) {
    std::cerr << "Usage: ./ft_irc <port> <password>" << std::endl;
    return ERROR;
  }
  int port = atoi(argv[1]);
  std::string password = argv[2];
  Server server(port, password);
  server.run();
  return 0;
}
