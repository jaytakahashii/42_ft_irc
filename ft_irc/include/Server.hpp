#pragma once

#include <map>
#include <string>
#include <vector>

class Client;
class Parser;
class CommandDispatcher;
class Channel;

// Serverの状態を保持する構造体 (他のクラスに渡す)
struct serverStateS {
  std::string host;
  int port;
  std::string password;
  std::map<std::string, Channel*> channels;  // チャンネルのリスト
  std::map<int, Client*> clients;            // クライアントのリスト
};

class Server {
 private:
  serverStateS _state;
  int _serverSocket;
  std::vector<struct pollfd> _pollfds;
  Parser* _parser;
  CommandDispatcher* _dispatcher;

  void _setupServerSocket();
  void _handleNewConnection();
  void _handleClientActivity(std::size_t index);
  void _removeClient(std::size_t index);
  bool _isValidPassword();

 public:
  Server(int port, std::string password);
  ~Server();

  void run();  // Main loop for the server
};
