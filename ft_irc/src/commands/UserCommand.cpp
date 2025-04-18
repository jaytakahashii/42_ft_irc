#include "commands/UserCommand.hpp"

#include <sys/socket.h>

#include "Client.hpp"
#include "Server.hpp"

void UserCommand::execute(const SCommand& cmd, Client& client,
                          ServerState& state) {
  if (cmd.args.size() < 4) {
    // state が unused なのでとりあえず表示 TODO
    client.sendMessage(state.password + ":server 461 " + client.getNickname() +
                       " USER :Not enough parameters\r\n");
    return;
  }

  client.setUsername(cmd.args[0]);
  client.setRealname(cmd.args[3]);

  if (client.isFullyRegistered()) {
    std::string welcomeMsg =
        ":server 001 " + client.getNickname() + " :Welcome to the IRC server " +
        client.getNickname() + "!" + client.getUsername() + "@localhost\r\n";
    client.sendMessage(welcomeMsg);
  }
}
