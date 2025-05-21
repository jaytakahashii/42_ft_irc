#pragma once

#include <string>

namespace irc {
namespace numericReplies {

// 221
inline std::string RPL_UMODEIS(const std::string& nick,
                               const std::string& userModeString) {
  return ":irc.42tokyo.jp 221 " + nick + " " + userModeString + "\r\n";
}

}  // namespace numericReplies
}  // namespace irc
