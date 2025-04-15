#pragma once

#include <string>

class Client {
 public:
  Client(int fd);
  ~Client();

  int getFd() const;
  const std::string& getNickname() const;
  const std::string& getUsername() const;
  bool isRegistered() const;

  void setNickname(const std::string& nickname);
  void setUsername(const std::string& username);
  void setRegistered(bool status);

 private:
  int _fd;
  std::string _nickname;
  std::string _username;
  bool _isRegistered;
};
