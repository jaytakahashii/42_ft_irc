#pragma once
#include <string>
#include <vector>

#include "Client.hpp"
#include "Server.hpp"

struct commandS {
  std::string prefix;             // a.g. ":nickname!username@hostname"
  std::string name;               // a.g. "NICK", "USER", "JOIN", etc.
  std::vector<std::string> args;  // arguments for the command
};

class ICommand {
 public:
  virtual void execute(const commandS& cmd, Client& client,
                       serverStateS& state) = 0;
  // clang-format off
  virtual ~ICommand() {}
  // clang-format on
};
