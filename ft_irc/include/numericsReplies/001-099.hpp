#pragma once

#include <string>

namespace irc {
namespace numericReplies {

inline std::string RPL_WELCOME(const std::string& nick, const std::string& user,
                               const std::string& host) {
  return ":irc.42tokyo.jp Welcome to the Internet Relay Network " + nick + "!" +
         user + "@" + host + "\r\n";
}

inline std::string RPL_YOURHOST(const std::string& servername) {
  return ":irc.42tokyo.jp Your host is " + servername +
         ", running version ft_irc\r\n";
}

inline std::string RPL_CREATED(const std::string& date) {
  return ":irc.42tokyo.jp This server was created " + date + "\r\n";
}

}  // namespace numericReplies
}  // namespace irc
