#pragma once
#include "ICommand.hpp"

class Parser {
 public:
  ICommand parse(const std::string& input);
};
