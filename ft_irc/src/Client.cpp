#include "Client.hpp"

Client::Client() : _fd(-1) {
}
Client::Client(int fd) : _fd(fd) {
}
Client::~Client() {
}

int Client::getFd() const {
  return _fd;
}

void Client::setNickname(const std::string& nickname) {
  _nickname = nickname;
}

const std::string& Client::getNickname() const {
  return _nickname;
}
