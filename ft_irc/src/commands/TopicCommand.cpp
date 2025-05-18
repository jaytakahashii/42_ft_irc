#include "commands/TopicCommand.hpp"

#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "numericsReplies/400-499.hpp"

void TopicCommand::execute(const commandS& cmd, Client& client,
                           Server& server) {
  const std::string& nick =
      client.getNickname().empty() ? "*" : client.getNickname();
  if (!client.isRegistered()) {
    std::string msg = irc::numericReplies::ERR_NOTREGISTERED(nick);
    client.sendMessage(msg);
    return;
  }

  if (cmd.args.size() != 1 && cmd.args.size() != 2) {
    std::string msg = irc::numericReplies::ERR_NEEDMOREPARAMS(client.getNickname(), cmd.name);
    client.sendMessage(msg);
    return;
  }

  std::string channelName = cmd.args[0];

  // チャンネルが存在しない場合はエラー
  if (server.channels.find(channelName) == server.channels.end()) {
    std::string msg = irc::numericReplies::ERR_NOSUCHCHANNEL(
        client.getNickname(), channelName);
    client.sendMessage(msg);
    return;
  }
  Channel* channel = server.channels[channelName];
  // チャンネルに参加していない場合はエラー
  if (!channel->hasClient(&client)) {
    std::string msg = irc::numericReplies::ERR_NOTONCHANNEL(
        client.getNickname(), channelName);
    client.sendMessage(msg);
    return;
  }

  if (cmd.args.size() == 1) {
    // トピックを取得
    std::string topic = channel->getTopic();
    if (topic.empty()) {
      // RPL_NOTOPICのレスポンス送信
      std::string msg = ":" + server.getServerName() + " 331 " + client.getNickname() + " " +
                         channelName + " :No topic is set\r\n";
      client.sendMessage(msg);
    } else {
      // RPL_TOPICのレスポンス送信
      std::string msg = ":" + server.getServerName() + " 332 " + client.getNickname() + " " +
                         channelName + " :" + topic + "\r\n";
      client.sendMessage(msg);
    }
  } else {
    // トピックを設定
    // トピック制限モードが有効で、オペレータでない場合はエラー
    if (channel->isTopicRestricted() && !channel->isOperator(client.getNickname())) {
      std::string msg = irc::numericReplies::ERR_CHANOPRIVSNEEDED(
          client.getNickname(), channelName);
      client.sendMessage(msg);
      std::cout << "TOPIC command rejected: Channel " << channelName 
                << " has topic restriction (+t) and user is not an operator" << std::endl;
      return;
    }
    
    std::string newTopic = cmd.args[1];
    channel->setTopic(newTopic);
    std::string msg = ":" + client.getNickname() + "!" + 
                      client.getUsername() + "@" + 
                      client.getHostname() + " TOPIC " + channelName +
                      " :" + newTopic + "\r\n";
    channel->sendToAll(msg);
  }
}
