#include "Client.hpp"

#include <sys/socket.h>

Client::Client(int fd) : _fd(fd), _authenticated(false), _registered(false) {
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
  // TODO
  return !nickname.empty() && nickname.size() <= 9;
}

bool Client::isValidUsername(const std::string& username) const {
  // TODO
  return !username.empty() && username.size() <= 9;
}

bool Client::isValidRealname(const std::string& realname) const {
  // TODO
  return !realname.empty() && realname.size() <= 9;
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
