#pragma once

#include <string>

namespace irc {
namespace numericReplies {

// 501
inline std::string ERR_UMODEUNKNOWNFLAG() {
  return ":irc.42tokyo.jp 501 :Unknown MODE flag\r\n";
}

// 502
inline std::string ERR_USERSDONTMATCH() {
  return ":irc.42tokyo.jp 502 :Cannot change mode for other users\r\n";
}

}  // namespace numericReplies
}  // namespace irc
