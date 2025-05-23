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
  std::istringstream tokenStream(channelsStr);
  
  // カンマで区切られたチャンネル名を分割（より堅牢な方法）
  while (std::getline(tokenStream, token, ',')) {
    if (!token.empty()) {
      channels.push_back(token);
    }
  }
  
  // チャンネル名が指定されていない場合はエラー
  if (channels.empty()) {
    std::string msg = 
        irc::numericReplies::ERR_NEEDMOREPARAMS(client.getNickname(), cmd.name);
    client.sendMessage(msg);
    return;
  }

  // 退出理由（オプション）
  std::string reason = "";
  if (cmd.args.size() > 1) {
    reason = " :" + cmd.args[1];
  }

  // 処理する前に対象チャンネルのリストをコピー（ループ中に変更が入るのを防ぐため）
  std::vector<std::string> channelsToProcess = channels;
  
  // 指定された各チャンネルから退出
  for (size_t i = 0; i < channelsToProcess.size(); ++i) {
    std::string channelName = channelsToProcess[i];

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
    
    // クライアントをチャンネルから削除
    channel->removeClient(&client);

    // チャンネルにクライアントがいなくなった場合は削除
    if (channel->getClientCount() == 0) {
      // 先にマップからチャンネルを削除し、その後メモリを解放
      server.channels.erase(channelName);
      delete channel;
      channel = NULL; // 無効なポインタ参照を防ぐ
    }
  }
}
