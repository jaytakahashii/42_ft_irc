#include "commands/SquitCommand.hpp"

#include <unistd.h>

#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "numericsReplies/400-499.hpp"

void SquitCommand::execute(const commandS& cmd, Client& client,
                           Server& server) {
  if (!client.isRegistered()) {
    std::string msg = irc::numericReplies::ERR_NOTREGISTERED();
    client.sendMessage(msg);
    return;
  }

  if (!client.isOperator()) {
    std::string msg = irc::numericReplies::ERR_NOPRIVILEGES();
    client.sendMessage(msg);
    return;
  }

  if (cmd.args.size() < 2) {
    std::string msg = irc::numericReplies::ERR_NEEDMOREPARAMS(cmd.name);
    client.sendMessage(msg);
    return;
  }

  if (cmd.args[0] != server.getServerName()) {
    std::string msg = irc::numericReplies::ERR_NOSUCHSERVER(cmd.args[0]);
    client.sendMessage(msg);
    return;
  }

  server.killServer();
}
