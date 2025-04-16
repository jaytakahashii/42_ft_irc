#include "CommandDispatcher.hpp"

#include <sys/socket.h>

#include <iostream>

CommandDispatcher::CommandDispatcher() {
}

CommandDispatcher::~CommandDispatcher() {
}

void CommandDispatcher::dispatch(const Command& cmd, Client& client) {
  std::cout << "Dispatching command: " << cmd.name << " from client "
            << client.getFd() << std::endl;

  if (cmd.name == "NICK") {
    handleNick(cmd, client);
  }
  // 他のコマンドもここに追加予定
}

void CommandDispatcher::handleNick(const Command& cmd, Client& client) {
  if (cmd.args.empty()) {
    std::string msg = ":server 431 * :No nickname given\r\n";
    send(client.getFd(), msg.c_str(), msg.size(), 0);
    return;
  }

  std::string nickname = cmd.args[0];
  client.setNickname(nickname);

  std::string msg =
      ":server 001 " + nickname + " :Welcome to the IRC server\r\n";
  send(client.getFd(), msg.c_str(), msg.size(), 0);
}
