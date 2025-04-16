#pragma once
#include <string>
#include <vector>

struct ICommand {
  std::string name;               // a.g. "NICK", "USER", "JOIN", etc.
  std::vector<std::string> args;  // arguments for the command
};
