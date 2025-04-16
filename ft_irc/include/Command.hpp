#pragma once
#include <string>
#include <vector>

struct Command {
  std::string name;               // a.g. "NICK", "USER", "JOIN", etc.
  std::vector<std::string> args;  // arguments for the command
};
