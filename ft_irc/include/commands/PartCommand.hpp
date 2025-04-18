#pragma once
#include "ICommand.hpp"

class PartCommand : public ICommand {
 public:
  void execute(const SCommand& cmd, Client& client, ServerState& state);
};
