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
