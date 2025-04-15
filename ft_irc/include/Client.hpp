#pragma once
#include <string>

class Client {
 private:
  int _fd;
  std::string _readBuffer;

 public:
  Client(int fd);
  ~Client();

  int getFd() const;
  std::string& getReadBuffer();
};
