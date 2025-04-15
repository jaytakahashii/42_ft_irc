#pragma once

#include <string>
#include <vector>

struct ParsedCommand {
  std::string prefix;
  std::string command;
  std::vector<std::string> params;
};

class Parser {
 public:
  static ParsedCommand parse(const std::string& line);
};
