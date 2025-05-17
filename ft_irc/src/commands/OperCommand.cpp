#include "commands/OperCommand.hpp"

#include <sys/socket.h>

#include "Server.hpp"
#include "numericsReplies/300-399.hpp"
#include "numericsReplies/400-499.hpp"

/**
 * @numericsReplies
 * ERR_NEEDMOREPARAMS: 引数が足りない
 * ERR_PASSWDMISMATCH: パスワードが一致しない
 * ERR_NOOPERHOST: オペレータ権限がない
 * RPL_YOUREOPER: オペレータ権限を取得した
 */
void OperCommand::execute(const commandS& cmd, Client& client, Server& server) {
  const std::string& nick =
      client.getNickname().empty() ? "*" : client.getNickname();
  if (!client.isRegistered()) {
    std::string msg = irc::numericReplies::ERR_NOTREGISTERED(nick);
    client.sendMessage(msg);
    return;
  }

  if (client.isOperator()) {
    std::string msg = irc::numericReplies::RPL_YOUREOPER(client.getNickname());
    client.sendMessage(msg);
    return;
  }

  if (cmd.args.size() < 2) {
    std::string msg =
        irc::numericReplies::ERR_NEEDMOREPARAMS(client.getNickname(), cmd.name);
    client.sendMessage(msg);
    return;
  }

  if (client.getHostname() != AUTHORIZED_HOSTS) {
    std::string msg = irc::numericReplies::ERR_NOOPERHOST(client.getNickname());
    client.sendMessage(msg);
    return;
  }

  if (cmd.args[0] != client.getUsername()) {
    std::string msg =
        irc::numericReplies::ERR_PASSWDMISMATCH(client.getNickname());
    client.sendMessage(msg);
    return;
  }

  if (cmd.args[1] != server.getServerPassword()) {
    std::string msg =
        irc::numericReplies::ERR_PASSWDMISMATCH(client.getNickname());
    client.sendMessage(msg);
    return;
  }

  client.setOperator(true);
  std::string msg = irc::numericReplies::RPL_YOUREOPER(client.getNickname());
  client.sendMessage(msg);
}
