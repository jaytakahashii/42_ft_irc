#pragma once
#include "ICommand.hpp"

class PassCommand : public ICommand {
 public:
  void execute(const SCommand& cmd, Client& client, ServerState& state);
};
