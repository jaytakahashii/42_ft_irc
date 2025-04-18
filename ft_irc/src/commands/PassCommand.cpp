#include "commands/PassCommand.hpp"

#include <sys/socket.h>

#include "numericsReplies/400-499.hpp"

void PassCommand::execute(const commandS& cmd, Client& client,
                          serverStateS& state) {
  if (cmd.args.empty()) {
    std::string msg = irc::numericReplies::ERR_NEEDMOREPARAMS(cmd.name);
    client.sendMessage(msg);
    return;
  }

  if (client.isAuthenticated()) {
    std::string msg = irc::numericReplies::ERR_ALREADYREGISTRED();
    client.sendMessage(msg);
    return;
  }

  if (cmd.args[0] == state.password) {
    client.setAuthenticated(true);
  } else {
    std::string msg = irc::numericReplies::ERR_PASSWDMISMATCH();
    client.sendMessage(msg);
  }
}
