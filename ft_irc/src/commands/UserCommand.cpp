#include "commands/UserCommand.hpp"

#include "Client.hpp"
#include "Server.hpp"
#include "numericsReplies/001-099.hpp"
#include "numericsReplies/400-499.hpp"

void UserCommand::execute(const commandS& cmd, Client& client,
                          serverStateS& state) {
  if (client.isRegistered()) {
    std::string msg = irc::numericReplies::ERR_ALREADYREGISTRED();
    client.sendMessage(msg);
  }

  if (client.getNickname().empty()) {
    std::string msg = irc ::numericReplies::ERR_NOTREGISTERED();
    client.sendMessage(msg);
    return;
  }

  if (cmd.args.size() != 4) {
    std::string msg = irc::numericReplies::ERR_NEEDMOREPARAMS(cmd.name);
    client.sendMessage(msg);
    return;
  }

  if (!client.isValidUsername(cmd.args[0])) {
    // TODO
    std::string msg = irc::numericReplies::ERR_ERRONEUSNICKNAME(cmd.args[0]);
    client.sendMessage(msg);
    return;
  }
  if (client.isValidRealname(cmd.args[3]) == false) {
    // TODO
    std::string msg = irc::numericReplies::ERR_ERRONEUSNICKNAME(cmd.args[3]);
    client.sendMessage(msg);
    return;
  }
  client.setUsername(cmd.args[0]);
  std::string realName;
  for (size_t i = 3; i < cmd.args.size(); ++i) {
    if (i != 3) {
      realName += " ";
    }
    realName += cmd.args[i];
  }
  client.setRealname(realName);
  client.setRegistered(true);
  std::string msg = irc::numericReplies::RPL_WELCOME(
      client.getNickname(), client.getUsername(), state.host);
  client.sendMessage(msg);
}
