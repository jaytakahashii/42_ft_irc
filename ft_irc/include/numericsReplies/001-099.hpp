#pragma once

#include <string>

namespace irc {
namespace numericReplies {

inline std::string RPL_WELCOME(const std::string& nick, const std::string& user,
                               const std::string& host) {
  return ":server 001 :Welcome to the Internet Relay Network " + nick + "!" +
         user + "@" + host + "\r\n";
}

inline std::string RPL_YOURHOST(const std::string& servername) {
  return ":server 002 :Your host is " + servername +
         ", running version ft_irc\r\n";
}

inline std::string RPL_CREATED(const std::string& date) {
  return ":server 003 :This server was created " + date + "\r\n";
}

inline std::string ERR_NEEDMOREPARAMS(const std::string& command) {
  return "461 " + command + " :Not enough parameters\r\n";
}

inline std::string ERR_ALREADYREGISTRED() {
  return "462 :You may not reregister\r\n";
}

}  // namespace numericReplies
}  // namespace irc
