#pragma once
#include "ICommand.hpp"

class PassCommand : public ICommand {
 public:
  void execute(const commandS& cmd, Client& client, ServerState& state);
};
