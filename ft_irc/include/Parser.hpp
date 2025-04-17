#pragma once
#include "SCommand.hpp"

class Parser {
 public:
  SCommand parse(const std::string& input);
};
