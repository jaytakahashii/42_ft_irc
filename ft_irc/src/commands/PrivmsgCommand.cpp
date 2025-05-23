#include "commands/PrivmsgCommand.hpp"

#include <sstream>
#include <vector>

#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "numericsReplies/400-499.hpp"

/**
 * @brief カンマで区切られた文字列を分割するヘルパー関数
 * @param str 分割する文字列
 * @return 分割された文字列のベクター
 *
 * IRCプロトコルでは、PRIVMSGコマンドの送信先はカンマ区切りで複数指定できる
 * 例: PRIVMSG user1,user2,#channel :メッセージ
 */
std::vector<std::string> splitByComma(const std::string& str) {
  std::vector<std::string> result;
  std::string token;
  std::istringstream tokenStream(str);

  while (std::getline(tokenStream, token, ',')) {
    if (!token.empty()) {
      result.push_back(token);
    }
  }

  return result;
}

/**
 * @brief PRIVMSGコマンドの実行
 *
 * RFC2812に準拠したPRIVMSGコマンドを実装。
 * 以下の機能を持つ：
 * - 複数のターゲット（カンマ区切り）へのメッセージ送信
 * - チャンネルメンバーシップの確認
 * - 適切なエラーレスポンス
 */
void PrivmsgCommand::execute(const commandS& cmd, Client& client,
                             Server& server) {
  const std::string& nick =
      client.getNickname().empty() ? "*" : client.getNickname();
  if (!client.isRegistered()) {
    std::string msg = irc::numericReplies::ERR_NOTREGISTERED(nick);
    client.sendMessage(msg);
    return;
  }

  // 引数の数チェック
  if (cmd.args.size() < 1) {
    std::string msg =
        irc::numericReplies::ERR_NORECIPIENT(client.getNickname(), cmd.name);
    client.sendMessage(msg);
    return;
  }

  if (cmd.args.size() < 2 || cmd.args[1].empty()) {
    std::string msg =
        irc::numericReplies::ERR_NOTEXTTOSEND(client.getNickname());
    client.sendMessage(msg);
    return;
  }

  std::string targets = cmd.args[0];
  std::string message = cmd.args[1];

  // カンマで区切られたターゲットリストを処理
  std::vector<std::string> targetList = splitByComma(targets);

  if (targetList.empty()) {
    std::string msg =
        irc::numericReplies::ERR_NORECIPIENT(client.getNickname(), cmd.name);
    client.sendMessage(msg);
    return;
  }

  // 各ターゲットに対してメッセージを送信
  for (size_t i = 0; i < targetList.size(); ++i) {
    const std::string& target = targetList[i];

    // ターゲットが空の場合はスキップ
    if (target.empty()) {
      continue;
    }

    // チャンネルにメッセージを送信
    if (target[0] == '#') {
      Channel* channel = server.channels[target];
      if (channel) {
        // クライアントがそのチャンネルのメンバーであることを確認
        if (!channel->hasClient(&client)) {
          std::string msg = irc::numericReplies::ERR_CANNOTSENDTOCHAN(
              client.getNickname(), target);
          client.sendMessage(msg);
          continue;  // 次のターゲットへ
        }

        // チャンネルの全員（送信者を除く）にメッセージを送信
        std::string privmsg = ":" + client.getNickname() + " PRIVMSG " +
                              target + " :" + message + "\r\n";
        channel->sendToAllExcept(&client, privmsg);
      } else {
        std::string msg = irc::numericReplies::ERR_NOSUCHCHANNEL(
            client.getNickname(), target);
        client.sendMessage(msg);
      }
    }
    // ユーザーにメッセージを送信
    else {
      // より効率的な方法でクライアントを検索（getClientByNicknameを利用）
      Client* recipient = server.getClientByNickname(target);

      // 受信者が見つかった場合
      if (recipient) {
        // 自分自身にメッセージを送ることはスキップ
        if (recipient == &client) {
          continue;
        }
        std::string privmsg = ":" + client.getNickname() + " PRIVMSG " +
                              target + " :" + message + "\r\n";
        recipient->sendMessage(privmsg);
      } else {  // 受信者が見つからない場合
        std::string msg =
            irc::numericReplies::ERR_NOSUCHNICK(client.getNickname(), target);
        client.sendMessage(msg);
      }
    }
  }
}
