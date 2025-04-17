#pragma once
#include <poll.h>
#include <stdlib.h>

#include <iostream>
#include <map>
#include <vector>

#define ERROR -1

// forward declarations
class Client;
class Parser;
class CommandDispatcher;
class Channel;

class Server {
 private:
  int _port;
  std::string _password;
  int _serverSocket;
  std::vector<struct pollfd> _pollfds;
  std::map<int, Client*> _clients;
  std::map<std::string, Channel*> _channels;
  Parser* _parser;
  CommandDispatcher* _dispatcher;

  void setupServerSocket();
  void handleNewConnection();
  void handleClientActivity(size_t index);
  void removeClient(size_t index);

 public:
  Server(int port, std::string password);
  ~Server();

  void run();  // Main loop for the server
};
