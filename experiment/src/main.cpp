#include "PollServer.hpp"

int main() {
  PollServer server(4242);  // 任意のポート
  server.run();
  return 0;
}
