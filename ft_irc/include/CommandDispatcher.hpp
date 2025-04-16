#pragma once
#include "Client.hpp"
#include "ICommand.hpp"

class CommandDispatcher {
 public:
  CommandDispatcher();
  ~CommandDispatcher();

  void dispatch(const ICommand& cmd, Client& client);
  void handleNick(const ICommand& cmd, Client& client);
  void handleUser(const ICommand& cmd, Client& client);
  void handlePing(const ICommand& cmd, Client& client);
};
