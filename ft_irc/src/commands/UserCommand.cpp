#include "commands/UserCommand.hpp"

#include "Client.hpp"
#include "Server.hpp"
#include "numericsReplies/001-099.hpp"
#include "numericsReplies/400-499.hpp"

/**
 * * @brief UserCommand constructor
 * * @numericReplies
 * * RPL_WELCOME
 * * ERR_NEEDMOREPARAMS
 * * ERR_ALREADYREGISTRED
 * * @example
 * * USER <user> <mode> <unused> <realname>
 */
void UserCommand::execute(const commandS& cmd, Client& client, Server& server) {
  const std::string& nick =
      client.getNickname().empty() ? "*" : client.getNickname();
  if (!client.isAuthenticated()) {
    std::string msg = irc::numericReplies::ERR_NOTREGISTERED(nick);
    client.sendMessage(msg);
    return;
  }

  if (client.isRegistered()) {
    std::string msg = irc::numericReplies::ERR_ALREADYREGISTRED(nick);
    client.sendMessage(msg);
    return;
  }

  if (client.getNickname().empty()) {
    std::string msg = irc ::numericReplies::ERR_NOTREGISTERED(nick);
    client.sendMessage(msg);
    return;
  }

  if (cmd.args.size() < 4) {
    std::string msg =
        irc::numericReplies::ERR_NEEDMOREPARAMS(client.getNickname(), cmd.name);
    client.sendMessage(msg);
    return;
  }

  if (!client.isValidUsername(cmd.args[0])) {
    std::string msg = irc::numericReplies::ERR_ERRONEUSNICKNAME(
        client.getNickname(), cmd.args[0]);
    client.sendMessage(msg);
    return;
  }

  if (!client.isValidRealname(cmd.args[3])) {
    std::string msg = irc::numericReplies::ERR_ERRONEUSNICKNAME(
        client.getNickname(), cmd.args[3]);
    client.sendMessage(msg);
    return;
  }

  client.setUsername(cmd.args[0]);
  client.setRealname(cmd.args[3]);
  client.setRegistered(true);
  std::string msg = irc::numericReplies::RPL_WELCOME(
      client.getNickname(), client.getUsername(), client.getHostname());
  client.sendMessage(msg);

  // TODO unused args
  std::string serverName = server.getServerName();
  (void)serverName;
}
