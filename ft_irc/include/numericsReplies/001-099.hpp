#pragma once

#include <iostream>
#include <string>

namespace irc {
namespace numericReplies {

// 001
inline std::string RPL_WELCOME(const std::string& nick, const std::string& user,
                               const std::string& host) {
  return ":irc.42tokyo.jp 001 " + nick +
         " :Welcome to the Internet Relay Network " + nick + "!" + user + "@" +
         host + "\r\n";
}

// 002
inline std::string RPL_YOURHOST(const std::string& nick,
                                const std::string& servername) {
  return ":irc.42tokyo.jp 002 " + nick + " :Your host is " + servername +
         ", running version ft_irc\r\n";
}

// 003
inline std::string RPL_CREATED(const std::string& nick,
                               const std::string& date) {
  return ":irc.42tokyo.jp 003 " + nick + " :This server was created " + date +
         "\r\n";
}

}  // namespace numericReplies
}  // namespace irc
