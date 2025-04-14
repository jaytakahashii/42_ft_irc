#include "pollServer.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

void run_poll_server(int port) {
  // 1. サーバー用ソケットの作成
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    std::cerr << "❌ ソケット作成に失敗: " << strerror(errno) << std::endl;
    return;
  }

  // 2. アドレス再利用オプション設定（再起動時の "Address already in use" 防止）
  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  // 3. サーバーアドレス構造体の初期化
  struct sockaddr_in server_addr;
  std::memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port =
      htons(port);  // ポート番号（ネットワークバイトオーダー）
  server_addr.sin_addr.s_addr = INADDR_ANY;  // 全てのインターフェースで待ち受け

  // 4. bind() でアドレスとポートをソケットに結び付ける
  if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) ==
      -1) {
    std::cerr << "❌ バインド失敗: " << strerror(errno) << std::endl;
    close(server_fd);
    return;
  }

  // 5. listen() で接続待機状態にする
  if (listen(server_fd, MAX_CLIENTS) == -1) {
    std::cerr << "❌ リッスン失敗: " << strerror(errno) << std::endl;
    close(server_fd);
    return;
  }

  std::cout << "✅ ポート " << port << " で接続待機中..." << std::endl;

  std::vector<struct pollfd> fds;

  // 6. サーバーソケットを poll の監視対象に追加
  struct pollfd server_pollfd;
  server_pollfd.fd = server_fd;
  server_pollfd.events = POLLIN;  // 読み込み可能イベントを監視
  server_pollfd.revents = 0;
  fds.push_back(server_pollfd);

  char buffer[BUFFER_SIZE];

  // 7. メインループ（イベント駆動）
  while (true) {
    int activity = poll(&fds[0], fds.size(), -1);  // 無限待機
    if (activity < 0) {
      std::cerr << "❌ poll() エラー: " << strerror(errno) << std::endl;
      break;
    }

    for (size_t i = 0; i < fds.size(); ++i) {
      if (fds[i].revents & POLLIN) {
        if (fds[i].fd == server_fd) {
          // クライアントから新規接続要求
          struct sockaddr_in client_addr;
          socklen_t client_len = sizeof(client_addr);
          int client_fd =
              accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
          if (client_fd == -1) {
            std::cerr << "❌ accept() 失敗: " << strerror(errno) << std::endl;
            continue;
          }

          std::cout << "🟢 新しい接続: " << inet_ntoa(client_addr.sin_addr)
                    << ":" << ntohs(client_addr.sin_port) << std::endl;

          // クライアントソケットを poll 対象に追加
          struct pollfd client_pollfd;
          client_pollfd.fd = client_fd;
          client_pollfd.events = POLLIN;
          client_pollfd.revents = 0;
          fds.push_back(client_pollfd);
        } else {
          // クライアントからデータ受信
          std::memset(buffer, 0, BUFFER_SIZE);
          int bytes = recv(fds[i].fd, buffer, BUFFER_SIZE - 1, 0);
          if (bytes <= 0) {
            std::cout << "🔴 切断: fd " << fds[i].fd << std::endl;
            close(fds[i].fd);
            fds.erase(fds.begin() + i);
            --i;
          } else {
            std::cout << "📩 fd " << fds[i].fd << " から受信: " << buffer;

            if (std::strncmp(buffer, "exit", 4) == 0) {
              std::cout << "👋 'exit' 受信、切断します: fd " << fds[i].fd
                        << std::endl;
              close(fds[i].fd);
              fds.erase(fds.begin() + i);
              --i;
            }
          }
        }
      }
    }
  }

  // 全ソケットクローズ（終了処理）
  for (size_t i = 0; i < fds.size(); ++i)
    close(fds[i].fd);
}
