#include "commands/PartCommand.hpp"

#include "Channel.hpp"
#include "Server.hpp"
#include "numericsReplies/400-499.hpp"

void PartCommand::execute(const commandS& cmd, Client& client, Server& server) {
  if (!client.isRegistered()) {
    std::string msg = irc::numericReplies::ERR_NOTREGISTERED();
    client.sendMessage(msg);
    return;
  }

  if (cmd.args.empty()) {
    client.sendMessage("461 PART :Not enough parameters\r\n");
    return;
  }

  std::string channelName = cmd.args[0];

  // チャンネルが存在しない場合はエラー
  if (server.channels.find(channelName) == server.channels.end()) {
    client.sendMessage(":server 403 " + client.getNickname() + " " +
                       channelName + " :No such channel\r\n");
    return;
  }

  Channel* channel = server.channels[channelName];  // チャンネルを取得

  // チャンネルに参加していない場合はエラー
  if (!channel->hasClient(&client)) {
    client.sendMessage(":server 442 " + client.getNickname() + " " +
                       channelName + " :You're not on that channel\r\n");
    return;
  }
}
