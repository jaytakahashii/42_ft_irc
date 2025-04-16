#pragma once
#include "Channel.hpp"
#include "Client.hpp"
#include "ICommand.hpp"
#include "Server.hpp"

class CommandDispatcher {
 private:
  std::map<std::string, Channel*> _channels;

 public:
  CommandDispatcher();
  ~CommandDispatcher();

  void dispatch(const ICommand& cmd, Client& client,
                std::map<int, Client*>& clients);
  void handleNick(const ICommand& cmd, Client& client);
  void handleUser(const ICommand& cmd, Client& client);
  void handlePing(const ICommand& cmd, Client& client);
  void handleJoin(const ICommand& cmd, Client& client);
  void handlePrivmsg(const ICommand& cmd, Client& client,
                     std::map<int, Client*>& clients);
};
