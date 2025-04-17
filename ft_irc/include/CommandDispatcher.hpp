#pragma once
#include "Channel.hpp"
#include "Client.hpp"
#include "ICommand.hpp"
#include "Server.hpp"

class CommandDispatcher {
 private:
  std::map<std::string, Channel*>& _channels;
  std::map<int, Client*>& _clients;

 public:
  CommandDispatcher(std::map<std::string, Channel*>& channels,
                    std::map<int, Client*>& clients);
  ~CommandDispatcher();

  void dispatch(const ICommand& cmd, Client& client);
  void handleNick(const ICommand& cmd, Client& client);
  void handleUser(const ICommand& cmd, Client& client);
  void handlePing(const ICommand& cmd, Client& client);
  void handleJoin(const ICommand& cmd, Client& client);
  void handlePrivmsg(const ICommand& cmd, Client& client);
};
