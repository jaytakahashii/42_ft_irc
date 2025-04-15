#pragma once
#include <poll.h>
#include <stdlib.h>

#include <iostream>
#include <map>
#include <vector>

class Client;
class Parser;
class CommandDispatcher;

#define ERROR -1

class Server {
 private:
  int _port;
  int _serverSocket;
  std::vector<struct pollfd> _pollfds;
  std::map<int, Client*> _clients;
  Parser* _parser;
  CommandDispatcher* _dispatcher;

  void setupServerSocket();
  void handleNewConnection();
  void handleClientActivity(size_t index);
  void removeClient(size_t index);

 public:
  Server(int port);
  ~Server();

  void run();  // Main loop for the server
};
