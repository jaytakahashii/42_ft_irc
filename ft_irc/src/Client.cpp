#include "Client.hpp"

#include <sys/socket.h>

Client::Client(int fd) : _fd(fd), _authenticated(false) {
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

std::string& Client::getReadBuffer() {
  return _readBuffer;
}

void Client::sendMessage(const std::string& message) const {
  send(_fd, message.c_str(), message.size(), 0);
}

bool Client::isFullyRegistered() const {
  return !_nickname.empty() && !_username.empty() && !_realname.empty();
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
