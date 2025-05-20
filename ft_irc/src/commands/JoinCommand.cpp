#include "commands/JoinCommand.hpp"

#include "Channel.hpp"
#include "Server.hpp"
#include "numericsReplies/300-399.hpp"
#include "numericsReplies/400-499.hpp"

// 一人が参加できるチャンネルの最大数
#define MAX_CHANNELS_PER_USER 10

// 一度に参加できるチャンネルの最大数
#define MAX_JOIN_TARGETS 5

// チャンネルに入れるメンバーの初期最大数
#define MAX_CHANNEL_MEMBERS 50

static const std::map<std::string, std::string> parsers(const commandS cmd) {
  // ','で分割
  std::map<std::string, std::string> ret;
  std::string channels = cmd.args[0];
  std::string keys = cmd.args.size() == 2 ? cmd.args[1] : "";

  // チャンネルリストとキーリストを作成
  std::vector<std::string> channelsList;
  std::vector<std::string> keysList;

  // 重複チャンネル検出用の一時的なセット
  std::set<std::string> uniqueChannels;
  std::vector<std::string> duplicateChannels;

  // チャンネルをパース
  size_t pos = 0;
  std::string token;
  while ((pos = channels.find(',')) != std::string::npos) {
    token = channels.substr(0, pos);

    // 重複チェック
    if (!token.empty() && uniqueChannels.find(token) == uniqueChannels.end()) {
      uniqueChannels.insert(token);
      channelsList.push_back(token);
    } else if (!token.empty()) {
      duplicateChannels.push_back(token);
    }

    channels.erase(0, pos + 1);
  }

  // 最後のチャンネルも同様に重複チェック
  if (!channels.empty()) {
    if (uniqueChannels.find(channels) == uniqueChannels.end()) {
      uniqueChannels.insert(channels);
      channelsList.push_back(channels);
    } else {
      duplicateChannels.push_back(channels);
    }
  }

  // デバッグ情報を出力
  //   if (!duplicateChannels.empty()) {
  //     std::cout << "Duplicate channels detected: ";
  //     for (size_t i = 0; i < duplicateChannels.size(); ++i) {
  //       std::cout << duplicateChannels[i] << " ";
  //     }
  //     std::cout << std::endl;
  //   }

  // キーをパース
  if (!keys.empty()) {
    pos = 0;
    while ((pos = keys.find(',')) != std::string::npos) {
      token = keys.substr(0, pos);
      keysList.push_back(token);  // 空のキーも保持する
      keys.erase(0, pos + 1);
    }
    // 最後のキー
    keysList.push_back(keys);
  }

  // チャンネルとキーをマッピング
  for (size_t i = 0; i < channelsList.size(); ++i) {
    std::string channelName = channelsList[i];
    std::string keyValue = "";

    // キーがある場合は対応するキーを取得
    if (i < keysList.size()) {
      keyValue = keysList[i];
    }

    ret[channelName] = keyValue;
  }

  // デバッグ出力: チャンネルとキーのマッピング
  //   std::cout << "Channel-Key mapping:" << std::endl;
  //   for (std::map<std::string, std::string>::iterator it = ret.begin(); it !=
  //   ret.end(); ++it) {
  //     std::cout << "  " << it->first << " -> '" << it->second << "'" <<
  //     std::endl;
  //   }

  return ret;
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
 * * * ERR_NOSUCHCHANNEL todo
 * * * ERR_TOOMANYCHANNELS
 * * * ERR_TOOMANYTARGETS
 * * * ERR_UNAVAILRESOURCE
 * * * RPL_TOPIC
 */
void JoinCommand::execute(const commandS& cmd, Client& client, Server& server) {
  if (!client.isRegistered()) {
    std::string msg =
        irc::numericReplies::ERR_NOTREGISTERED(client.getNickname());
    client.sendMessage(msg);
    return;
  }

  if (cmd.args.size() < 1) {
    std::string msg =
        irc::numericReplies::ERR_NEEDMOREPARAMS(client.getNickname(), cmd.name);
    client.sendMessage(msg);
    return;
  }

  // チャンネル名のバリデーション
  std::map<std::string, std::string> channels = parsers(cmd);

  if (channels.size() > MAX_JOIN_TARGETS) {
    // std::cout << "JOIN: Too many target channels specified" << std::endl;
    std::string msg = irc::numericReplies::ERR_TOOMANYTARGETS(
        client.getNickname(), "JOIN", "407",
        "Too many target channels specified");
    client.sendMessage(msg);
    return;
  }

  // JOINしているチャンネルの数をカウント上限を超えている場合は405
  int currentChannelCount = 0;
  for (std::map<std::string, Channel*>::iterator it = server.channels.begin();
       it != server.channels.end(); ++it) {
    if (it->second->hasClient(&client)) {
      currentChannelCount++;
    }
  }

  if (currentChannelCount >= MAX_CHANNELS_PER_USER) {
    std::string msg = irc::numericReplies::ERR_TOOMANYCHANNELS(
        client.getNickname(), channels.begin()->first);
    client.sendMessage(msg);
    return;
  }
  for (std::map<std::string, std::string>::iterator it = channels.begin();
       it != channels.end(); ++it) {

    // Validate channel name
    if (!server.isValidChannelName(it->first)) {
      std::string msg =
          irc::numericReplies::ERR_BADCHANMASK(client.getNickname(), it->first);
      client.sendMessage(msg);
      continue;
    }

    // Key validation
    if (!it->second.empty() && !server.isValidChannelKey(it->second)) {
      std::string msg = irc::numericReplies::ERR_BADCHANNELKEY(
          client.getNickname(), it->first);
      client.sendMessage(msg);
      continue;
    }

    // 使えないチャンネル名について。todo
    if (it->first == "reserved" || it->first == "system" ||
        it->first == "admin") {
      std::string msg = irc::numericReplies::ERR_UNAVAILRESOURCE(
          client.getNickname(), client.getNickname(), it->first);
      client.sendMessage(msg);
      continue;
    }

    // チャンネルが存在しない場合は新規作成
    // map::operatorを使うとキーが存在しない場合は新しい要素が作成されるため、hasChannel関数で確認
    if (!server.hasChannel(it->first)) {
      std::cout << "JOIN: Creating new channel " << it->first << std::endl;
      server.channels[it->first] = new Channel(it->first);
      std::string joinMsg = ":" + client.getNickname() + "!" +
                            client.getUsername() + "@" + client.getHostname() +
                            " JOIN " + it->first + "\r\n";
      server.channels[it->first]->addClient(&client);
      server.channels[it->first]->addOperator(client.getNickname());
      client.sendMessage(joinMsg);
    } else {
      // チャンネルに参加する
      Channel* channel = server.channels[it->first];  // チャンネルを取得
      if (channel->getClientCount() >= MAX_CHANNEL_MEMBERS) {  // Maximum channel capacity check
        std::string msg = irc::numericReplies::ERR_CHANNELISFULL(
            client.getNickname(), it->first);
        client.sendMessage(msg);
        continue;
      }
      // invite-onlyモードが有効な場合、チェック
      if (channel->isInviteOnly()) {
        // オペレータか招待されたユーザーのみ参加可能
        if (!channel->isOperator(client.getNickname()) &&
            !channel->isInvited(client.getNickname())) {
          std::string msg = irc::numericReplies::ERR_INVITEONLYCHAN(
              client.getNickname(), it->first);
          client.sendMessage(msg);
          //   std::cout << "JOIN: User " << client.getNickname() << " rejected
          //   from "
          //             << it->first << " (invite-only)" << std::endl;
          continue;
        } else {
          // 招待されていた場合、デバッグ出力
          //   if (channel->isInvited(client.getNickname())) {
          //     std::cout << "JOIN: User " << client.getNickname() << " joins "
          //               << it->first << " via invitation" << std::endl;
          //   }
        }
      }

      // 招待リストから削除（招待によるJOINの場合）
      if (channel->isInvited(client.getNickname())) {
        channel->removeInvite(client.getNickname());
      }
      // Check if the user is already in the channel
      if (channel->hasClient(&client)) {
        continue;  // User is already in the channel, skip
      }

      // Check if the user is banned
      std::string userMask = client.getNickname() + "!" + client.getUsername() +
                             "@" + client.getHostname();
      if (channel->getIsUserLimit() &&
          channel->getClientCount() >= channel->getUserLimit()) {
        std::string msg = irc::numericReplies::ERR_CHANNELISFULL(
            client.getNickname(), it->first);
        client.sendMessage(msg);
        continue;
      }
      // MODEコマンドで設定されたキーの確認
      if (channel->hasKey()) {
        // クライアントがキーを提供しているか確認、提供されたキーが正しいか確認
        if (it->second.empty() || it->second != channel->getKey()) {
          // キーが要求されるが提供されていない
          std::string msg = irc::numericReplies::ERR_BADCHANNELKEY(
              client.getNickname(), it->first);
          client.sendMessage(msg);
          //   std::cout << "JOIN failed: Channel " << it->first
          //             << " requires a key, but none provided" << std::endl;
          continue;
        }
        // ここまで来たら正しいキーが提供された
        // std::cout << "JOIN: Correct key provided for channel " << it->first
        //           << std::endl;
      }

      // チャンネルの上限を超えている場合は405
      // クライアントを追加する前にもう一回確認
      int currentJoinedChannels = 0;
      for (std::map<std::string, Channel*>::iterator chanIt =
               server.channels.begin();
           chanIt != server.channels.end(); ++chanIt) {
        if (chanIt->second->hasClient(&client)) {
          currentJoinedChannels++;
        }
      }

      if (currentJoinedChannels >= MAX_CHANNELS_PER_USER) {
        std::string msg = irc::numericReplies::ERR_TOOMANYCHANNELS(
            client.getNickname(), it->first);
        client.sendMessage(msg);
        continue;
      }

      channel->addClient(&client);
      std::string joinMsg = ":" + client.getNickname() + "!" +
                            client.getUsername() + "@" + client.getHostname() +
                            " JOIN " + it->first + "\r\n";
      channel->sendToAll(joinMsg);
      std::string topicMsg = irc::numericReplies::RPL_TOPIC(
          client.getNickname(), it->first, channel->getTopic());
      client.sendMessage(topicMsg);
    }
  }
}
