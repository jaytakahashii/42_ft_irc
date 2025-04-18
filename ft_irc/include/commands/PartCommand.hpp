#pragma once
#include "ICommand.hpp"

class PartCommand : public ICommand {
 public:
  void execute(const commandS& cmd, Client& client, ServerState& state);
};
