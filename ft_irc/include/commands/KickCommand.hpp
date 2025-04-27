#pragma once
#include "ICommand.hpp"

class KickCommand : public ICommand {
 public:
  void execute(const commandS& cmd, Client& client, Server& server);
};
