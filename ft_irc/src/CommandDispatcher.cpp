#include "CommandDispatcher.hpp"

#include <iostream>

CommandDispatcher::CommandDispatcher() {
}

CommandDispatcher::~CommandDispatcher() {
}

void CommandDispatcher::dispatch(const Command& cmd, Client& client) {
  std::cout << "Dispatching command: " << cmd.name << " from client "
            << client.getFd() << std::endl;
  // ここでコマンドごとの処理に分岐していく予定
}
