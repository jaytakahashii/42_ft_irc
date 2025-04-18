#pragma once

#include <string>

namespace irc {
namespace numericReplies {

inline std::string ERR_NEEDMOREPARAMS(const std::string& command) {
  return ":server 461 " + command + " :Not enough parameters\r\n";
}

inline std::string ERR_ALREADYREGISTRED() {
  return ":server 462 :Unauthorized command (already registered)\r\n";
}

inline std::string ERR_PASSWDMISMATCH() {
  return ":server 464 :Password incorrect\r\n";
}

}  // namespace numericReplies
}  // namespace irc
