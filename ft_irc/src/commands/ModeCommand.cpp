#include "commands/ModeCommand.hpp"

#include "Channel.hpp"
#include "Server.hpp"
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

void channelMode(const commandS& cmd, Channel& channel, Client& client,
                 Server& server) {
  // TODO

  std::string mode = cmd.args[1];
  (void)mode;

  // TODO unused
  std::string pass = server.getServerPassword();
  (void)pass;

  std::string clientName = client.getNickname();
  (void)clientName;

  std::string channelName = channel.getName();
  (void)channelName;
}

void ModeCommand::execute(const commandS& cmd, Client& client, Server& server) {
  if (!client.isRegistered()) {
    std::string msg = irc::numericReplies::ERR_NOTREGISTERED();
    client.sendMessage(msg);
    return;
  }

  if (cmd.args.size() < 1) {
    std::string msg = irc::numericReplies::ERR_NEEDMOREPARAMS(cmd.name);
    client.sendMessage(msg);
    return;
  }

  std::string channelPrefix = "#+!&";
  if (channelPrefix.find(cmd.args[0][0]) !=
      std::string::npos) {  // チャンネルモード
    std::string channelName = cmd.args[0];
    Channel* channel = server.channels[channelName];

    if (!channel) {
      std::string msg = irc::numericReplies::ERR_NOSUCHCHANNEL(channelName);
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
