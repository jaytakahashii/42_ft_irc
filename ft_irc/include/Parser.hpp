#pragma once
#include "Command.hpp"

class Parser {
 public:
  Parser();
  ~Parser();

  Command parse(const std::string& input);
};
