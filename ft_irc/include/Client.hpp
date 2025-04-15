#pragma once

#include <string>

class Client {
 public:
  Client();
  Client(int fd);
  ~Client();

  int getFd() const;
  void setNickname(const std::string& nickname);
  const std::string& getNickname() const;

  // 状態情報（登録済みか、ユーザー名など）も管理予定

 private:
  int _fd;
  std::string _nickname;
};
