#pragma once
#include "commands/ICommand.hpp"

class UserCommand : public ICommand {
 public:
  void execute(const commandS& cmd, Client& client, serverStateS& state);
};
