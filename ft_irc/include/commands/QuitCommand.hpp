#pragma once
#include "commands/ICommand.hpp"

class QuitCommand : public ICommand {
 public:
  void execute(const SCommand& cmd, Client& client, ServerState& state);
};
