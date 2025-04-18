#pragma once
#include "commands/ICommand.hpp"

class Parser {
 public:
  commandS parse(const std::string& input);
};
