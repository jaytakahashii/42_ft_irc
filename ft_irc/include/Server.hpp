#pragma once

#include <map>
#include <string>
#include <vector>

#include "Parser.hpp"

class Client;
class CommandDispatcher;
class Channel;
class ICommand;

#define AUTHORIZED_HOSTS "127.0.0.1"
#define SERVER_NAME "irc.42tokyo.jp"
#define NO_LIMIT -1
#define ERROR -1
#define ENY_PROTOCOL 0
#define BUFFER_SIZE 512
#define NON_OPTIONAL 0

class Server {
 private:
  int _serverSocket;
  const std::string _serverName;
  const int _port;
  const std::string _password;
  std::vector<struct pollfd> _pollfds;
  Parser _parser;
  std::map<std::string, ICommand*> _commandHandlers;

  void _addCommandHandlers();
  void _setupServerSocket();
  void _handleNewConnection();
  void _handleClientActivity(int clientFd);
  void _processClientBuffer(Client* client);
  void _commandDispatch(const commandS& cmd, Client& client);
  void _removeClient(int clientFd);

 public:
  std::map<std::string, Channel*> channels;  // チャンネルのリスト
  std::map<int, Client*> clients;            // クライアントのリスト

  Server(const int port, const std::string password);
  ~Server();

  std::string getServerName() const;
  std::string getServerPassword() const;

  bool isAlreadyUsedNickname(const std::string& nickname) const;
  void sendAllClients(const std::string& message) const;
  void removeClientFromAllChannels(Client& client);

  bool isValidChannelName(const std::string& channelName) const;
  bool isValidChannelKey(const std::string& channelKey) const;

  void deleteAllChannels();

  bool hasChannel(const std::string& channelName) const;

  void killServer();

  void run();  // Main loop for the server
};
