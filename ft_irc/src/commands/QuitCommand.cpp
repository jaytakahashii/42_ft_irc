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

  std::string quitMsg = ":" + client.getNickname() + " " + cmd.name + "\r\n";
  for (std::map<int, Client*>::iterator it = server.clients.begin();
       it != server.clients.end(); ++it) {
    (*it).second->sendMessage(quitMsg);
  }
  close(client.getFd());
  // ↓SEGV原因 pollでeraseしてるfdみてる↓
  // server.clients.erase(client.getFd());

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
