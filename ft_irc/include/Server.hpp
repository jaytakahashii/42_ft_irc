#pragma once

#include <map>
#include <string>
#include <vector>

#include "Parser.hpp"

class Client;
class CommandDispatcher;
class Channel;
class ICommand;

class Server {
 private:
  int _serverSocket;
  std::string _serverName;
  int _port;
  std::string _password;
  std::vector<struct pollfd> _pollfds;
  Parser _parser;
  std::map<std::string, ICommand*> _commandHandlers;

  void _setupServerSocket();
  void _handleNewConnection();
  void _handleClientActivity(std::size_t index);
  void _removeClient(std::size_t index);
  void _processClientBuffer(Client* client);
  void _commandDispatch(const commandS& cmd, Client& client);
  void _addCommandHandlers();

 public:
  std::map<std::string, Channel*> channels;  // チャンネルのリスト
  std::map<int, Client*> clients;            // クライアントのリスト

  Server(int port, std::string password);
  ~Server();

  std::string getServerName() const;
  std::string getServerPassword() const;

  bool isAlreadyUsedNickname(const std::string& nickname) const;

  void run();  // Main loop for the server
};
