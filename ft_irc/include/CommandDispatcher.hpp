#pragma once
#include "Channel.hpp"
#include "Client.hpp"
#include "SCommand.hpp"
#include "Server.hpp"

class CommandDispatcher {
 private:
  ServerState& _state;
  std::map<std::string, void (CommandDispatcher::*)(const SCommand&, Client&)>
      _commandHandlers;

 public:
  CommandDispatcher(ServerState& state);
  ~CommandDispatcher();

  void dispatch(const SCommand& cmd, Client& client);
  void handlePass(const SCommand& cmd, Client& client);
  void handleNick(const SCommand& cmd, Client& client);
  void handleUser(const SCommand& cmd, Client& client);
  void handlePing(const SCommand& cmd, Client& client);
  void handleJoin(const SCommand& cmd, Client& client);
  void handlePrivmsg(const SCommand& cmd, Client& client);
};
