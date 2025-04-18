#include "commands/JoinCommand.hpp"

#include <sys/socket.h>

#include "Channel.hpp"

void JoinCommand::execute(const SCommand& cmd, Client& client,
                          ServerState& state) {
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
  if (state.channels.find(channelName) == state.channels.end()) {
    state.channels[channelName] = new Channel(channelName);
    state.channels[channelName]->addOperator(client.getNickname());
  }

  // チャンネルに参加する
  Channel* channel = state.channels[channelName];  // チャンネルを取得

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
