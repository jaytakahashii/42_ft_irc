#pragma once

#include <poll.h>

#include <map>
#include <string>
#include <vector>

#include "Parser.hpp"

// Forward declarations
class Client;
class Channel;
class ICommand;

// Constants
#define AUTHORIZED_HOSTS "127.0.0.1"
#define SERVER_NAME "irc.42tokyo.jp"
#define NO_LIMIT -1
#define ERROR -1
#define ENY_PROTOCOL 0
#define BUFFER_SIZE 512
#define NON_OPTIONAL 0

class Server {
 private:
  // Core configuration
  int _serverSocket;
  const std::string _serverName;
  const int _port;
  const std::string _password;

  // State & management
  std::vector<struct pollfd> _pollfds;
  Parser _parser;
  std::map<std::string, ICommand*> _commandHandlers;

  // Internal handlers
  void _addCommandHandlers();
  void _setupServerSocket();
  void _handleNewConnection();
  void _handleClientActivity(int clientFd);
  void _processClientBuffer(Client* client);
  void _commandDispatch(const commandS& cmd, Client& client);
  void _removeClient(int clientFd);

 public:
  // Client & Channel registry
  std::map<std::string, Channel*> channels;
  std::map<int, Client*> clients;

  // Constructor & Destructor
  Server(const int port, const std::string password);
  ~Server();

  // Server Info
  std::string getServerName() const;
  std::string getServerPassword() const;

  // Client Utilities
  bool isAlreadyUsedNickname(const std::string& nickname) const;
  void sendAllClients(const std::string& message) const;
  void removeClientFromAllChannels(Client& client);

  // Channel Utilities
  bool hasChannel(const std::string& channelName) const;
  bool isValidChannelName(const std::string& channelName) const;
  bool isValidChannelKey(const std::string& channelKey) const;
  void deleteAllChannels();

  // Lifecycle
  void run();
  void killServer(int exitCode = 0);
};
