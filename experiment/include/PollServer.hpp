#pragma once

#include <netinet/in.h>
#include <poll.h>

#include <vector>

class PollServer {
 public:
  PollServer(int port);
  ~PollServer();

  void run();  // サーバの起動とメインループ

 private:
  int _port;
  int _server_fd;
  std::vector<struct pollfd> _fds;

  bool setupSocket();  // ソケットの初期設定
  void handleNewConnection();
  void handleClientMessage(size_t index);
  void closeAllClients();
};
