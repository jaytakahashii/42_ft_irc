#include "commands/KickCommand.hpp"

#include <unistd.h>

#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "numericsReplies/400-499.hpp"

void KickCommand::execute(const commandS& cmd, Client& client, Server& server) {
  std::string nick = client.getNickname().empty() ? "*" : client.getNickname();
  if (!client.isRegistered()) {
    std::string msg = irc::numericReplies::ERR_NOTREGISTERED(nick);
    client.sendMessage(msg);
    return;
  }

  if (cmd.args.size() < 2) {
    std::string msg = irc::numericReplies::ERR_NEEDMOREPARAMS(nick, "KICK");
    client.sendMessage(msg);
    return;
  }

  std::string channelName = cmd.args[0];
  std::string targetNickname = cmd.args[1];

  // チャンネルが存在しない場合はエラー
  if (server.channels.find(channelName) == server.channels.end()) {
    std::string msg = irc::numericReplies::ERR_NOSUCHCHANNEL(nick, channelName);
    client.sendMessage(msg);
    return;
  }

  Channel* channel = server.channels[channelName];

  // チャンネルに参加していない場合はエラー
  if (!channel->hasClient(&client)) {
    std::string msg = irc::numericReplies::ERR_NOTONCHANNEL(nick, channelName);
    client.sendMessage(msg);
    return;
  }

  // ターゲットがチャンネルに参加していない場合はエラー
  Client* targetClient = channel->getClient(targetNickname);
  if (targetClient == NULL) {
    std::string msg = irc::numericReplies::ERR_USERNOTINCHANNEL(
        nick, targetNickname, channelName);
    client.sendMessage(msg);
    return;
  }

  // チャンネルのオペレーターでない場合はエラー
  if (!channel->isOperator(client.getNickname())) {
    std::string msg =
        irc::numericReplies::ERR_CHANOPRIVSNEEDED(nick, channelName);
    client.sendMessage(msg);
    return;
  }

  // キック理由（オプション）を取得
  std::string kickReason = "";
  if (cmd.args.size() >= 3) {
    kickReason = cmd.args[2];
  } else {
    kickReason = targetNickname;  // デフォルトではターゲットのニックネーム
  }

  // ターゲットをチャンネルから削除
  channel->removeClient(targetClient);

  // RFC2812に準拠したKICKメッセージフォーマット
  // :<prefix> KICK <channel> <user> [<comment>]
  std::string kickMsg = ":" + client.getNickname() + "!" +
                        client.getUsername() + "@localhost KICK " +
                        channelName + " " + targetNickname + " :" + kickReason +
                        "\r\n";

  // チャンネルの全員に通知
  channel->sendToAll(kickMsg);

  // キックされたユーザーにも同じメッセージを送信
  // （チャンネルから既に削除されているため）
  targetClient->sendMessage(kickMsg);
}
