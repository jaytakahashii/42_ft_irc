#pragma once
#include <string>

class Client {
 private:
  int _fd;
  std::string _readBuffer;
  std::string _nickname;

 public:
  Client(int fd);
  ~Client();

  int getFd() const;
  std::string& getReadBuffer();
  const std::string& getNickname() const;
  void setNickname(const std::string& nickname);
};
