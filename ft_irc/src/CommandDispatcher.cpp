#include "CommandDispatcher.hpp"

#include <sys/socket.h>

#include <iostream>

CommandDispatcher::CommandDispatcher() {
}

CommandDispatcher::~CommandDispatcher() {
}

void CommandDispatcher::dispatch(const ICommand& cmd, Client& client) {
  std::cout << "Dispatching command: " << cmd.name << " from client "
            << client.getFd() << std::endl;

  if (cmd.name == "NICK") {
    handleNick(cmd, client);
  } else if (cmd.name == "USER") {
    handleUser(cmd, client);
  } else if (cmd.name == "PING") {
    handlePing(cmd, client);
  } else if (cmd.name == "JOIN") {
    handleJoin(cmd, client);
  } else if (cmd.name == "PART") {
    // handlePart(cmd, client);
  } else if (cmd.name == "PRIVMSG") {
    // handlePrivmsg(cmd, client);
  } else if (cmd.name == "PONG") {
    // handlePong(cmd, client);
  } else {
    std::string msg = ":server 421 " + client.getNickname() + " " + cmd.name +
                      " :Unknown command\r\n";
    send(client.getFd(), msg.c_str(), msg.size(), 0);
  }
  // 他のコマンドもここに追加予定
}

void CommandDispatcher::handleNick(const ICommand& cmd, Client& client) {
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

void CommandDispatcher::handleUser(const ICommand& cmd, Client& client) {
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

void CommandDispatcher::handlePing(const ICommand& cmd, Client& client) {
  if (cmd.args.empty()) {
    client.sendMessage(":server PONG :No arguments provided\r\n");
    return;
  }

  std::string pongMsg = ":server PONG " + cmd.args[0] + "\r\n";
  client.sendMessage(pongMsg);
}

void CommandDispatcher::handleJoin(const ICommand& cmd, Client& client) {
  if (cmd.args.empty()) {
    client.sendMessage("461 JOIN :Not enough parameters\r\n");
    return;
  }

  std::string channelName = cmd.args[0];
  // チャンネル名の先頭が '#' でない場合はエラー
  if (channelName.empty() || channelName[0] != '#') {
    client.sendMessage(":server 403 " + client.getNickname() + " " +
                       channelName + " :No such channel\r\n");
    return;
  }

  // チャンネルが存在しない場合は新規作成
  if (_channels.find(channelName) == _channels.end()) {
    _channels[channelName] = new Channel(channelName);
  }

  // チャンネルに参加する
  Channel* channel = _channels[channelName];  // チャンネルを取得

  // すでに参加している場合はエラー
  if (!channel->hasClient(&client)) {
    channel->addClient(&client);

    // JOIN通知をそのチャンネルの全員に送信
    std::string joinMsg = ":" + client.getNickname() + "!" +
                          client.getUsername() + "@localhost JOIN " +
                          channelName + "\r\n";
    for (std::set<Client*>::iterator it = channel->getClients().begin();
         it != channel->getClients().end(); ++it) {
      (*it)->sendMessage(joinMsg);
    }

    // 参加者一覧（353）、終了（366）
    std::string names;
    for (std::set<Client*>::iterator it = channel->getClients().begin();
         it != channel->getClients().end(); ++it) {
      names += (*it)->getNickname() + " ";
    }
    client.sendMessage(":server 353 " + client.getNickname() + " = " +
                       channelName + " :" + names + "\r\n");
    client.sendMessage(":server 366 " + client.getNickname() + " " +
                       channelName + " :End of /NAMES list\r\n");
  }
  // すでに参加している場合は何もしない
  else {
    client.sendMessage(":server 443 " + client.getNickname() + " " +
                       channelName + " :You are already on that channel\r\n");
  }
}
