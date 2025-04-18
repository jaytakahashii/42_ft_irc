#include "commands/QuitCommand.hpp"

#include <unistd.h>

#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

void QuitCommand::execute(const commandS& cmd, Client& client,
                          ServerState& state) {
  std::string quitMsg = ":" + client.getNickname() + " " + cmd.name + "\r\n";
  for (std::map<int, Client*>::iterator it = state.clients.begin();
       it != state.clients.end(); ++it) {
    (*it).second->sendMessage(quitMsg);
  }
  close(client.getFd());
  state.clients.erase(client.getFd());

  // チャンネルからクライアントを削除
  for (std::map<std::string, Channel*>::iterator it = state.channels.begin();
       it != state.channels.end(); ++it) {
    Channel* channel = it->second;
    if (channel->hasClient(&client)) {
      channel->removeClient(&client);
      if (channel->getClientCount() == 0) {
        delete channel;
        state.channels.erase(it->first);
      }
    }
  }
}
