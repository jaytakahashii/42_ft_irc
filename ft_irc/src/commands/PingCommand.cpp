#include "commands/PingCommand.hpp"

#include <sys/socket.h>

#include "Client.hpp"
#include "Server.hpp"
#include "numericsReplies/400-499.hpp"

void PingCommand::execute(const commandS& cmd, Client& client,
                          serverStateS& state) {
  if (!client.isRegistered()) {
    std::string msg = irc::numericReplies::ERR_NOTREGISTERED();
    client.sendMessage(msg);
    return;
  }

  if (cmd.args.size() < 1) {
    // state が unused なのでとりあえず表示 TODO
    client.sendMessage(state.password + ":server 461 " + client.getNickname() +
                       " PING :Not enough parameters\r\n");
    return;
  }

  std::string target = cmd.args[0];
  std::string msg = ":" + client.getNickname() + " PONG " + target + "\r\n";
  send(client.getFd(), msg.c_str(), msg.size(), 0);
}
