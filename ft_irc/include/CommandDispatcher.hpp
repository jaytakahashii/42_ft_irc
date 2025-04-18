#pragma once
#include "Channel.hpp"
#include "Client.hpp"
#include "SCommand.hpp"
#include "Server.hpp"
#include "commands/ICommand.hpp"

class CommandDispatcher {
 private:
  ServerState& _state;
  std::map<std::string, ICommand*> _commandHandlers;

 public:
  CommandDispatcher(ServerState& state);
  ~CommandDispatcher();

  void dispatch(const SCommand& cmd, Client& client);
};
