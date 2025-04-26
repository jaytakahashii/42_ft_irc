#pragma once
#include <string>

class Client {
 private:
  int _fd;
  bool _authenticated;  // PASS
  bool _registered;     // PASS -> NICK -> USER
  std::string _hostname;
  std::string _readBuffer;
  std::string _nickname;
  std::string _username;
  std::string _realname;

  std::string _getClientHostname() const;

 public:
  Client(int fd);
  ~Client();

  int getFd() const;
  std::string& getReadBuffer();  // Buffer for incoming messages

  bool isAuthenticated() const;
  void setAuthenticated(bool authenticated);

  bool isRegistered() const;
  void setRegistered(bool isRegistered);

  bool isValidNickname(const std::string& nickname) const;
  bool isValidUsername(const std::string& username) const;
  bool isValidRealname(const std::string& realname) const;

  void sendMessage(const std::string& message) const;

  const std::string& getNickname() const;
  const std::string& getUsername() const;
  const std::string& getRealname() const;
  void setNickname(const std::string& nickname);
  void setUsername(const std::string& username);
  void setRealname(const std::string& realname);

  const std::string& getHostname() const;
};
