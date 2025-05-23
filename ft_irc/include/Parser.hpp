#pragma once
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "commands/ICommand.hpp"

#define MAX_ARGS 15
#define MAX_BYTE 512

/**
 * https://tex2e.github.io/rfc-translater/html/rfc2813.html ref[3.3 Messages]
 * [prefix(option)] [command] [command parameter(up to 15)]
 *
 * in	: [:username!~user@host PRIVMSG #channel :Hello, World!]
 * out	: ParsedResult
 */

class Parser {
 private:
  bool _hasPrefix;
  std::string handleBytes(const std::string& message);
  void toIrcCharacters(std::string& message);
  void setCommand(std::istringstream& iss, commandS& result);
  void setPrefix(std::istringstream& iss, commandS& result);
  void setArguments(std::string msg, commandS& result);

 public:
  // == OCCF ========================================
  Parser();
  Parser(const Parser& other);
  Parser& operator=(const Parser& other);
  ~Parser();

  commandS parseCommand(const std::string& message);
};
