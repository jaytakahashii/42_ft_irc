#pragma once
#include "ICommand.hpp"

class NickCommand : public ICommand {
 public:
  void execute(const commandS& cmd, Client& client, serverStateS& state);
};
