#include "commands/ModeCommand.hpp"

#include <stdlib.h>

#include "Channel.hpp"
#include "Server.hpp"
#include "numericsReplies/300-399.hpp"
#include "numericsReplies/400-499.hpp"

void userMode(const commandS& cmd, Client& client, Server& server) {
  // TODO
  std::string mode = cmd.args[1];
  (void)mode;

  // TODO unused
  std::string pass = server.getServerPassword();
  (void)pass;

  std::string clientName = client.getNickname();
  (void)clientName;
}

bool isValidSign(char sign) {
  return sign == '+' || sign == '-';
}

bool isValidMode(char mode) {
  return mode == 'i' || mode == 't' || mode == 'k' || mode == 'o' ||
         mode == 'l';
}

void channelMode(const commandS& cmd, Channel& channel, Client& client,
                 Server& server) {
  /**
   * * @mode
   * * i: Set/remove Invite-only channel
   * * t: Set/remove the restrictions of the TOPIC command to channel operators
   * * k: Set/remove the channel key (password)
   * * o: Give/take channel operator privilege
   * * l: Set/remove the user limit to channel
   */
  // TODO
  std::cout << "Channel mode" << std::endl;

  if (!channel.isOperator(client.getNickname())) {
    std::string msg = irc::numericReplies::ERR_CHANOPRIVSNEEDED(
        client.getNickname(), channel.getName());
    client.sendMessage(msg);
    return;
  }

  if (!isValidSign(cmd.args[1][0])) {
    std::string msg = irc::numericReplies::ERR_UNKNOWNMODE(
        client.getNickname(), cmd.args[1][0], channel.getName());
    client.sendMessage(msg);
    return;
  }

  bool sign = (cmd.args[1][0] == '+');
  for (std::string::const_iterator it = cmd.args[1].begin() + 1;
       it != cmd.args[1].end(); ++it) {
    if (!isValidMode(*it)) {
      std::string msg = irc::numericReplies::ERR_UNKNOWNMODE(
          client.getNickname(), *it, channel.getName());
      client.sendMessage(msg);
      return;
    }
  }

  std::cout << "Channel mode: " << cmd.args[1] << std::endl;

  for (std::string::const_iterator it = cmd.args[1].begin() + 1;
       it != cmd.args[1].end(); ++it) {
    if (*it == 'i') {
      channel.setInviteOnly(sign);
    } else if (*it == 't') {
      channel.setTopicRestricted(sign);
    } else if (*it == 'k') {
      if (sign) { // キーを設定する場合
        if (cmd.args.size() < 3 || cmd.args[2].empty()) {
          std::string msg = irc::numericReplies::ERR_NEEDMOREPARAMS(
              client.getNickname(), cmd.name);
          client.sendMessage(msg);
          return;
        }
        
        // すでにキーが設定されている場合は、古いキーを削除してから新しいキーを設定
        if (channel.hasKey()) {
          std::cout << "Replacing existing key for " << channel.getName() << std::endl;
        }
        
        // キーのバリデーション - キーは空白を含まない単一の単語である必要がある
        if (cmd.args[2].find(' ') != std::string::npos || !server.isValidChannelKey(cmd.args[2])) {
          std::string msg = irc::numericReplies::ERR_BADCHANNELKEY(
              client.getNickname(), channel.getName());
          client.sendMessage(msg);
          return;
        }
        
        std::cout << "Setting channel key for " << channel.getName() << " to: " << cmd.args[2] << std::endl;
        channel.setKey(cmd.args[2]);
        
        // キー設定の通知 - モードメッセージをすべてのチャンネルメンバーに送信
        std::string modeMsg = ":" + client.getNickname() + "!" +
                             client.getUsername() + "@" + client.getHostname() +
                             " MODE " + channel.getName() + " +k " + cmd.args[2] + "\r\n";
        channel.sendToAll(modeMsg);
      } else { // キーを削除する場合
        // キーが設定されていない場合はエラー
        if (!channel.hasKey()) {
          // キーが設定されていない場合のカスタムメッセージ
          std::string msg = ":" + server.getServerName() + " 467 " + 
                            client.getNickname() + " " + channel.getName() + 
                            " :Channel key not set\r\n";
          client.sendMessage(msg);
          return;
        }
        
        // キーの削除にはパラメータが必要（RFC準拠）
        if (cmd.args.size() < 3) {
          std::string msg = irc::numericReplies::ERR_NEEDMOREPARAMS(
              client.getNickname(), cmd.name);
          client.sendMessage(msg);
          return;
        }
        
        // オペレータの場合、キーの一致確認はオプション
        // ただし、パラメータとして現在のキーが提供されているかどうかを確認
        std::string currentKey = channel.getKey();
        
        std::cout << "Removing channel key for " << channel.getName() << std::endl;
        channel.setKey("");
        
        // キー削除の通知 - モードメッセージをすべてのチャンネルメンバーに送信
        std::string modeMsg = ":" + client.getNickname() + "!" +
                             client.getUsername() + "@" + client.getHostname() +
                             " MODE " + channel.getName() + " -k " + cmd.args[2] + "\r\n";
        channel.sendToAll(modeMsg);
      }
    } else if (*it == 'o') {
      if (cmd.args.size() < 3) {
        std::string msg = irc::numericReplies::ERR_NEEDMOREPARAMS(
            client.getNickname(), cmd.name);
        client.sendMessage(msg);
        return;
      }
      
      // 対象ユーザーのニックネームを取得
      std::string targetNick = cmd.args[2];
      Client* targetClient = channel.getClient(targetNick);
      
      // 対象ユーザーがチャンネルに存在するか確認
      if (!targetClient) {
        // ERR_NOSUCHNICKは定義されていないため、同様の意味を持つERR_NOSUCHCHANNELを使用
        std::string msg = irc::numericReplies::ERR_NOTONCHANNEL(
            client.getNickname(), channel.getName());
        client.sendMessage(msg);
        return;
      }
      
      if (sign) { // オペレータ権限を付与
        if (!channel.isOperator(targetNick)) {
          channel.addOperator(targetNick);
          
          // オペレータ権限付与の通知
          std::string modeMsg = ":" + client.getNickname() + "!" +
                               client.getUsername() + "@" + client.getHostname() +
                               " MODE " + channel.getName() + " +o " + targetNick + "\r\n";
          channel.sendToAll(modeMsg);
        }
      } else { // オペレータ権限を剥奪
        if (channel.isOperator(targetNick)) {
          // 最後のオペレータを削除しようとしている場合はエラー
          if (channel.getOperators().size() <= 1 && channel.isOperator(targetNick)) {
            std::string msg = ":" + server.getServerName() + " NOTICE " + client.getNickname() + 
                             " :Cannot remove last operator from channel " + channel.getName() + "\r\n";
            client.sendMessage(msg);
            return;
          }
          
          channel.removeOperator(targetNick);
          
          // オペレータ権限剥奪の通知
          std::string modeMsg = ":" + client.getNickname() + "!" +
                               client.getUsername() + "@" + client.getHostname() +
                               " MODE " + channel.getName() + " -o " + targetNick + "\r\n";
          channel.sendToAll(modeMsg);
        }
      }
    } else if (*it == 'l') {
      if (sign) { // ユーザー制限を設定
        if (cmd.args.size() < 3) {
          std::string msg = irc::numericReplies::ERR_NEEDMOREPARAMS(
              client.getNickname(), cmd.name);
          client.sendMessage(msg);
          return;
        }
        
        // 数値のみが有効
        int limit = atoi(cmd.args[2].c_str());
        if (limit <= 0) {
          std::string msg = ":" + server.getServerName() + " NOTICE " + client.getNickname() +
                           " :Invalid user limit value\r\n";
          client.sendMessage(msg);
          return;
        }
        
        channel.setUserLimit(limit, true);
        
        // ユーザー制限設定の通知
        std::string modeMsg = ":" + client.getNickname() + "!" +
                             client.getUsername() + "@" + client.getHostname() +
                             " MODE " + channel.getName() + " +l " + cmd.args[2] + "\r\n";
        channel.sendToAll(modeMsg);
      } else { // ユーザー制限を解除
        channel.setUserLimit(-1, false);
        
        // ユーザー制限解除の通知
        std::string modeMsg = ":" + client.getNickname() + "!" +
                             client.getUsername() + "@" + client.getHostname() +
                             " MODE " + channel.getName() + " -l\r\n";
        channel.sendToAll(modeMsg);
      }
    }
    // 各モード適用後にレスポンスを送信
    // 注: 実際のモード設定値と正しいパラメータを使用
    std::string modeParams = "";
    if (*it == 'k' || *it == 'l' || *it == 'o') {
      if (cmd.args.size() >= 3) {
        modeParams = cmd.args[2];
      }
    }
    
    // 各モードに対する正しいパラメータを構築
    std::string modeString = std::string(1, cmd.args[1][0]) + std::string(1, *it);
    std::string paramString = "";
    
    if (*it == 'k' && cmd.args.size() >= 3) {
        paramString = cmd.args[2];
    } else if (*it == 'o' && cmd.args.size() >= 3) {
        paramString = cmd.args[2];
    } else if (*it == 'l' && sign && cmd.args.size() >= 3) {
        paramString = cmd.args[2];
    }
    
    // チャンネルモード情報のレスポンス送信
    std::string msg = irc::numericReplies::RPL_CHANNELMODEIS(
        client.getNickname(), channel.getName(), modeString, paramString);
    client.sendMessage(msg);
  }

  std::string serverName = server.getServerName();
  (void)serverName;
}

#include <cstdio>

void ModeCommand::execute(const commandS& cmd, Client& client, Server& server) {
  std::string nick = client.getNickname().empty() ? "*" : client.getNickname();
  if (!client.isRegistered()) {
    std::string msg = irc::numericReplies::ERR_NOTREGISTERED(nick);
    client.sendMessage(msg);
    return;
  }

  if (cmd.args.size() < 2) {
    // 引数が1つの場合は現在のモードを表示
    if (cmd.args.size() == 1) {
      std::string channelPrefix = "#+!&";
      if (channelPrefix.find(cmd.args[0][0]) != std::string::npos) {
        std::string channelName = cmd.args[0];
        Channel* channel = server.channels[channelName];
        
        if (!channel) {
          std::string msg = irc::numericReplies::ERR_NOSUCHCHANNEL(
              client.getNickname(), channelName);
          client.sendMessage(msg);
          return;
        }
        
        // チャンネルの現在のモードを表示
        std::string currentModes = "+";
        std::string modeParams = "";
        
        if (channel->isInviteOnly()) currentModes += "i";
        if (channel->isTopicRestricted()) currentModes += "t";
        if (channel->hasKey()) {
          currentModes += "k";
          modeParams += channel->getKey() + " ";
        }
        if (channel->getUserLimit() != (size_t)-1) {
          currentModes += "l";
          char buffer[32];
          sprintf(buffer, "%zu", channel->getUserLimit());
          modeParams += buffer;
        }
        
        std::string msg = irc::numericReplies::RPL_CHANNELMODEIS(
            client.getNickname(), channelName, currentModes, modeParams);
        client.sendMessage(msg);
        return;
      }
    }
    
    std::string msg =
        irc::numericReplies::ERR_NEEDMOREPARAMS(client.getNickname(), cmd.name);
    client.sendMessage(msg);
    return;
  }

  std::string channelPrefix = "#+!&";
  if (channelPrefix.find(cmd.args[0][0]) !=
      std::string::npos) {  // チャンネルモード
    std::string channelName = cmd.args[0];
    Channel* channel = server.channels[channelName];

    if (!channel) {
      std::string msg = irc::numericReplies::ERR_NOSUCHCHANNEL(
          client.getNickname(), channelName);
      client.sendMessage(msg);
      return;
    }

    channelMode(cmd, *channel, client, server);
  } else {  // ユーザーモード
    userMode(cmd, client, server);
  }

  // TODO unused
  std::string pass = server.getServerPassword();
  (void)pass;
}
