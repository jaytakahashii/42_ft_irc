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

  // チャンネル名をパース
  std::string channelsStr = cmd.args[0];
  std::vector<std::string> channels;
  std::string token;
  size_t pos = 0;
  
  // カンマで区切られたチャンネル名を分割
  while ((pos = channelsStr.find(',')) != std::string::npos) {
    token = channelsStr.substr(0, pos);
    channels.push_back(token);
    channelsStr.erase(0, pos + 1);
  }
  channels.push_back(channelsStr);  // 最後のチャンネル名を追加

  // 退出理由（オプション）
  std::string reason = "";
  if (cmd.args.size() > 1) {
    reason = " :" + cmd.args[1];
  }

  // print channnel names
//   std::cout << channels[0] << std::endl;
  // サーバー上の全チャンネルを表示
//   std::cout << "Available channels on server: ";
//   for (std::map<std::string, Channel*>::const_iterator it = server.channels.begin();
//        it != server.channels.end(); ++it) {
//     std::cout << it->first << " ";
//   }

  // 指定された各チャンネルから退出
  for (size_t i = 0; i < channels.size(); ++i) {
    std::string& channelName = channels[i];

    // チャンネルが存在しない場合はエラー
    if (server.channels.find(channelName) == server.channels.end()) {
      std::string msg = irc::numericReplies::ERR_NOSUCHCHANNEL(
          client.getNickname(), channelName);
      client.sendMessage(msg);
      continue;  // 次のチャンネルを処理
    }

    Channel* channel = server.channels[channelName];  // チャンネルを取得

    // チャンネルに参加していない場合はエラー
    if (!channel->hasClient(&client)) {
      std::string msg = irc::numericReplies::ERR_NOTONCHANNEL(
          client.getNickname(), channelName);
      client.sendMessage(msg);
      continue;  // 次のチャンネルを処理
    }

    // チャンネルから退出メッセージを作成
    std::string partMsg = ":" + client.getNickname() + "!" +
                         client.getUsername() + "@" + client.getHostname() +
                         " PART " + channelName + reason + "\r\n";
    
    // 全チャンネルメンバーに通知（自分も含む）
    channel->sendToAll(partMsg);
    
    // IRC クライアントに確実に応答を送るため、直接クライアントにも送信
    client.sendMessage(partMsg);
    
    // std::cout << "PART message sent to client: " << partMsg;
    
    // クライアントをチャンネルから削除
    channel->removeClient(&client);

    // チャンネルにクライアントがいなくなった場合は削除
    if (channel->getClientCount() == 0) {
    //   std::cout << "Channel " << channelName << " is now empty, deleting..." << std::endl;
      delete channel;
      server.channels.erase(channelName);
    }
  }
}
