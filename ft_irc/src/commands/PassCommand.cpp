#include "commands/PassCommand.hpp"

#include <sys/socket.h>

#include "Server.hpp"
#include "numericsReplies/400-499.hpp"

void PassCommand::execute(const commandS& cmd, Client& client, Server& server) {
  const std::string& nick =
      client.getNickname().empty() ? "*" : client.getNickname();
  if (cmd.args.empty()) {
    std::string msg = irc::numericReplies::ERR_NEEDMOREPARAMS(nick, cmd.name);
    client.sendMessage(msg);
    return;
  }

  if (client.isAuthenticated()) {
    std::string msg = irc::numericReplies::ERR_ALREADYREGISTRED(nick);
    client.sendMessage(msg);
    return;
  }

  std::string password = server.getServerPassword();
  if (cmd.args[0] == password) {
    client.setAuthenticated(true);
  } else {
    std::string msg = irc::numericReplies::ERR_PASSWDMISMATCH(nick);
    client.sendMessage(msg);
  }
}
