#include <iostream>
#include <string>

#include "Server.hpp"

int main(int argc, char** argv) {
  if (argc != 3) {
    std::cerr << "Usage: ./ft_irc <port> <password>" << std::endl;
    return ERROR;
  }
  std::string portStr = argv[1];
  int port = std::stoi(portStr);
  std::string password = argv[2];
  Server server(port, password);
  server.run();
  return 0;
}
