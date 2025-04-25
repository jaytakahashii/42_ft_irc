#pragma once

#include <string>

void printError(const std::string& message);
char ircToLower(char c);
std::string ircToLower(const std::string& str);
std::string ircCharacters(const std::string& str);
bool ircEqual(const std::string& str1, const std::string& str2);
bool ircInsensitiveEqual(const std::string& str1, const std::string& str2);
