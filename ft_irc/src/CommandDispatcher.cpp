/**
 * @file CommandDispatcher.cpp
 * @brief CommandDispatcher class implementation
 * このクラスは、IRCサーバーのコマンドを処理するためのクラスです。
 * コマンドのディスパッチ、各コマンドのハンドリングを行います。
 * コマンドは、クライアントから送信された文字列を解析して生成されます。
 */

#include "CommandDispatcher.hpp"

#include <sys/socket.h>

#include <iostream>

#include "commands/JoinCommand.hpp"
#include "commands/KickCommand.hpp"
#include "commands/NickCommand.hpp"
#include "commands/PartCommand.hpp"
#include "commands/PassCommand.hpp"
#include "commands/PingCommand.hpp"
#include "commands/PrivmsgCommand.hpp"
#include "commands/QuitCommand.hpp"
#include "commands/TopicCommand.hpp"
#include "commands/UserCommand.hpp"

CommandDispatcher::CommandDispatcher(serverStateS& state) : _state(state) {
  _commandHandlers["PASS"] = new PassCommand();
  _commandHandlers["NICK"] = new NickCommand();
  _commandHandlers["USER"] = new UserCommand();
  _commandHandlers["JOIN"] = new JoinCommand();
  _commandHandlers["PART"] = new PartCommand();
  _commandHandlers["PRIVMSG"] = new PrivmsgCommand();
  _commandHandlers["PING"] = new PingCommand();
  _commandHandlers["QUIT"] = new QuitCommand();
  _commandHandlers["KICK"] = new KickCommand();
  _commandHandlers["TOPIC"] = new TopicCommand();
  // TODO : 他のコマンドもここに追加
}

CommandDispatcher::~CommandDispatcher() {
}

void CommandDispatcher::dispatch(const commandS& cmd, Client& client) {
  std::cout << "Dispatching command: " << cmd.name << " from client "
            << client.getFd() << std::endl;

  if (!client.isAuthenticated() && cmd.name != "PASS") {
    std::string msg =
        ":server 451 " + client.getNickname() + " :You have not registered\r\n";
    send(client.getFd(), msg.c_str(), msg.size(), 0);
    return;
  }

  if (_commandHandlers.find(cmd.name) != _commandHandlers.end()) {
    _commandHandlers[cmd.name]->execute(cmd, client, _state);
    return;
  }
  std::string msg = ":server 421 " + client.getNickname() + " " + cmd.name +
                    " :Unknown command\r\n";
  send(client.getFd(), msg.c_str(), msg.size(), 0);
}
