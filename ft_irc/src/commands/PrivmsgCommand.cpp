#include "commands/PrivmsgCommand.hpp"

#include "Channel.hpp"
#include "Client.hpp"

void PrivmsgCommand::execute(const commandS& cmd, Client& client,
                             ServerState& state) {
  if (cmd.args.size() < 2) {
    client.sendMessage(":server 411 " + client.getNickname() +
                       " :No recipient given\r\n");
    return;
  }

  std::string target = cmd.args[0];
  std::string message = cmd.args[1];

  // チャンネルにメッセージを送信
  if (target[0] == '#') {
    Channel* channel = state.channels[target];
    if (channel) {
      std::string privmsg = ":" + client.getNickname() + " PRIVMSG " + target +
                            " :" + message + "\r\n";
      for (std::set<Client*>::iterator it = channel->getClients().begin();
           it != channel->getClients().end(); ++it) {
        (*it)->sendMessage(privmsg);
      }
    } else {
      client.sendMessage(":server 403 " + client.getNickname() + " " + target +
                         " :No such channel\r\n");
    }
  }
  // ユーザーにメッセージを送信
  else {
    Client* recipient = NULL;
    // クライアントのリストから受信者を検索
    for (std::map<int, Client*>::iterator it = state.clients.begin();
         it != state.clients.end(); ++it) {
      if ((*it).second->getNickname() == target) {
        recipient = (*it).second;
        break;
      }
    }

    // 受信者が見つかった場合
    if (recipient) {
      std::string privmsg = ":" + client.getNickname() + " PRIVMSG " + target +
                            " :" + message + "\r\n";
      recipient->sendMessage(privmsg);
    } else {  // 受信者が見つからない場合
      client.sendMessage(":server 401 " + client.getNickname() + " " + target +
                         " :No such nick/channel\r\n");
    }
  }
}
