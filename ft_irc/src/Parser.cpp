#include "Parser.hpp"

#include <sstream>

ParsedCommand Parser::parse(const std::string& line) {
  ParsedCommand cmd;
  std::istringstream iss(line);
  std::string token;

  if (line[0] == ':') {
    iss >> token;
    cmd.prefix = token.substr(1);
  }

  if (iss >> token)
    cmd.command = token;

  while (iss >> token)
    cmd.params.push_back(token);

  return cmd;
}
