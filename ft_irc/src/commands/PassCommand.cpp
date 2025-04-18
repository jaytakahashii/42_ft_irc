#include "commands/PassCommand.hpp"

#include <sys/socket.h>

#include "numericsReplies/400-499.hpp"

void PassCommand::execute(const commandS& cmd, Client& client,
                          serverStateS& state) {
  if (cmd.args.empty()) {
    std::string msg = irc::numericReplies::ERR_NEEDMOREPARAMS(cmd.name);
    send(client.getFd(), msg.c_str(), msg.size(), 0);
    return;
  }

  if (client.isAuthenticated()) {
    std::string msg = irc::numericReplies::ERR_ALREADYREGISTRED();
    send(client.getFd(), msg.c_str(), msg.size(), 0);
    return;
  }

  if (cmd.args[0] == state.password) {
    client.setAuthenticated(true);
  } else {
    std::string msg = irc::numericReplies::ERR_PASSWDMISMATCH();
    send(client.getFd(), msg.c_str(), msg.size(), 0);
  }
}
