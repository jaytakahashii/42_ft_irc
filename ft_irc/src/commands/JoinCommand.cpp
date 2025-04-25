#include "commands/JoinCommand.hpp"

#include "Channel.hpp"
#include "Server.hpp"
#include "numericsReplies/400-499.hpp"

void JoinCommand::execute(const commandS& cmd, Client& client, Server& server) {
  if (!client.isRegistered()) {
    std::string msg = irc::numericReplies::ERR_NOTREGISTERED();
    client.sendMessage(msg);
    return;
  }

  if (cmd.args.empty()) {
    client.sendMessage("461 JOIN :Not enough parameters\r\n");
    return;
  }

  std::string channelName = cmd.args[0];
  // チャンネル名の先頭が '#' でない場合はエラー
  if (channelName.empty() || channelName[0] != '#') {
    std::string msg = irc::numericReplies::ERR_NOSUCHCHANNEL(channelName);
    client.sendMessage(msg);
    return;
  }

  // チャンネルが存在しない場合は新規作成
  if (server.channels.find(channelName) == server.channels.end()) {
    server.channels[channelName] = new Channel(channelName);
    server.channels[channelName]->addOperator(client.getNickname());
  }

  // チャンネルに参加する
  Channel* channel = server.channels[channelName];  // チャンネルを取得

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
