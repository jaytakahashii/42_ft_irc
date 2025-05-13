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

	size_t pos = 0;
	std::string token;
	
	// Parse channels
	while ((pos = channels.find(',')) != std::string::npos) {
		token = channels.substr(0, pos);
		ret[token] = ""; // Initially set empty key
		channels.erase(0, pos + 1);
	}
	if (!channels.empty()) {
		ret[channels] = ""; // Add the last channel
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
			
			// std::cout << "keyPos: " << keyPos << " ";
			// Extract key token
			if (keyPos != std::string::npos) {
				// Get the key segment and assign it to the current channel
				it->second = keys.substr(0, keyPos);
				// Remove the key and the comma from the keys string
				keys.erase(0, keyPos + 1);
			}
			else if (keyPos == 0)
			{
				it->second = "";
				keys.erase(0, keyPos + 1);
			}
			else {
				// This is the last key
				it->second = keys;
				keys = "";
			}

			// std::cout << "key: " << it->second << std::endl;
			
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
      // チャンネルに参加する
      if (!it->second.empty()) {
        if (it->second != channel->getKey()) {
          std::string msg = irc::numericReplies::ERR_BADCHANNELKEY(
              client.getNickname(), it->first);
          client.sendMessage(msg);
          return;
        }
      } else if (channel->getKey() != "") {
        std::string msg = irc::numericReplies::ERR_BADCHANNELKEY(
            client.getNickname(), it->first);
        client.sendMessage(msg);
        return;
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
