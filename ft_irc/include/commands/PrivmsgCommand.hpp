#pragma once
#include "commands/ICommand.hpp"

class PrivmsgCommand : public ICommand {
 public:
  void execute(const commandS& cmd, Client& client, ServerState& state);
};
