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

  if (!channel.isOperator(client.getNickname())) {
    std::string msg =
        irc::numericReplies::ERR_CHANOPRIVSNEEDED(channel.getName());
    client.sendMessage(msg);
    return;
  }

  if (!isValidSign(cmd.args[1][0])) {
    std::string msg =
        irc::numericReplies::ERR_UNKNOWNMODE(cmd.args[1][0], channel.getName());
    client.sendMessage(msg);
    return;
  }

  bool sign = (cmd.args[1][0] == '+');
  for (std::string::const_iterator it = cmd.args[1].begin() + 1;
       it != cmd.args[1].end(); ++it) {
    if (!isValidMode(*it)) {
      std::string msg =
          irc::numericReplies::ERR_UNKNOWNMODE(*it, channel.getName());
      client.sendMessage(msg);
      return;
    }
  }

  for (std::string::const_iterator it = cmd.args[1].begin() + 1;
       it != cmd.args[1].end(); ++it) {
    if (*it == 'i') {
      channel.setInviteOnly(sign);
    } else if (*it == 't') {
      channel.setTopicRestricted(sign);
    } else if (*it == 'k') {
      if (cmd.args.size() < 3) {
        std::string msg = irc::numericReplies::ERR_NEEDMOREPARAMS(cmd.name);
        client.sendMessage(msg);
        return;
      }
      if (sign) {
        channel.setKey(cmd.args[2]);
      } else {
        if (channel.getKey() != cmd.args[2]) {
          std::string msg =
              irc::numericReplies::ERR_BADCHANNELKEY(channel.getName());
          client.sendMessage(msg);
          return;
        }
        channel.setKey("");
      }
    } else if (*it == 'o') {
      if (sign) {
        channel.addOperator(client.getNickname());
      } else {
        channel.removeOperator(client.getNickname());
      }
    } else if (*it == 'l') {
      if (sign) {
        if (cmd.args.size() < 3) {
          std::string msg = irc::numericReplies::ERR_NEEDMOREPARAMS(cmd.name);
          client.sendMessage(msg);
          return;
        }
        channel.setUserLimit(atoi(cmd.args[2].c_str()), true);
      } else {
        channel.setUserLimit(-1, false);
      }
    }
    std::string msg = irc::numericReplies::RPL_CHANNELMODEIS(
        client.getNickname(), channel.getName(), cmd.args[1], cmd.args[2]);
  }

  std::string serverName = server.getServerName();
  (void)serverName;
}

void ModeCommand::execute(const commandS& cmd, Client& client, Server& server) {
  if (!client.isRegistered()) {
    std::string msg = irc::numericReplies::ERR_NOTREGISTERED();
    client.sendMessage(msg);
    return;
  }

  if (cmd.args.size() < 2) {
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
