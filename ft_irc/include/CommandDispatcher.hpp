#pragma once
#include "Client.hpp"
#include "Command.hpp"

class CommandDispatcher {
 public:
  CommandDispatcher();
  ~CommandDispatcher();

  void dispatch(const Command& cmd, Client& client);
};
