#include "commands/InviteCommand.hpp"

#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "numericsReplies/300-399.hpp"
#include "numericsReplies/400-499.hpp"

/**
 * * @brief INVITEコマンドの実装
 * * @numericsReplies
 * * * ERR_NEEDMOREPARAMS
 * * * ERR_NOSUCHNICK
 * * * ERR_NOTONCHANNEL
 * * * ERR_USERONCHANNEL
 * * * ERR_CHANOPRIVSNEEDED
 * * * RPL_INVITING
 */
void InviteCommand::execute(const commandS& cmd, Client& client, Server& server) {
  const std::string& nick =
      client.getNickname().empty() ? "*" : client.getNickname();
  if (!client.isRegistered()) {
    std::string msg = irc::numericReplies::ERR_NOTREGISTERED(nick);
    client.sendMessage(msg);
    return;
  }

  // INVITEコマンドには2つの引数が必要: <nickname> <channel>
  if (cmd.args.size() < 2) {
    std::string msg = irc::numericReplies::ERR_NEEDMOREPARAMS(client.getNickname(), cmd.name);
    client.sendMessage(msg);
    return;
  }

  std::string targetNick = cmd.args[0];
  std::string channelName = cmd.args[1];
  
  // 対象ユーザーがサーバーに存在するか確認
  Client* targetClient = server.getClientByNickname(targetNick);
  if (!targetClient) {
    std::string msg = irc::numericReplies::ERR_NOSUCHNICK(
        client.getNickname(), targetNick);
    client.sendMessage(msg);
    return;
  }
  
  // チャンネルが存在するか確認
  if (server.channels.find(channelName) == server.channels.end()) {
    std::string msg = irc::numericReplies::ERR_NOSUCHCHANNEL(
        client.getNickname(), channelName);
    client.sendMessage(msg);
    return;
  }
  
  Channel* channel = server.channels[channelName];
  
  // 招待者がチャンネルに参加しているか確認
  if (!channel->hasClient(&client)) {
    std::string msg = irc::numericReplies::ERR_NOTONCHANNEL(
        client.getNickname(), channelName);
    client.sendMessage(msg);
    return;
  }
  
  // 招待先のユーザーがすでにチャンネルに参加しているか確認
  if (channel->hasClient(targetClient)) {
    std::string msg = irc::numericReplies::ERR_USERONCHANNEL(
        client.getNickname(), targetNick, channelName);
    client.sendMessage(msg);
    return;
  }

  // invite-onlyモードの場合、招待者がオペレータかどうか確認
  if (channel->isInviteOnly() && !channel->isOperator(client.getNickname())) {
    std::string msg = irc::numericReplies::ERR_CHANOPRIVSNEEDED(
        client.getNickname(), channel->getName());
    client.sendMessage(msg);
    return;
  }
  
  // チャンネルの招待リストに追加
  channel->invite(targetNick);
  
  // 招待者に招待成功メッセージを送信
  std::string inviteMsg = irc::numericReplies::RPL_INVITING(
      client.getNickname(), targetNick, channelName);
  client.sendMessage(inviteMsg);
  
  // 対象ユーザーに招待メッセージを送信
  std::string inviteNotification = ":" + client.getNickname() + "!" +
                                  client.getUsername() + "@" +
                                  client.getHostname() + " INVITE " +
                                  targetNick + " " + channelName + "\r\n";
  targetClient->sendMessage(inviteNotification);
  
  // デバッグ出力
  std::cout << "INVITE: " << client.getNickname() << " invited " 
            << targetNick << " to " << channelName << std::endl;
}
