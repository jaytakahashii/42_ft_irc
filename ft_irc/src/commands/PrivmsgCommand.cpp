#include "commands/PrivmsgCommand.hpp"

#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "numericsReplies/400-499.hpp"

void PrivmsgCommand::execute(const commandS& cmd, Client& client,
                             Server& server) {
  const std::string& nick =
      client.getNickname().empty() ? "*" : client.getNickname();
  if (!client.isRegistered()) {
    std::string msg = irc::numericReplies::ERR_NOTREGISTERED(nick);
    client.sendMessage(msg);
    return;
  }

  if (cmd.args.size() < 2) {
    std::string msg =
        irc::numericReplies::ERR_NORECIPIENT(client.getNickname(), cmd.name);
    client.sendMessage(msg);
    return;
  }

  std::string target = cmd.args[0];
  std::string message = cmd.args[1];

  // チャンネルにメッセージを送信
  if (target[0] == '#') {
    Channel* channel = server.channels[target];
    // print server.channels
    std::cout << "server.channels: " << std::endl;
    for (std::map<std::string, Channel*>::iterator it = server.channels.begin();
         it != server.channels.end(); ++it) {
      std::cout << "  " << it->first << std::endl;
    }
    if (channel) {
      std::string privmsg = ":" + client.getNickname() + " PRIVMSG " + target +
                            " :" + message + "\r\n";
      server.sendQuitMessageToRelevantClients(client, privmsg);
    } else {
      std::cout << "hello1" << std::endl;
      std::string msg =
          irc::numericReplies::ERR_NOSUCHCHANNEL(client.getNickname(), target);
      client.sendMessage(msg);
    }
  }
  // ユーザーにメッセージを送信
  else {
    std::cout << "hello2" << std::endl;
    Client* recipient = NULL;
    // クライアントのリストから受信者を検索
    for (std::map<int, Client*>::iterator it = server.clients.begin();
         it != server.clients.end(); ++it) {
      if ((*it).second->getNickname() == target) {
        recipient = (*it).second;
        break;
      }
    }

    // 受信者が見つかった場合
    if (recipient) {
      std::cout << recipient << std::endl;
      std::string privmsg = ":" + client.getNickname() + " PRIVMSG " + target +
                            " :" + message + "\r\n";
      std::cout << privmsg << std::endl;
      recipient->sendMessage(privmsg);
      std::cout << "send" << std::endl;
    } else {  // 受信者が見つからない場合
      std::string msg =
          irc::numericReplies::ERR_NOSUCHNICK(client.getNickname(), target);
      client.sendMessage(msg);
    }
  }
}
