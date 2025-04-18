#include "commands/NickCommand.hpp"

#include <sys/socket.h>

void NickCommand::execute(const commandS& cmd, Client& client,
                          serverStateS& state) {
  if (cmd.args.empty()) {
    // state が unused なのでとりあえず表示 TODO
    std::string msg = state.password + ":server 431 * :No nickname given\r\n";
    send(client.getFd(), msg.c_str(), msg.size(), 0);
    return;
  }

  std::string nickname = cmd.args[0];
  client.setNickname(nickname);

  std::string msg =
      ":server 001 " + nickname + " :Welcome to the IRC server\r\n";
  send(client.getFd(), msg.c_str(), msg.size(), 0);
}
