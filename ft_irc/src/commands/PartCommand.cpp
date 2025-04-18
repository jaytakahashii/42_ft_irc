#include "commands/PartCommand.hpp"

#include <sys/socket.h>

#include "Channel.hpp"

void PartCommand::execute(const commandS& cmd, Client& client,
                          serverStateS& state) {
  if (cmd.args.empty()) {
    client.sendMessage("461 PART :Not enough parameters\r\n");
    return;
  }

  std::string channelName = cmd.args[0];

  // チャンネルが存在しない場合はエラー
  if (state.channels.find(channelName) == state.channels.end()) {
    client.sendMessage(":server 403 " + client.getNickname() + " " +
                       channelName + " :No such channel\r\n");
    return;
  }

  Channel* channel = state.channels[channelName];  // チャンネルを取得

  // チャンネルに参加していない場合はエラー
  if (!channel->hasClient(&client)) {
    client.sendMessage(":server 442 " + client.getNickname() + " " +
                       channelName + " :You're not on that channel\r\n");
    return;
  }
}
