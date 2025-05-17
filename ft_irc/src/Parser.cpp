#include "../include/Parser.hpp"

#include <iostream>

// == OCCF ====================================================================
Parser::Parser(void) {
  return;
}
Parser::Parser(const Parser& other) {
  *this = other;
}
Parser::~Parser(void) {
  return;
}
Parser& Parser::operator=(const Parser& other) {
  if (this != &other) {
  }
  return *this;
}

// == PUBLIC ==================================================================

/*
 * @brief Parse the command from the message.
 *
 * Steps:
 * 1. Use `istringstream` to tokenize the message.
 * 2. If the message starts with ':', treat it as a prefix and skip it.
 * 3. Read the next token as the IRC command.
 * 4. Convert the command to uppercase (IRC is case-insensitive).
 * 5. Return the corresponding Command enum if matched.
 * 6. If no match is found, return Command::INVALID.
 * 7. If the command token is missing, throw `InvalidCommandException`.
 * 8. Check the argument length using `isValidArgLength`. If invalid, throw
 * `InvalidCommandException`.
 * 9. If the command requires further processing (e.g., `MODE`), handle it
 * appropriately.
 * 10. Return the `ParsedResult` containing the parsed command and its
 * arguments.
 */
commandS Parser::parseCommand(const std::string& message) {
  std::string safeMessage = handleBytes(message);

  if (safeMessage.empty())
    return commandS();

  // safeMessage = safeMessage.substr(0, safeMessage.length() - 2);
  toIrcCharacters(safeMessage);
  std::istringstream iss(safeMessage);
  commandS result;

  this->_hasPrefix = false;
  setPrefix(iss, result);
  setCommand(iss, result);
  setArguments(safeMessage, result);
  this->_hasPrefix = false;
  return result;
}

// == PRIVATE =================================================================

std::string Parser::handleBytes(const std::string& message) {
  if (message.empty() || message == "\r\n")
    return "";
  if (message.length() > MAX_BYTE)
    return "";

  // trim [\r\n] from the end of the message
  // std::string body = message.substr(0, message.length() - 2);
  for (size_t i = 0; i < message.size(); ++i) {
    if (!std::isspace(message[i]))
      return message;
  }
  return "";
}

void Parser::toIrcCharacters(std::string& message) {
  for (size_t i = 0; i < message.size(); ++i) {
    if (message[i] == '{') {
      message[i] = '[';
    } else if (message[i] == '}') {
      message[i] = ']';
    } else if (message[i] == '|') {
      message[i] = '\\';
    } else if (message[i] == '^') {
      message[i] = '~';
    }
  }
}

void Parser::setPrefix(std::istringstream& iss, commandS& result) {
  std::string token;

  iss >> token;
  if (token[0] == ':') {
    result.prefix = token.substr(1);
    this->_hasPrefix = true;
  } else {
    iss.clear();
    iss.seekg(0);
    result.prefix = "";
  }
}

void Parser::setCommand(std::istringstream& iss, commandS& result) {
  std::string token;

  iss >> token;
  for (size_t i = 0; i < token.length(); ++i) {
    if (token[i] >= 'a' && token[i] <= 'z')
      token[i] = token[i] - 'a' + 'A';
  }
  result.name = token;
}

void Parser::setArguments(std::string message, commandS& result) {
  std::string token;
  size_t position = 0;

  if (this->_hasPrefix)
    position = result.prefix.length() + 2;  // / Skip the prefix + [:, ' ']
  while (position < message.length() && message[position] != ' ')
    position++;  // Skip the command
  while (position < message.length() && message[position] == ' ')
    position++;  // Skip spaces

  while (position < message.length()) {
    if (message[position] == ':') {
      position++;
      while (position < message.length())
        token += message[position++];
      result.args.push_back(token);
      token.clear();
      break;
    } else if (message[position] == ' ') {
      result.args.push_back(token);
      token.clear();
      position++;
      continue;
    }
    token += message[position++];
  }
  if (!token.empty()) {
    result.args.push_back(token);
    token.clear();
  }
  if (result.args.size() == 0) {
    result.args.push_back("");
  }
}

/*
 * Prefix: server
 * Command: 001
 * Arguments: ["user", "Welcome to the IRC Network!"]
 */

void run(const std::string& message) {
  commandS result = Parser().parseCommand(message);

  std::cout << "=======================================" << std::endl;
  std::cout << "Message			: " << message << std::endl;
  std::cout << "Prefix			: " << result.prefix << std::endl;
  std::cout << "Parsed command		: " << result.name << std::endl;
  std::cout << "Parsed arguments	: ";
  for (size_t i = 0; i < result.args.size(); ++i) {
    std::cout << "[" << result.args[i] << "] ";
  }
  std::cout << std::endl;
}

// int main() {
//   run("NICK user123\r\n");
//   run("USER user123 0 * :Real Name\r\n");
//   run("JOIN #general\r\n");
//   run("PART #general :Goodbye!\r\n");
//   run("PRIVMSG #general :Hello, everyone!\r\n");
//   run("PRIVMSG #general :'{}^|'\r\n");
//   run("MODE #general +i\r\n");
//   run("PING server\r\n");
//   run("PONG server\r\n");
//   run("QUIT :Goodbye IRC!\r\n");
//   run("WHOIS user123\r\n");
//   run("KICK #general user123 :Reason for kick\r\n");
//   run("TOPIC #general :New Topic\r\n");
//   run("\r\n");
//   run("USER user123 0 * :Real Name");
// }
