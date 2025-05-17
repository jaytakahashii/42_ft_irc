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

static const bool checkRegistered(Client& client) {
  if (!client.isRegistered()) {
    std::string msg = irc::numericReplies::ERR_NOTREGISTERED(client.getNickname());
    client.sendMessage(msg);
    return false;
  }
  return true;
}

static const bool checkArgmentSize(const commandS& cmd, Client& client) {
  if (cmd.args.size() < 1) {
    std::string msg =
        irc::numericReplies::ERR_NEEDMOREPARAMS(client.getNickname(), cmd.name);
    client.sendMessage(msg);
    return false;
  }
  return true;
}

static const bool checkChannelName(
  const std::string& channel,
  const std::vector<std::string> channels,
  Client& client,
  Server& server
) {
  for (size_t i = 0; i < channels.size(); ++i) {
    if (!server.isValidChannelName(channels[i])) {
      std::string msg = irc::numericReplies::ERR_BADCHANMASK(
          client.getNickname(), channels[i]);
      client.sendMessage(msg);
      return false;
    }
  }
  return true;
}

static const bool checkKey(
  const std::vector<std::string> keys,
  Client& client,
  Server& server
) {
  for (size_t i = 0; i < keys.size(); ++i) {
    if (keys[i].empty()) { continue; }
    if (!server.isValidChannelKey(keys[i])) {
      std::string msg = irc::numericReplies::ERR_BADCHANNELKEY(
          client.getNickname(), keys[i]);
      client.sendMessage(msg);
      return false;
    }
  }
  return true;
}


bool isClientAlreadyInChannel(Channel* channel, Client& client, const std::string& channelName) {
  if (channel->hasClient(&client)) {
    client.sendMessage(irc::numericReplies::ERR_USERONCHANNEL(
        client.getNickname(), client.getNickname(), channelName));
    return true;
  }
  return false;
}

bool isChannelFull(Channel* channel, Client& client, const std::string& channelName) {
  if (channel->getClientCount() >= 50) {
    client.sendMessage(irc::numericReplies::ERR_CHANNELISFULL(client.getNickname(), channelName));
    return true;
  }
  return false;
}

bool isInviteOnlyChannel(Channel* channel, Client& client, const std::string& channelName) {
  if (channel->isInviteOnly()) {
    client.sendMessage(irc::numericReplies::ERR_INVITEONLYCHAN(client.getNickname(), channelName));
    return true;
  }
  return false;
}

/* TODO: hasClient -> isBanned */
bool isClientBannedFromChannel(Channel* channel, Client& client, const std::string& channelName) {
  if (channel->hasClient(&client)) {
    client.sendMessage(irc::numericReplies::ERR_BANNEDFROMCHAN(client.getNickname(), channelName));
    return true;
  }
  return false;
}

bool checkChannelKey(Channel* channel, const std::string& key, 
  Client& client, const std::string& channelName) {
  if (!key.empty()) {
    if (key != channel->getKey()) {
      client.sendMessage(irc::numericReplies::ERR_BADCHANNELKEY(client.getNickname(), channelName));
      return false;
    }
  } else if (!channel->getKey().empty()) {
    client.sendMessage(irc::numericReplies::ERR_BADCHANNELKEY(client.getNickname(), channelName));
    return false;
  }
  return true;
}


// JOIN <channel>{,<channel>} [<key>{,<key>}]
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
  if (!checkRegistered(client)) { return; }
  if (!checkArgmentSize(cmd, client)) { return; }

  std::vector<std::string> channels = parsers(cmd.args[0]);
  std::vector<std::string> keys = (cmd.args.size() > 1)
                                ? parsers(cmd.args[1])
                                : std::vector<std::string>(channels.size(), "");

  if (!checkChannelName(cmd.args[0], channels, client, server)) { return; }
  if (!checkKey(keys, client, server)) { return; }

  for (size_t i = 0; i < channels.size(); ++i) {
    // チャンネルが存在しない場合は新規作成
    if (!server.hasChannel(channels[i])) {
      server.channels[channels[i]] = new Channel(channels[i]);
      // 参加
      server.channels[channels[i]]->setKey(keys[i]);
      server.channels[channels[i]]->addClient(&client);

      std::string joinMsg = ":" + client.getNickname() + "!" +
                            client.getUsername() + "@" + client.getHostname() +
                            " JOIN " + channels[i] + "\r\n";
      client.sendMessage(joinMsg);
    } else {
      // チャンネルに参加する
      Channel* channel = server.channels[channels[i]];  // チャンネルを取得

      if (isClientAlreadyInChannel(channel, client, channels[i])) { return; }
      if (isChannelFull(channel, client, channels[i])) { return; }
      if (isInviteOnlyChannel(channel, client, channels[i])) { return; }
      if (isClientBannedFromChannel(channel, client, channels[i])) { return; }
      if (!checkChannelKey(channel, keys[i], client, channels[i])) { return; }

      channel->addClient(&client);
      std::string joinMsg = ":" + client.getNickname() + "!" +
                            client.getUsername() + "@" + client.getHostname() +
                            " JOIN " + channels[i] + "\r\n";
      channel->sendToAll(joinMsg);
      std::string topicMsg = irc::numericReplies::RPL_TOPIC(
          client.getNickname(), channels[i], channel->getTopic());
      client.sendMessage(topicMsg);
    }
  }
}
