#pragma once

#include <map>
#include <string>
#include <vector>

class Client;
class Parser;
class CommandDispatcher;
class Channel;

struct serverStateS {
  std::string host;
  int port;
  std::string password;
  std::map<std::string, Channel*> channels;
  std::map<int, Client*> clients;
};

class Server {
 private:
  serverStateS _state;
  int _serverSocket;
  std::vector<struct pollfd> _pollfds;
  Parser* _parser;
  CommandDispatcher* _dispatcher;

  void setupServerSocket();
  void handleNewConnection();
  void handleClientActivity(std::size_t index);
  void removeClient(std::size_t index);

 public:
  Server(int port, std::string password);
  ~Server();

  void run();  // Main loop for the server
};
