#include "commands/SquitCommand.hpp"

#include <unistd.h>

#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "numericsReplies/400-499.hpp"

void SquitCommand::execute(const commandS& cmd, Client& client,
                           Server& server) {
  const std::string& nick =
      client.getNickname().empty() ? "*" : client.getNickname();
  if (!client.isRegistered()) {
    std::string msg = irc::numericReplies::ERR_NOTREGISTERED(nick);
    client.sendMessage(msg);
    return;
  }

  if (!client.isOperator()) {
    std::string msg = irc::numericReplies::ERR_NOPRIVILEGES(nick);
    client.sendMessage(msg);
    return;
  }

  if (cmd.args.size() < 2) {
    std::string msg =
        irc::numericReplies::ERR_NEEDMOREPARAMS(client.getNickname(), cmd.name);
    client.sendMessage(msg);
    return;
  }

  if (cmd.args[0] != server.getServerName()) {
    std::string msg = irc::numericReplies::ERR_NOSUCHSERVER(
        client.getNickname(), cmd.args[0]);
    client.sendMessage(msg);
    return;
  }

  server.killServer();
}
