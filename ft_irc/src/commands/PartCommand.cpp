#include "commands/PartCommand.hpp"

#include "Channel.hpp"
#include "Server.hpp"
#include "numericsReplies/400-499.hpp"

void PartCommand::execute(const commandS& cmd, Client& client, Server& server) {
  const std::string& nick =
      client.getNickname().empty() ? "*" : client.getNickname();
  if (!client.isRegistered()) {
    std::string msg = irc::numericReplies::ERR_NOTREGISTERED(nick);
    client.sendMessage(msg);
    return;
  }

  if (cmd.args.empty()) {
    std::string msg =
        irc::numericReplies::ERR_NEEDMOREPARAMS(client.getNickname(), cmd.name);
    client.sendMessage(msg);
    return;
  }

  std::string channelName = cmd.args[0];
  // チャンネルが存在しない場合はエラー
  if (server.channels.find(channelName) == server.channels.end()) {
    const std::string msg = irc::numericReplies::ERR_NOSUCHCHANNEL(
        client.getNickname(), channelName);
    return;
  }

  Channel* channel = server.channels[channelName];  // チャンネルを取得

  // チャンネルに参加していない場合はエラー
  if (!channel->hasClient(&client)) {
    const std::string msg = irc::numericReplies::ERR_NOTONCHANNEL(
        client.getNickname(), channelName);
    return;
  }
}
