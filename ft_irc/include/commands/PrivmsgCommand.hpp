#pragma once
#include "commands/ICommand.hpp"

class PrivmsgCommand : public ICommand {
 public:
  void execute(const SCommand& cmd, Client& client, ServerState& state);
};
