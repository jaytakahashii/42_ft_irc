#pragma once
#include "Server.hpp"
#include "commands/ICommand.hpp"

class CommandDispatcher {
 private:
  serverStateS& _state;
  std::map<std::string, ICommand*> _commandHandlers;

 public:
  CommandDispatcher(serverStateS& state);
  ~CommandDispatcher();

  void dispatch(const commandS& cmd, Client& client);
};
