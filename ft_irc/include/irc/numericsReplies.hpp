#pragma once

#include <string>

namespace irc {
namespace numericReplies {

inline std::string ERR_NEEDMOREPARAMS(const std::string& command) {
  return "461 " + command + " :Not enough parameters\r\n";
}

inline std::string ERR_ALREADYREGISTRED() {
  return "462 :You may not reregister\r\n";
}

}  // namespace numericReplies
}  // namespace irc
