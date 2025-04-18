#include "commands/NickCommand.hpp"

#include <sys/socket.h>

#include "numericsReplies/400-499.hpp"

void NickCommand::execute(const commandS& cmd, Client& client,
                          serverStateS& state) {
  if (!client.isAuthenticated()) {
    // TODO: ?
    std::string msg = irc::numericReplies::ERR_NOTREGISTERED();
    send(client.getFd(), msg.c_str(), msg.size(), 0);
    return;
  }

  if (cmd.args.empty()) {
    std::string msg = irc::numericReplies::ERR_NONICKNAMEGIVEN();
    send(client.getFd(), msg.c_str(), msg.size(), 0);
    return;
  }

  std::string nickname = cmd.args[0];
  if (client.isValidNickname(nickname) == false) {
    std::string msg = irc::numericReplies::ERR_ERRONEUSNICKNAME(nickname);
    send(client.getFd(), msg.c_str(), msg.size(), 0);
    return;
  }

  client.setNickname(nickname);
}
