#pragma once

#include <string>

namespace irc {
namespace numericReplies {

// 501
inline std::string ERR_UMODEUNKNOWNFLAG(const std::string& nick) {
  return ":irc.42tokyo.jp 501 " + nick + " :Unknown MODE flag\r\n";
}

// 502
inline std::string ERR_USERSDONTMATCH(const std::string& nick) {
  return ":irc.42tokyo.jp 502 " + nick +
         " :Cannot change mode for other users\r\n";
}

}  // namespace numericReplies
}  // namespace irc
