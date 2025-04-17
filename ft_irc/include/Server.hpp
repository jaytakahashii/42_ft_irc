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

struct ServerState {
  std::string host;
  int port;
  std::string password;
  std::map<std::string, Channel*> channels;
  std::map<int, Client*> clients;
};

class Server {
 private:
  ServerState _state;
  int _serverSocket;
  std::vector<struct pollfd> _pollfds;
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
