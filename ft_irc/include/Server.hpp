#pragma once

#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "Client.hpp"

class Server {
 private:
  int _port;
  int _serverSocket;
  std::vector<struct pollfd> _pollFds;
  std::map<int, Client*> _clients;

  void initSocket();                    // ソケット初期化
  void acceptNewClient();               // 新規接続処理
  void handleClientData(int clientFd);  // 受信処理
  void removeClient(int index);         // クライアント削除

  void setNonBlocking(int fd);  // ノンブロッキング設定

 public:
  Server(int port);  // コンストラクタ
  ~Server();         // デストラクタ

  void run();  // メインループ
};
