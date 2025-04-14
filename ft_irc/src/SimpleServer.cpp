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
  struct sockaddr_in server_addr;  // サーバのアドレス構造体
  struct sockaddr_in client_addr;  // クライアントのアドレス構造体

  socklen_t client_len =
      sizeof(client_addr);   // クライアントのアドレス構造体のサイズ
  char buffer[BUFFER_SIZE];  // 受信バッファ

  // 1. ソケットの作成
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  // AF_INET: IPv4, SOCK_STREAM: TCP
  // 0: プロトコルは自動選択
  // socket()が成功すると、ソケットのファイルディスクリプタが返る

  // エラーチェック
  if (server_fd < 0) {
    std::cerr << "Socket creation failed: " << strerror(errno) << std::endl;
    return 1;
  }

  // 2. サーバのアドレス構造体を設定
  std::memset(&server_addr, 0, sizeof(server_addr));  // 構造体をゼロクリア

  server_addr.sin_family = AF_INET;          // IPv4
  server_addr.sin_addr.s_addr = INADDR_ANY;  // 全IPアドレスを指定
  server_addr.sin_port = htons(PORT);        // ポート番号(4242)

  // 3. ソケットにアドレスをバインド(OSにソケットを関連付ける)
  // bind()は、ソケットにアドレスを関連付ける関数
  // 第1引数: ソケットのファイルディスクリプタ
  // 第2引数: ソケットのアドレス構造体
  if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) <
      0) {  // bind()は成功すると0を返す
    std::cerr << "Bind failed: " << strerror(errno) << std::endl;
    close(server_fd);
    return 1;
  }

  // 4. 接続待ち状態にする
  // listen()は、ソケットを接続待ち状態にする関数
  // 第1引数: ソケットのファイルディスクリプタ
  // 第2引数: 接続待ちキューのサイズ(接続待ちのクライアント数)
  // listen()は成功すると0を返す
  // エラーチェック
  if (listen(server_fd, 5) != 0) {
    std::cerr << "Listen failed: " << strerror(errno) << std::endl;
    close(server_fd);
    return 1;
  }

  std::cout << "📡 Server is listening on port " << PORT << "..." << std::endl;

  // 5. クライアントの接続を待つ
  // accept()は、接続要求を受け入れる関数
  // 第1引数: ソケットのファイルディスクリプタ
  // 第2引数: クライアントのアドレス構造体
  // 第3引数: クライアントのアドレス構造体のサイズ
  // accept()は成功すると、クライアントのソケットのファイルディスクリプタを返す
  client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
  if (client_fd < 0) {
    std::cerr << "Accept failed: " << strerror(errno) << std::endl;
    close(server_fd);
    return 1;
  }

  std::cout << "✅ Client connected!" << std::endl;

  // 6. メッセージの受信と表示ループ
  while (true) {
    std::memset(buffer, 0, BUFFER_SIZE);  // バッファをゼロクリア
    int bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received <= 0) {
      std::cout << "❌ Client disconnected." << std::endl;
      break;
    }

    std::cout << "📩 Received: " << buffer;

    if (std::strncmp(buffer, "exit", 4) == 0)
      break;
  }

  // 7. クローズ処理
  close(client_fd);
  close(server_fd);
  std::cout << "🔒 Server closed." << std::endl;

  return 0;
}
