#pragma once

#include <poll.h>

#include <map>
#include <string>
#include <vector>

#include "Client.hpp"

class Server {
 public:
  Server(int port, const std::string& password);
  ~Server();

  void run();

 private:
  int _port;
  std::string _password;
  int _serverSocket;
  std::vector<pollfd> _pollFds;
  std::map<int, Client> _clients;

  void setupSocket();
  void handleNewConnection();
  void handleClientMessage(int clientFd);
  void removeClient(int clientFd);
};
