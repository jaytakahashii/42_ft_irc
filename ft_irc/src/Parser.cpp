#include "Parser.hpp"

#include <sstream>

ICommand Parser::parse(const std::string& input) {
  ICommand cmd;
  std::istringstream iss(input);
  iss >> cmd.name;
  std::string arg;
  while (iss >> arg)
    cmd.args.push_back(arg);
  return cmd;
}
