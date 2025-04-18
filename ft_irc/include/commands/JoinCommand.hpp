#pragma once
#include "ICommand.hpp"

class JoinCommand : public ICommand {
 public:
  void execute(const commandS& cmd, Client& client, serverStateS& state);
};
