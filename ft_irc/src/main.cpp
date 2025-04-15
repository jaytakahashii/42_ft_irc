#include "Server.hpp"

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Usage: ./IRCServer <port>" << std::endl;
    return 1;
  }

  int port = std::atoi(argv[1]);
  try {
    Server server(port);
    server.run();
  } catch (const std::exception& e) {
    std::cerr << "Fatal error: " << e.what() << std::endl;
  }

  return 0;
}
