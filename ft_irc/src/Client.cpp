#include "Client.hpp"

#include <netdb.h>
#include <sys/socket.h>

std::string Client::_getClientHostname() const {
  struct sockaddr_storage addr;
  socklen_t len = sizeof(addr);
  char host[NI_MAXHOST];

  if (getpeername(_fd, (struct sockaddr*)&addr, &len) == 0) {
    if (getnameinfo((struct sockaddr*)&addr, len, host, sizeof(host), NULL, 0,
                    NI_NUMERICHOST) == 0) {
      return std::string(host);
    }
  }
  return std::string("unknown");
}

Client::Client(int fd) : _fd(fd), _authenticated(false), _registered(false) {
  _hostname = _getClientHostname();
}

Client::~Client() {
}

int Client::getFd() const {
  return _fd;
}

bool Client::isAuthenticated() const {
  return _authenticated;
}

void Client::setAuthenticated(bool authenticated) {
  _authenticated = authenticated;
}

bool Client::isRegistered() const {
  return _registered;
}

void Client::setRegistered(bool isRegistered) {
  _registered = isRegistered;
}

bool Client::isValidNickname(const std::string& nickname) const {
  /**
   * Should be 1-9 characters long
   * Should not contain spaces, colons
   * Should start with a alphabetic character or special character
   * Special characters: "[", "\", "]", "^", "_", "`", "{", "|", "}"
   * Special characters ASCIIs: 91, 92, 93, 94, 95, 96, 123, 124, 125
   */
  if (nickname.empty() || nickname.size() > 9) {
    return false;
  }

  char first = nickname[0];
  if (!isalpha(first) &&
      std::string("[]\\^_`{|}").find(first) == std::string::npos) {
    return false;
  }

  for (size_t i = 1; i < nickname.size(); ++i) {
    char c = nickname[i];
    if (!isalnum(c) && std::string("[]\\^_`{|}").find(c) == std::string::npos) {
      return false;
    }
  }

  return true;
}

bool Client::isValidUsername(const std::string& username) const {
  /**
   * * Should be 1-15 characters long
   * * Should not contain NUL, CR, LR, " ", "@"
   */
  if (username.empty() || username.size() > 15) {
    return false;
  }
  for (size_t i = 0; i < username.size(); ++i) {
    char c = username[i];
    if (c == '\0' || c == '\r' || c == '\n' || c == ' ' || c == '@') {
      return false;
    }
  }
  return true;
}

bool Client::isValidRealname(const std::string& realname) const {
  /**
   * * Should be 1-15 characters long
   * * Should not contain NUL, CR, LR, " ", "@"
   */
  if (realname.empty() || realname.size() > 15) {
    return false;
  }
  for (size_t i = 0; i < realname.size(); ++i) {
    char c = realname[i];
    if (c == '\0' || c == '\r' || c == '\n' || c == ' ' || c == '@') {
      return false;
    }
  }
  return true;
}

std::string& Client::getReadBuffer() {
  return _readBuffer;
}

void Client::sendMessage(const std::string& message) const {
  send(_fd, message.c_str(), message.size(), 0);
}

const std::string& Client::getNickname() const {
  return _nickname;
}

const std::string& Client::getUsername() const {
  return _username;
}

const std::string& Client::getRealname() const {
  return _realname;
}

void Client::setNickname(const std::string& nickname) {
  _nickname = nickname;
}

void Client::setUsername(const std::string& username) {
  _username = username;
}

void Client::setRealname(const std::string& realname) {
  _realname = realname;
}

const std::string& Client::getHostname() const {
  return _hostname;
}
