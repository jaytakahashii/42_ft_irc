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
  if (!client.isRegistered()) {
    std::string msg = irc::numericReplies::ERR_NOTREGISTERED();
    client.sendMessage(msg);
    return;
  }

  if (client.isOperator()) {
    std::string msg = irc::numericReplies::ERR_ALREADYREGISTRED();
    client.sendMessage(msg);
    return;
  }

  if (cmd.args.size() < 2) {
    std::string msg = irc::numericReplies::ERR_NEEDMOREPARAMS(cmd.name);
    client.sendMessage(msg);
    return;
  }

  if (client.getHostname() != AUTHORIZED_HOSTS) {
    std::string msg = irc::numericReplies::ERR_NOOPERHOST();
    client.sendMessage(msg);
    return;
  }

  if (cmd.args[0] != server.getServerPassword()) {
    std::string msg = irc::numericReplies::ERR_PASSWDMISMATCH();
    client.sendMessage(msg);
    return;
  }

  client.setOperator(true);
  std::string msg = irc::numericReplies::RPL_YOUREOPER();
}
