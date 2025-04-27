#pragma once
#include "ICommand.hpp"

class SquitCommand : public ICommand {
 public:
  void execute(const commandS& cmd, Client& client, Server& server);
};
