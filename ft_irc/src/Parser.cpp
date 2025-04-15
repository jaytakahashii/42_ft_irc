#include "Parser.hpp"

#include <sstream>

Parser::Parser() {
}

Parser::~Parser() {
}

Command Parser::parse(const std::string& input) {
  Command cmd;
  std::istringstream iss(input);
  iss >> cmd.name;
  std::string arg;
  while (iss >> arg)
    cmd.args.push_back(arg);
  return cmd;
}
