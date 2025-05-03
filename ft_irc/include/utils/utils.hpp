#pragma once

#include <string>

void printError(const std::string& message);
std::string toIrcCharacters(const std::string& str);
bool ircEqual(const std::string& str1, const std::string& str2,
              bool insensitive = false);
void ft_send(int fd, const std::string& message);
