#include "utils/utils.hpp"

#include <iostream>

#include "utils/color.hpp"

void printError(const std::string& message) {
  std::cerr << RED "Error: " << message << RESET << std::endl;
}

char ircToLower(char c) {
  switch (c) {
    case '{':
      return '[';
    case '}':
      return ']';
    case '|':
      return '\\';
    case '^':
      return '~';
    default:
      return std::tolower(static_cast<unsigned char>(c));
  }
}

std::string ircToLower(const std::string& str) {
  std::string result;
  result.reserve(str.size());
  for (size_t i = 0; i < str.size(); ++i) {
    result += ircToLower(str[i]);
  }
  return result;
}

bool ircInsensitiveEqual(const std::string& str1, const std::string& str2) {
  if (str1.size() != str2.size()) {
    return false;
  }
  return ircToLower(str1) == ircToLower(str2);
}

std::string ircCharacters(const std::string& str) {
  std::string result;
  for (size_t i = 0; i < str.size(); ++i) {
    if (str[i] == '{') {
      result += '[';
    } else if (str[i] == '}') {
      result += ']';
    } else if (str[i] == '|') {
      result += '\\';
    } else if (str[i] == '^') {
      result += '~';
    } else {
      result += str[i];
    }
  }
  return result;
}

bool ircEqual(const std::string& str1, const std::string& str2) {
  if (str1.size() != str2.size()) {
    return false;
  }
  return ircCharacters(str1) == ircCharacters(str2);
}
