#pragma once
#include "Client.hpp"
#include "Command.hpp"

class CommandDispatcher {
 public:
  CommandDispatcher();
  ~CommandDispatcher();

  void dispatch(const Command& cmd, Client& client);
  void handleNick(const Command& cmd, Client& client);
  void handleUser(const Command& cmd, Client& client);
  void handlePing(const Command& cmd, Client& client);
};
