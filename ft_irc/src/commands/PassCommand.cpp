#include "commands/PassCommand.hpp"

#include <sys/socket.h>

void PassCommand::execute(const commandS& cmd, Client& client,
                          serverStateS& state) {
  if (cmd.args.empty()) {
    std::string msg = ":server 461 " + client.getNickname() +
                      " PASS :Not enough parameters\r\n";
    send(client.getFd(), msg.c_str(), msg.size(), 0);
    return;
  }

  if (client.isAuthenticated()) {
    std::string msg = ":server 462 " + client.getNickname() +
                      " PASS :You may not reregister\r\n";
    send(client.getFd(), msg.c_str(), msg.size(), 0);
    return;
  }

  if (cmd.args[0] == state.password) {
    client.setAuthenticated(true);
  } else {
    std::string msg = ":server 464 Password incorrect\r\n";
    send(client.getFd(), msg.c_str(), msg.size(), 0);
  }
}
