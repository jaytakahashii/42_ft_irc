#pragma once

#include <string>

namespace irc {
namespace numericReplies {

// 001
inline std::string RPL_WELCOME(const std::string& nick, const std::string& user,
                               const std::string& host) {
  return ":" + SERVER_NAME + " 001 " + nick +
         " :Welcome to the Internet Relay Network " + nick + "!" + user + "@" +
         host + "\r\n";
}

// 002
inline std::string RPL_YOURHOST(const std::string& nick,
                                const std::string& servername) {
  return ":" + SERVER_NAME + " 002 " + nick + " :Your host is " + servername +
         ", running version ft_irc\r\n";
}

// 003
inline std::string RPL_CREATED(const std::string& nick,
                               const std::string& date) {
  return ":" + SERVER_NAME + " 003 " + nick + " :This server was created " +
         date + "\r\n";
}

// 004
inline std::string RPL_MYINFO(const std::string& nick,
                              const std::string& servername,
                              const std::string& version,
                              const std::string& userModes,
                              const std::string& channelModes) {
  return ":" + SERVER_NAME + " 004 " + nick + " " + servername + " " + version +
         " " + userModes + " " + channelModes + "\r\n";
}

}  // namespace numericReplies
}  // namespace irc
