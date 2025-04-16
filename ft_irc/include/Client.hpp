#pragma once
#include <string>

class Client {
 private:
  int _fd;
  std::string _readBuffer;
  std::string _nickname;
  std::string _username;
  std::string _realname;

 public:
  Client(int fd);
  ~Client();

  int getFd() const;
  std::string& getReadBuffer();
  void sendMessage(const std::string& message) const;
  bool isFullyRegistered() const;
  const std::string& getNickname() const;
  const std::string& getUsername() const;
  const std::string& getRealname() const;
  void setNickname(const std::string& nickname);
  void setUsername(const std::string& username);
  void setRealname(const std::string& realname);
};
