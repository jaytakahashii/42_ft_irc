#include <errno.h>       // errno
#include <netinet/in.h>  // sockaddr_in
#include <sys/socket.h>  // socket, bind, listen, accept
#include <unistd.h>      // close

#include <cstring>  // strerror
#include <iostream>

#define PORT 4242
#define BUFFER_SIZE 1024

int main() {
  int server_fd, client_fd;
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_len = sizeof(client_addr);
  char buffer[BUFFER_SIZE];

  // 1. ソケットの作成
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    std::cerr << "Socket creation failed: " << strerror(errno) << std::endl;
    return 1;
  }

  // 2. サーバのアドレス構造体を設定
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;          // IPv4
  server_addr.sin_addr.s_addr = INADDR_ANY;  // 全インターフェースから受け入れる
  server_addr.sin_port = htons(PORT);        // ポート番号

  // 3. ソケットにアドレスをバインド
  if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) <
      0) {
    std::cerr << "Bind failed: " << strerror(errno) << std::endl;
    close(server_fd);
    return 1;
  }

  // 4. 接続待ち状態にする
  if (listen(server_fd, 5) < 0) {
    std::cerr << "Listen failed: " << strerror(errno) << std::endl;
    close(server_fd);
    return 1;
  }

  std::cout << "📡 Server is listening on port " << PORT << "..." << std::endl;

  // 5. クライアントの接続を待つ
  client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
  if (client_fd < 0) {
    std::cerr << "Accept failed: " << strerror(errno) << std::endl;
    close(server_fd);
    return 1;
  }

  std::cout << "✅ Client connected!" << std::endl;

  // 6. メッセージの受信と表示ループ
  while (true) {
    memset(buffer, 0, BUFFER_SIZE);
    int bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received <= 0) {
      std::cout << "❌ Client disconnected." << std::endl;
      break;
    }

    std::cout << "📩 Received: " << buffer;

    if (strncmp(buffer, "exit", 4) == 0)
      break;
  }

  // 7. クローズ処理
  close(client_fd);
  close(server_fd);
  std::cout << "🔒 Server closed." << std::endl;

  return 0;
}
