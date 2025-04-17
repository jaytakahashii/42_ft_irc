#include "Parser.hpp"

#include <sstream>

SCommand Parser::parse(const std::string& input) {
  SCommand cmd;
  std::istringstream iss(input);
  iss >> cmd.name;
  std::string arg;
  while (iss >> arg)
    cmd.args.push_back(arg);
  return cmd;
}
