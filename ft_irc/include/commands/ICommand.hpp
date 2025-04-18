#pragma once
#include "Client.hpp"
#include "SCommand.hpp"
#include "Server.hpp"

class ICommand {
 public:
  virtual void execute(const SCommand& cmd, Client& client,
                       ServerState& state) = 0;
  // clang-format off
  virtual ~ICommand() {}
  // clang-format on
};
