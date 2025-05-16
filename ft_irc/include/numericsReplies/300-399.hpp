#pragma once

#include <string>

namespace irc {
namespace numericReplies {

// 324
inline std::string RPL_CHANNELMODEIS(const std::string& nick,
                                     const std::string& channel,
                                     const std::string& mode,
                                     const std::string& params) {
  return ":irc.42tokyo.jp 324 " + nick + " " + channel + " " + mode + " " +
         params + "\r\n";
}

// 332
inline std::string RPL_TOPIC(const std::string& nick,
                             const std::string& channel,
                             const std::string& topic) {
  return ":irc.42tokyo.jp 332 " + nick + " " + channel + " :" + topic + "\r\n";
}

// 381
inline std::string RPL_YOUREOPER(const std::string& nick) {
  return ":irc.42tokyo.jp 381 " + nick + " :You are now an IRC operator\r\n";
}

}  // namespace numericReplies
}  // namespace irc
