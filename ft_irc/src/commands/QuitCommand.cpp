#include "commands/QuitCommand.hpp"

#include <unistd.h>

#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "numericsReplies/400-499.hpp"

void QuitCommand::execute(const commandS& cmd, Client& client, Server& server) {
  if (!client.isRegistered()) {
    std::string msg = irc::numericReplies::ERR_NOTREGISTERED();
    client.sendMessage(msg);
    return;
  }

  std::string quitMessage = "";
  if (cmd.args.size() > 0) {
    std::string quitMessage = cmd.args[0];
  }
  std::string quitMsg = ":" + client.getNickname() + "!" +
                        client.getUsername() + "@" + client.getHostname() +
                        " QUIT :" + quitMessage;
  // 全てのクライアントにQUITメッセージを送信
  server.sendAllClients(quitMsg);

  // チャンネルからクライアントを削除
  for (std::map<std::string, Channel*>::iterator it = server.channels.begin();
       it != server.channels.end(); ++it) {
    Channel* channel = it->second;
    if (channel->hasClient(&client)) {
      channel->removeClient(&client);
      if (channel->getClientCount() == 0) {
        delete channel;
        server.channels.erase(it->first);
      }
    }
  }
}
