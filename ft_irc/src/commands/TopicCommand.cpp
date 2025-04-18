#include "commands/TopicCommand.hpp"

#include "Channel.hpp"
#include "Client.hpp"
#include "numericsReplies/400-499.hpp"

void TopicCommand::execute(const commandS& cmd, Client& client,
                           serverStateS& state) {
  if (!client.isRegistered()) {
    std::string msg = irc::numericReplies::ERR_NOTREGISTERED();
    client.sendMessage(msg);
    return;
  }

  if (cmd.args.size() != 1 && cmd.args.size() != 2) {
    client.sendMessage("461 TOPIC :Not enough parameters\r\n");
    return;
  }

  std::string channelName = cmd.args[0];

  // チャンネルが存在しない場合はエラー
  if (state.channels.find(channelName) == state.channels.end()) {
    client.sendMessage(":server 403 " + client.getNickname() + " " +
                       channelName + " :No such channel\r\n");
    return;
  }
  Channel* channel = state.channels[channelName];
  // チャンネルに参加していない場合はエラー
  if (!channel->hasClient(&client)) {
    client.sendMessage(":server 442 " + client.getNickname() + " " +
                       channelName + " :You're not on that channel\r\n");
    return;
  }

  if (cmd.args.size() == 1) {
    // トピックを取得
    std::string topic = channel->getTopic();
    if (topic.empty()) {
      client.sendMessage(":server 331 " + client.getNickname() + " " +
                         channelName + " :No topic is set\r\n");
    } else {
      client.sendMessage(":server 332 " + client.getNickname() + " " +
                         channelName + " :" + topic + "\r\n");
    }
  } else {
    // トピックを設定
    if (!channel->isOperator(client.getNickname())) {
      client.sendMessage(":server 482 " + client.getNickname() + " " +
                         channelName + " :You're not channel operator\r\n");
      return;
    }
    std::string newTopic = cmd.args[1];
    channel->setTopic(newTopic);
    std::string msg = ":" + client.getNickname() + " TOPIC " + channelName +
                      " :" + newTopic + "\r\n";
    channel->sendToAll(msg);
  }
}
