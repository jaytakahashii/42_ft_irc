#pragma once
#include "ICommand.hpp"

class InviteCommand : public ICommand {
 public:
  void execute(const commandS& cmd, Client& client, Server& server);
};
