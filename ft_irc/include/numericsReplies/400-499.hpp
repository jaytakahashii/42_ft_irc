#pragma once

#include <string>

namespace irc {
namespace numericReplies {

// 402
inline std::string ERR_NOSUCHSERVER(const std::string& serverName) {
  return ":server 402 " + serverName + " :No such server\r\n";
}

// 403
inline std::string ERR_NOSUCHCHANNEL(const std::string& channel) {
  return ":server 403 " + channel + " :No such channel\r\n";
}

// 405
inline std::string ERR_TOOMANYCHANNELS(const std::string& channel) {
  return ":server 405 " + channel + " :You have joined too many channels\r\n";
}

// 407
inline std::string ERR_TOOMANYTARGETS(const std::string& target,
                                      const std::string& errorCode,
                                      const std::string& abortMessage) {
  return ":server 407 " + target + " :" + errorCode + " recipients. " +
         abortMessage + "\r\n";
}

// 431
inline std::string ERR_NONICKNAMEGIVEN() {
  return ":server 431 :No nickname given\r\n";
}

// 432
inline std::string ERR_ERRONEUSNICKNAME(const std::string& nickname) {
  return ":server 432 " + nickname + " :Erroneous nickname\r\n";
}

// 433
inline std::string ERR_NICKNAMEINUSE(const std::string& nickname) {
  return ":server 433 " + nickname + " :Nickname is already in use\r\n";
}

// 436
inline std::string ERR_NICKCOLLISION(const std::string& nickname,
                                     const std::string& user,
                                     const std::string& host) {
  return ":server 436 " + nickname + " :Nickname collision KILL from " + user +
         "@" + host + "\r\n";
}

// 437
inline std::string ERR_UNAVAILRESOURCE(const std::string& nickname,
                                       const std::string& channel) {
  return ":server 437 " + nickname + "/" + channel +
         " :Nick/channel is temporarily unavailable\r\n";
}

// 451
inline std::string ERR_NOTREGISTERED() {
  return ":server 451 :You have not registered\r\n";
}

// 461
inline std::string ERR_NEEDMOREPARAMS(const std::string& command) {
  return ":server 461 " + command + " :Not enough parameters\r\n";
}

// 462
inline std::string ERR_ALREADYREGISTRED() {
  return ":server 462 :Unauthorized command (already registered)\r\n";
}

// 464
inline std::string ERR_PASSWDMISMATCH() {
  return ":server 464 :Password incorrect\r\n";
}

// 471
inline std::string ERR_CHANNELISFULL(const std::string& channel) {
  return ":server 471 " + channel + " :Cannot join channel (+l)\r\n";
}

// 473
inline std::string ERR_INVITEONLYCHAN(const std::string& channel) {
  return ":server 473 " + channel + " :Cannot join channel (+i)\r\n";
}

// 474
inline std::string ERR_BANNEDFROMCHAN(const std::string& channel) {
  return ":server 474 " + channel + " :Cannot join channel (+b)\r\n";
}

// 475
inline std::string ERR_BADCHANNELKEY(const std::string& channel) {
  return ":server 475 " + channel + " :Cannot join channel (+k)\r\n";
}

// 476
inline std::string ERR_BADCHANMASK(const std::string& channel) {
  return ":server 476 " + channel + " :Bad Channel Mask\r\n";
}

// 481
inline std::string ERR_NOPRIVILEGES(const std::string& mode) {
  return ":server 481 :Permission Denied - You're not an IRC operator\r\n";
}

// 484
inline std::string ERR_RESTRICTED() {
  return ":server 484 :Your connection is restricted!\r\n";
}

}  // namespace numericReplies
}  // namespace irc
