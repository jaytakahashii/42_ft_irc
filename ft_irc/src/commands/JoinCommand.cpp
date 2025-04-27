#include "commands/JoinCommand.hpp"

#include "Channel.hpp"
#include "Server.hpp"
#include "numericsReplies/300-399.hpp"
#include "numericsReplies/400-499.hpp"

static const std::vector<std::string> parsers(std::string params) {
  // ','で分割
  std::vector<std::string> result;
  std::string token;
  size_t pos = 0;
  while ((pos = params.find(',')) != std::string::npos) {
    token = params.substr(0, pos);
    result.push_back(token);
    params.erase(0, pos + 1);
  }
  result.push_back(params);
  return result;
}

/**
 * * @brief JoinCommandクラスの実装
 * * @numericsReplies
 * * * ERR_NEEDMOREPARAMS
 * * * ERR_BANNEDFROMCHAN
 * * * ERR_INVITEONLYCHAN
 * * * ERR_BADCHANNELKEY
 * * * ERR_CHANNELISFULL
 * * * ERR_BADCHANMASK
 * * * ERR_NOSUCHCHANNEL
 * * * ERR_TOOMANYCHANNELS
 * * * ERR_TOOMANYTARGETS
 * * * ERR_UNAVAILRESOURCE
 * * * RPL_TOPIC
 */
void JoinCommand::execute(const commandS& cmd, Client& client, Server& server) {
  if (!client.isRegistered()) {
    std::string msg = irc::numericReplies::ERR_NOTREGISTERED();
    client.sendMessage(msg);
    return;
  }

  if (cmd.args.size() < 1) {
    std::string msg = irc::numericReplies::ERR_NEEDMOREPARAMS(cmd.name);
    client.sendMessage(msg);
    return;
  }

  // チャンネル名のバリデーション
  std::vector<std::string> channels = parsers(cmd.args[0]);
  for (size_t i = 0; i < channels.size(); ++i) {
    if (!server.isValidChannelName(channels[i])) {
      std::string msg = irc::numericReplies::ERR_BADCHANMASK(channels[i]);
      client.sendMessage(msg);
      return;
    }
  }
  // key validation
  std::vector<std::string> keys = std::vector<std::string>();
  if (cmd.args.size() == 2) {
    keys = parsers(cmd.args[1]);
    for (size_t i = 0; i < keys.size(); ++i) {
      if (!server.isValidChannelKey(keys[i])) {
        std::string msg = irc::numericReplies::ERR_BADCHANNELKEY(channels[i]);
        client.sendMessage(msg);
        return;
      }
    }
  }

  for (size_t i = 0; i < channels.size(); ++i) {
    // チャンネルが存在しない場合は新規作成
    if (!server.hasChannel(channels[i])) {
      server.channels[channels[i]] = new Channel(channels[i]);
      std::string joinMsg = ":" + client.getNickname() + "!" +
                            client.getUsername() + "@" + client.getHostname() +
                            " JOIN " + channels[i] + "\r\n";
      client.sendMessage(joinMsg);
    } else {
      // チャンネルに参加する
      Channel* channel = server.channels[channels[i]];  // チャンネルを取得
      if (channel->getClientCount() >= 50) {            // TODO
        std::string msg = irc::numericReplies::ERR_CHANNELISFULL(channels[i]);
        client.sendMessage(msg);
        return;
      }
      if (channel->isInviteOnly()) {
        std::string msg = irc::numericReplies::ERR_INVITEONLYCHAN(channels[i]);
        client.sendMessage(msg);
        return;
      }
      if (channel->hasClient(&client)) {
        std::string msg = irc::numericReplies::ERR_BANNEDFROMCHAN(channels[i]);
        client.sendMessage(msg);
        return;
      }
      // チャンネルに参加する
      if (keys.size() > i) {
        if (keys[i] != channel->getKey()) {
          std::string msg = irc::numericReplies::ERR_BADCHANNELKEY(channels[i]);
          client.sendMessage(msg);
          return;
        }
      } else if (channel->getKey() != "") {
        std::string msg = irc::numericReplies::ERR_BADCHANNELKEY(channels[i]);
        client.sendMessage(msg);
        return;
      }

      channel->addClient(&client);
      std::string joinMsg = ":" + client.getNickname() + "!" +
                            client.getUsername() + "@" + client.getHostname() +
                            " JOIN " + channels[i] + "\r\n";
      channel->sendToAll(joinMsg);
      std::string topicMsg =
          irc::numericReplies::RPL_TOPIC(channels[i], channel->getTopic());
      client.sendMessage(topicMsg);
    }
  }
}
