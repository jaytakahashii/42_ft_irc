#include "commands/QuitCommand.hpp"

#include <unistd.h>

#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "numericsReplies/400-499.hpp"

void QuitCommand::execute(const commandS& cmd, Client& client, Server& server) {
  const std::string& nick =
      client.getNickname().empty() ? "*" : client.getNickname();
  if (!client.isRegistered()) {
    std::string msg = irc::numericReplies::ERR_NOTREGISTERED(nick);
    client.sendMessage(msg);
    return;
  }

  std::string quitMessage = "";
  if (cmd.args.size() > 0) {
    quitMessage = " :" + cmd.args[0];
  }
  std::string quitMsg = ":" + client.getNickname() + "!" +
                        client.getUsername() + "@" + client.getHostname() +
                        " QUIT" + quitMessage + "\r\n";
  // 全てのクライアントにQUITメッセージを送信
  server.sendAllClients(quitMsg);
  close(client.getFd());
}
