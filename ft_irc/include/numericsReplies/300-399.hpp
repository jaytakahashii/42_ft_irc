#pragma once

#include <string>

namespace irc {
namespace numericReplies {

// 324
inline std::string RPL_CHANNELMODEIS(const std::string& nick,
                                     const std::string& channel,
                                     const std::string& mode,
                                     const std::string& params) {
  return ":" + SERVER_NAME + " 324 " + nick + " " + channel + " " + mode + " " +
         params + "\r\n";
}

// 332
inline std::string RPL_TOPIC(const std::string& nick,
                             const std::string& channel,
                             const std::string& topic) {
  return ":" + SERVER_NAME + " 332 " + nick + " " + channel + " :" + topic +
         "\r\n";
}

// 341
inline std::string RPL_INVITING(const std::string& nick,
                                const std::string& target,
                                const std::string& channel) {
  return ":" + SERVER_NAME + " 341 " + nick + " " + target + " " + channel +
         "\r\n";
}

// 353
inline std::string RPL_NAMREPLY(const std::string& nick,
                                const std::string& symbol,
                                const std::string& channel,
                                const std::string& names) {
  return ":" + SERVER_NAME + " 353 " + nick + " " + symbol + " " + channel +
         " :" + names + "\r\n";
}

// 367
inline std::string RPL_BANLIST(const std::string& nick,
                               const std::string& channel,
                               const std::string& banmask,
                               const std::string& who,
                               const std::string& time) {
  return ":" + SERVER_NAME + " 367 " + nick + " " + channel + " " + banmask +
         " " + who + " " + time + "\r\n";
}

// 368
inline std::string RPL_ENDOFBANLIST(const std::string& nick,
                                    const std::string& channel) {
  return ":" + SERVER_NAME + " 368 " + nick + " " + channel +
         " :End of channel ban list\r\n";
}

// 346
inline std::string RPL_INVITELIST(const std::string& nick,
                                  const std::string& channel,
                                  const std::string& invitemask) {
  return ":" + SERVER_NAME + " 346 " + nick + " " + channel + " " + invitemask +
         "\r\n";
}

// 347
inline std::string RPL_ENDOFINVITELIST(const std::string& nick,
                                       const std::string& channel) {
  return ":" + SERVER_NAME + " 347 " + nick + " " + channel +
         " :End of channel invite list\r\n";
}

// 348
inline std::string RPL_EXCEPTLIST(const std::string& nick,
                                  const std::string& channel,
                                  const std::string& exceptmask) {
  return ":" + SERVER_NAME + " 348 " + nick + " " + channel + " " + exceptmask +
         "\r\n";
}

// 349
inline std::string RPL_ENDOFEXCEPTLIST(const std::string& nick,
                                       const std::string& channel) {
  return ":" + SERVER_NAME + " 349 " + nick + " " + channel +
         " :End of channel exception list\r\n";
}

// 381
inline std::string RPL_YOUREOPER(const std::string& nick) {
  return ":" + SERVER_NAME + " 381 " + nick +
         " :You are now an IRC operator\r\n";
}

// 325
inline std::string RPL_UNIQOPIS(const std::string& nick,
                                const std::string& channel,
                                const std::string& nickname) {
  return ":" + SERVER_NAME + " 325 " + nick + " " + channel + " " + nickname +
         "\r\n";
}

}  // namespace numericReplies
}  // namespace irc
