#include "commands/KickCommand.hpp"

#include <unistd.h>

#include "Channel.hpp"
#include "Client.hpp"

void KickCommand::execute(const commandS& cmd, Client& client,
                          serverStateS& state) {
  if (cmd.args.size() < 2) {
    client.sendMessage("461 KICK :Not enough parameters\r\n");
    return;
  }

  std::string channelName = cmd.args[0];
  std::string targetNickname = cmd.args[1];

  // チャンネルが存在しない場合はエラー
  if (state.channels.find(channelName) == state.channels.end()) {
    client.sendMessage(":server 403 " + client.getNickname() + " " +
                       channelName + " :No such channel\r\n");
    return;
  }

  Channel* channel = state.channels[channelName];

  // チャンネルに参加していない場合はエラー
  if (!channel->hasClient(&client)) {
    client.sendMessage(":server 442 " + client.getNickname() + " " +
                       channelName + " :You're not on that channel\r\n");
    return;
  }

  // ターゲットがチャンネルに参加していない場合はエラー
  Client* targetClient = channel->getClient(targetNickname);
  if (targetClient == NULL) {
    client.sendMessage(":server 441 " + client.getNickname() + " " +
                       targetNickname + " " + channelName +
                       " :They aren't on that channel\r\n");
    return;
  }

  // チャンネルのオペレーターでない場合はエラー
  if (!channel->isOperator(client.getNickname())) {
    client.sendMessage(":server 482 " + client.getNickname() + " " +
                       channelName + " :You're not channel operator\r\n");
    return;
  }

  // ターゲットをチャンネルから削除
  channel->removeClient(targetClient);

  // KICK通知をそのチャンネルの全員に送信
  std::string kickMsg = ":" + client.getNickname() + "!KICK!" +
                        client.getUsername() + "@localhost KICK " +
                        channelName + " :" + targetNickname + "\r\n";
  for (std::set<Client*>::iterator it = channel->getClients().begin();
       it != channel->getClients().end(); ++it) {
    (*it)->sendMessage(kickMsg);
  }

  // ターゲットにも通知
  std::string targetMsg = ":" + client.getNickname() + "!KICK!" +
                          client.getUsername() + "@localhost KICK " +
                          channelName + " :" + targetNickname + "\r\n";
  targetClient->sendMessage(targetMsg);
  targetClient->sendMessage("You have been kicked from " + channelName +
                            "\r\n");
  close(targetClient->getFd());
}
