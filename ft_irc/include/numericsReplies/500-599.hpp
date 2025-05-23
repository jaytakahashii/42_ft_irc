#pragma once

#include <string>

namespace irc {
namespace numericReplies {

// 501
inline std::string ERR_UMODEUNKNOWNFLAG(const std::string& nick,
                                        const std::string& flag) {
  return ":" + SERVER_NAME + " 501 " + nick + " " + flag +
         " :Unknown MODE flag\r\n";
}

// 502
inline std::string ERR_USERSDONTMATCH(const std::string& nick) {
  return ":" + SERVER_NAME + " 502 " + nick +
         " :Cannot change mode for other users\r\n";
}

}  // namespace numericReplies
}  // namespace irc
