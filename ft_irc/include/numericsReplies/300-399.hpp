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

// 341
inline std::string RPL_INVITING(const std::string& nick,
                               const std::string& target,
                               const std::string& channel) {
  return ":irc.42tokyo.jp 341 " + nick + " " + target + " " + channel + "\r\n";
}

// 381
inline std::string RPL_YOUREOPER(const std::string& nick) {
  return ":irc.42tokyo.jp 381 " + nick + " :You are now an IRC operator\r\n";
}

}  // namespace numericReplies
}  // namespace irc
