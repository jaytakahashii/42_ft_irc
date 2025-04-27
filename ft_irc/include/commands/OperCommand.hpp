#pragma once
#include "ICommand.hpp"

class OperCommand : public ICommand {
 public:
  void execute(const commandS& cmd, Client& client, Server& server);
};
