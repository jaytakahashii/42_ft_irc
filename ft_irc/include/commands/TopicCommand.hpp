
#pragma once
#include "commands/ICommand.hpp"

class TopicCommand : public ICommand {
 public:
  void execute(const SCommand& cmd, Client& client, ServerState& state);
};
