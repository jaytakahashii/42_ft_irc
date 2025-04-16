#include "Client.hpp"

Client::Client(int fd) : _fd(fd) {
}

Client::~Client() {
}

int Client::getFd() const {
  return _fd;
}

std::string& Client::getReadBuffer() {
  return _readBuffer;
}

const std::string& Client::getNickname() const {
  return _nickname;
}

void Client::setNickname(const std::string& nickname) {
  _nickname = nickname;
}
