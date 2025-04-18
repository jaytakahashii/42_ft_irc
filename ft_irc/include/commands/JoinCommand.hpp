#pragma once
#include "ICommand.hpp"

class JoinCommand : public ICommand {
 public:
  void execute(const SCommand& cmd, Client& client, ServerState& state);
};
