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
  } else if (cmd.name == "USER") {
    handleUser(cmd, client);
  } else if (cmd.name == "PING") {
    // handlePing(cmd, client);
  } else if (cmd.name == "JOIN") {
    // handleJoin(cmd, client);
  } else if (cmd.name == "PART") {
    // handlePart(cmd, client);
  } else if (cmd.name == "PRIVMSG") {
    // handlePrivmsg(cmd, client);
  } else if (cmd.name == "PONG") {
    // handlePong(cmd, client);
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

void CommandDispatcher::handleUser(const Command& cmd, Client& client) {
  if (cmd.args.size() < 4) {
    client.sendMessage(":server 461 " + client.getNickname() +
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
