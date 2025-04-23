#pragma once
#include "commands/ICommand.hpp"

class PingCommand : public ICommand {
 public:
  void execute(const commandS& cmd, Client& client, serverStateS& state);
};
