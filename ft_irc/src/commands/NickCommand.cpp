#include "commands/NickCommand.hpp"

#include <sys/socket.h>

#include "numericsReplies/400-499.hpp"

/**
 * @numericsReplies
 * ERR_NONICKNAMEGIVEN: 引数なし
 * ERR_ERRONEUSNICKNAME: invalid nickname
 * ERR_NICKNAMEINUSE: already used
 * ERR_UNAVAILRESOURCE: blocked by the nick delay mechanism.
 * ERR_RESTRICTED: nickname is restricted.
 */
void NickCommand::execute(const commandS& cmd, Client& client,
                          serverStateS& state) {
  // before PASS command
  if (!client.isAuthenticated()) {
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
  if (!client.isValidNickname(nickname)) {
    std::string msg = irc::numericReplies::ERR_ERRONEUSNICKNAME(nickname);
    client.sendMessage(msg);
    client.sendMessage(state.host);  // unused params error対策
    return;
  }

  client.setNickname(nickname);
}
