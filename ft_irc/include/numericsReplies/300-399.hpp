#pragma once

#include <string>

namespace irc {
namespace numericReplies {

// 332
inline std::string RPL_TOPIC(const std::string& channel,
                             const std::string& topic) {
  return "irc.42tokyo.jp 332 " + channel + " :" + topic + "\r\n";
}

}  // namespace numericReplies
}  // namespace irc
