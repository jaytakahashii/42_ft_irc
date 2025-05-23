#pragma once
#include "ICommand.hpp"

class ModeCommand : public ICommand {
 public:
  void execute(const commandS& cmd, Client& client, Server& server);
};
