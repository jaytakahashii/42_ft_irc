#pragma once

#include "Client.hpp"
#include "Server.hpp"

class CommandHandler {
 public:
  static void handleCommand(Server& server, Client& client,
                            const std::string& line);
};
