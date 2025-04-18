#include "commands/NickCommand.hpp"

#include <sys/socket.h>

#include "numericsReplies/400-499.hpp"

void NickCommand::execute(const commandS& cmd, Client& client,
                          serverStateS& state) {
  if (!client.isAuthenticated()) {
    // TODO: ?
    std::string msg = irc::numericReplies::ERR_NOTREGISTERED();
    client.sendMessage(msg);
    return;
  }

  if (cmd.args.empty()) {
    std::string msg = irc::numericReplies::ERR_NONICKNAMEGIVEN();
    client.sendMessage(msg);
    return;
  }

  std::string nickname = cmd.args[0];
  if (client.isValidNickname(nickname) == false) {
    std::string msg = irc::numericReplies::ERR_ERRONEUSNICKNAME(nickname);
    client.sendMessage(msg);
    client.sendMessage(state.host);  // unused params error対策
    return;
  }

  client.setNickname(nickname);
}
