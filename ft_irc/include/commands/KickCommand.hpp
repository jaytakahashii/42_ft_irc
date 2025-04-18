#pragma once
#include "ICommand.hpp"

class KickCommand : public ICommand {
 public:
  void execute(const SCommand& cmd, Client& client, ServerState& state);
};
