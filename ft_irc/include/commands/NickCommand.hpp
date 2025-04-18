#pragma once
#include "ICommand.hpp"

class NickCommand : public ICommand {
 public:
  void execute(const SCommand& cmd, Client& client, ServerState& state);
};
