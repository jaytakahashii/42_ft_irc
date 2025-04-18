#pragma once
#include "commands/ICommand.hpp"

class UserCommand : public ICommand {
 public:
  void execute(const SCommand& cmd, Client& client, ServerState& state);
};
