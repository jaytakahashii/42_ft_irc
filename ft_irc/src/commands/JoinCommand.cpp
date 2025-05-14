#include "commands/JoinCommand.hpp"

#include "Channel.hpp"
#include "Server.hpp"
#include "numericsReplies/300-399.hpp"
#include "numericsReplies/400-499.hpp"

static const std::map<std::string, std::string> parsers(const commandS cmd) {
	// ','で分割
	std::map<std::string, std::string> ret;
	std::string channels = cmd.args[0];
	std::string keys = cmd.args.size() == 2 ? cmd.args[1] : "";

	// 重複チャンネル検出用の一時的なセット
	std::set<std::string> uniqueChannels;
	
	// チャンネルの処理とメッセージ出力のためのベクトル
	std::vector<std::string> channelsList;
	std::vector<std::string> duplicateChannels;

	size_t pos = 0;
	std::string token;
	
	// Parse channels
	while ((pos = channels.find(',')) != std::string::npos) {
		token = channels.substr(0, pos);
		
		// 重複チェック
		if (uniqueChannels.find(token) == uniqueChannels.end()) {
			uniqueChannels.insert(token);
			ret[token] = ""; // Initially set empty key
			channelsList.push_back(token);
		} else {
			// 重複が発見された場合は、前回の指定を上書き（mapが自動的に対応）
			duplicateChannels.push_back(token);
		}
		
		channels.erase(0, pos + 1);
	}
	
	// 最後のチャンネルも同様に重複チェック
	if (!channels.empty()) {
		if (uniqueChannels.find(channels) == uniqueChannels.end()) {
			uniqueChannels.insert(channels);
			ret[channels] = ""; // Add the last channel
			channelsList.push_back(channels);
		} else {
			duplicateChannels.push_back(channels);
		}
	}
	
	// デバッグ情報を出力
	if (!duplicateChannels.empty()) {
		for (size_t i = 0; i < duplicateChannels.size(); ++i) {
			std::cout << duplicateChannels[i] << " ";
		}
		std::cout << std::endl;
	}
	
	// Parse keys if they exist
	if (!keys.empty()) {
		size_t keyPos = 0;
		std::map<std::string, std::string>::iterator it = ret.begin();
		
		// Handle keys parsing with proper empty key support
		while (it != ret.end()) {
			// Check if we've reached the end of the keys string
			if (keys.empty()) {
				break;
			}
			
			// Find next comma
			keyPos = keys.find(',');
			
			// Extract key token
			if (keyPos != std::string::npos) {
				// Get the key segment and assign it to the current channel
				it->second = keys.substr(0, keyPos);
				// Remove the key and the comma from the keys string
				keys.erase(0, keyPos + 1);
			}
			else {
				// This is the last key
				it->second = keys;
				keys = "";
			}

			++it;
		}
	}
	
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
 * * * ERR_NOSUCHCHANNEL
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
	

	// std::cout << cmd.args[1] << std::endl;
	// /join test1,test2,test3 key,,keyとすると、args[1]にkey,x,keyが入ってる todo
	
  // チャンネル名のバリデーション
  std::map<std::string, std::string> channels = parsers(cmd);

  // チャンネルとキーの詳細をデバッグ表示
//   std::cout << "Channels to join: ";
//   for (std::map<std::string, std::string>::iterator it = channels.begin(); it != channels.end(); ++it) {
//     std::cout << it->first << " (key: '" << it->second << "') ";
//   }
//   std::cout << std::endl;
  
  for (std::map<std::string, std::string>::iterator it = channels.begin(); it != channels.end(); ++it) {
    if (!server.isValidChannelName(it->first)) {
      std::string msg = irc::numericReplies::ERR_BADCHANMASK(
          client.getNickname(), it->first);
      client.sendMessage(msg);
      return;
    }
    
    // Key validation
    if (!it->second.empty() && !server.isValidChannelKey(it->second)) {
      std::string msg = irc::numericReplies::ERR_BADCHANNELKEY(
          client.getNickname(), it->first);
      client.sendMessage(msg);
      return;
    }
  }

  for (std::map<std::string, std::string>::iterator it = channels.begin(); it != channels.end(); ++it) {
    // チャンネルが存在しない場合は新規作成
    if (!server.hasChannel(it->first)) {
      server.channels[it->first] = new Channel(it->first);
      std::string joinMsg = ":" + client.getNickname() + "!" +
                            client.getUsername() + "@" + client.getHostname() +
                            " JOIN " + it->first + "\r\n";
      client.sendMessage(joinMsg);
    } else {
      // チャンネルに参加する
      Channel* channel = server.channels[it->first];  // チャンネルを取得
      if (channel->getClientCount() >= 50) {            // TODO
        std::string msg = irc::numericReplies::ERR_CHANNELISFULL(
            client.getNickname(), it->first);
        client.sendMessage(msg);
        return;
      }
      if (channel->isInviteOnly()) {
        std::string msg = irc::numericReplies::ERR_INVITEONLYCHAN(
            client.getNickname(), it->first);
        client.sendMessage(msg);
        return;
      }
      if (channel->hasClient(&client)) {
        std::string msg = irc::numericReplies::ERR_BANNEDFROMCHAN(
            client.getNickname(), it->first);
        client.sendMessage(msg);
        return;
      }
      // MODEコマンドで設定されたキーの確認
      if (channel->hasKey()) {
        // クライアントがキーを提供しているか確認
        if (it->second.empty()) {
          // キーが要求されるが提供されていない
          std::string msg = irc::numericReplies::ERR_BADCHANNELKEY(
              client.getNickname(), it->first);
          client.sendMessage(msg);
          std::cout << "JOIN failed: Channel " << it->first << " requires a key, but none provided" << std::endl;
          return;
        } 
        // 提供されたキーが正しいか確認
        if (it->second != channel->getKey()) {
          std::string msg = irc::numericReplies::ERR_BADCHANNELKEY(
              client.getNickname(), it->first);
          client.sendMessage(msg);
          std::cout << "JOIN failed: Incorrect key for channel " << it->first << std::endl;
          return;
        }
        // ここまで来たら正しいキーが提供された
        std::cout << "JOIN: Correct key provided for channel " << it->first << std::endl;
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
