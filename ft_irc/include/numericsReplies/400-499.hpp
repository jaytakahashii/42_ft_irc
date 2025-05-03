#pragma once

#include <string>

namespace irc {
namespace numericReplies {

// 402
inline std::string ERR_NOSUCHSERVER(const std::string& nick,
                                    const std::string& serverName) {
  return ":irc.42tokyo.jp 402 " + nick + " " + serverName +
         " :No such server\r\n";
}

// 403
inline std::string ERR_NOSUCHCHANNEL(const std::string& nick,
                                     const std::string& channel) {
  return ":irc.42tokyo.jp 403 " + nick + " " + channel +
         " :No such channel\r\n";
}

// 404
inline std::string ERR_CANNOTSENDTOCHAN(const std::string& nick,
                                        const std::string& channel) {
  return ":irc.42tokyo.jp 404 " + nick + " " + channel +
         " :Cannot send to channel\r\n";
}

// 405
inline std::string ERR_TOOMANYCHANNELS(const std::string& nick,
                                       const std::string& channel) {
  return ":irc.42tokyo.jp 405 " + nick + " " + channel +
         " :You have joined too many channels\r\n";
}

// 407
inline std::string ERR_TOOMANYTARGETS(const std::string& nick,
                                      const std::string& target,
                                      const std::string& errorCode,
                                      const std::string& abortMessage) {
  return ":irc.42tokyo.jp 407 " + nick + " " + target + " :" + errorCode +
         " recipients. " + abortMessage + "\r\n";
}

// 411
inline std::string ERR_NORECIPIENT(const std::string& nick,
                                   const std::string& command) {
  return ":irc.42tokyo.jp 411 " + nick + " " + command +
         " :No recipient given\r\n";
}

// 421
inline std::string ERR_UNKNOWNCOMMAND(const std::string& nick,
                                      const std::string& command) {
  return ":irc.42tokyo.jp 421 " + nick + " " + command +
         " :Unknown command\r\n";
}

// 431
inline std::string ERR_NONICKNAMEGIVEN(const std::string& nick) {
  return ":irc.42tokyo.jp 431 " + nick + " :No nickname given\r\n";
}

// 432
inline std::string ERR_ERRONEUSNICKNAME(const std::string& nick,
                                        const std::string& nickname) {
  return ":irc.42tokyo.jp 432 " + nick + " " + nickname +
         " :Erroneous nickname\r\n";
}

// 433
inline std::string ERR_NICKNAMEINUSE(const std::string& nick,
                                     const std::string& nickname) {
  return ":irc.42tokyo.jp 433 " + nick + " " + nickname +
         " :Nickname is already in use\r\n";
}

// 436
inline std::string ERR_NICKCOLLISION(const std::string& nick,
                                     const std::string& nickname,
                                     const std::string& user,
                                     const std::string& host) {
  return ":irc.42tokyo.jp 436 " + nick + " " + nickname +
         " :Nickname collision KILL from " + user + "@" + host + "\r\n";
}

// 437
inline std::string ERR_UNAVAILRESOURCE(const std::string& nick,
                                       const std::string& nickname,
                                       const std::string& channel) {
  return ":irc.42tokyo.jp 437 " + nick + " " + nickname + "/" + channel +
         " :Nick/channel is temporarily unavailable\r\n";
}

// 442
inline std::string ERR_NOTONCHANNEL(const std::string& nick,
                                    const std::string& channel) {
  return ":irc.42tokyo.jp 442 " + nick + " " + channel +
         " :You're not on that channel\r\n";
}

// 451
inline std::string ERR_NOTREGISTERED(const std::string& nick) {
  return ":irc.42tokyo.jp 451 " + nick + " :You have not registered\r\n";
}

// 461
inline std::string ERR_NEEDMOREPARAMS(const std::string& nick,
                                      const std::string& command) {
  return ":irc.42tokyo.jp 461 " + nick + " " + command +
         " :Not enough parameters\r\n";
}

// 462
inline std::string ERR_ALREADYREGISTRED(const std::string& nick) {
  return ":irc.42tokyo.jp 462 " + nick +
         " :Unauthorized command (already registered)\r\n";
}

// 464
inline std::string ERR_PASSWDMISMATCH(const std::string& nick) {
  return ":server 464 " + nick + " :Password incorrect\r\n";
}

// 471
inline std::string ERR_CHANNELISFULL(const std::string& nick,
                                     const std::string& channel) {
  return ":irc.42tokyo.jp 471 " + nick + " " + channel +
         " :Cannot join channel (+l)\r\n";
}

// 472
inline std::string ERR_UNKNOWNMODE(const std::string& nick, char mode,
                                   const std::string& channel) {
  return ":irc.42tokyo.jp 472 " + nick + " " + std::string(1, mode) +
         " :is unknown mode char to me for " + channel + "\r\n";
}

// 473
inline std::string ERR_INVITEONLYCHAN(const std::string& nick,
                                      const std::string& channel) {
  return ":irc.42tokyo.jp 473 " + nick + " " + channel +
         " :Cannot join channel (+i)\r\n";
}

// 474
inline std::string ERR_BANNEDFROMCHAN(const std::string& nick,
                                      const std::string& channel) {
  return ":irc.42tokyo.jp 474 " + nick + " " + channel +
         " :Cannot join channel (+b)\r\n";
}

// 475
inline std::string ERR_BADCHANNELKEY(const std::string& nick,
                                     const std::string& channel) {
  return ":irc.42tokyo.jp 475 " + nick + " " + channel +
         " :Cannot join channel (+k)\r\n";
}

// 476
inline std::string ERR_BADCHANMASK(const std::string& nick,
                                   const std::string& channel) {
  return ":irc.42tokyo.jp 476 " + nick + " " + channel +
         " :Bad Channel Mask\r\n";
}

// 481
inline std::string ERR_NOPRIVILEGES(const std::string& nick) {
  return ":irc.42tokyo.jp 481 " + nick +
         " :Permission Denied - You're not an IRC "
         "operator\r\n";
}

// 482
inline std::string ERR_CHANOPRIVSNEEDED(const std::string& nick,
                                        const std::string& channel) {
  return ":irc.42tokyo.jp 482 " + nick + " :" + channel +
         " :You're not channel operator\r\n";
}

// 484
inline std::string ERR_RESTRICTED(const std::string& nick) {
  return ":irc.42tokyo.jp 484 " + nick + " :Your connection is restricted!\r\n";
}

// 491
inline std::string ERR_NOOPERHOST(const std::string& nick) {
  return ":irc.42tokyo.jp 491 " + nick + " :No O-lines for your host\r\n";
}

}  // namespace numericReplies
}  // namespace irc
