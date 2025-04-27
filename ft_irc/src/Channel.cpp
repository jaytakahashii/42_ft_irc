#include "Channel.hpp"

#include <algorithm>

#include "Client.hpp"

Channel::Channel(const std::string& name) : _name(name) {
}

Channel::~Channel() {
}

const std::string& Channel::getName() const {
  return _name;
}

void Channel::addClient(Client* client) {
  _clients.insert(client);
}

Client* Channel::getClient(const std::string& nickname) const {
  for (std::set<Client*>::const_iterator it = _clients.begin();
       it != _clients.end(); ++it) {
    if ((*it)->getNickname() == nickname) {
      return *it;
    }
  }
  return NULL;
}

void Channel::removeClient(Client* client) {
  if (isOperator(client->getNickname())) {
    _operators.erase(std::remove(_operators.begin(), _operators.end(),
                                 client->getNickname()),
                     _operators.end());
  }
  _clients.erase(client);

  if (_operators.empty()) {
    // 一番前に登録されたクライアントをオペレーターにする
    std::set<Client*>::iterator it = _clients.begin();
    if (it != _clients.end()) {
      _operators.push_back((*it)->getNickname());
    }
  }
}

const std::set<Client*>& Channel::getClients() const {
  return _clients;
}

bool Channel::hasClient(Client* client) const {
  return _clients.count(client) > 0;
}

int Channel::getClientCount() const {
  return _clients.size();
}

void Channel::addOperator(const std::string& operatorName) {
  _operators.push_back(operatorName);
}

bool Channel::isOperator(const std::string& operatorName) const {
  return std::find(_operators.begin(), _operators.end(), operatorName) !=
         _operators.end();
}

void Channel::removeOperator(const std::string& operatorName) {
  _operators.erase(
      std::remove(_operators.begin(), _operators.end(), operatorName),
      _operators.end());
}

const std::vector<std::string>& Channel::getOperators() const {
  return _operators;
}

void Channel::setTopic(const std::string& topic) {
  _topic = topic;
}

const std::string& Channel::getTopic() const {
  return _topic;
}

void Channel::sendToAll(const std::string& message) const {
  for (std::set<Client*>::const_iterator it = _clients.begin();
       it != _clients.end(); ++it) {
    (*it)->sendMessage(message);
  }
}

static bool isInvalidChannelChar(char c) {
  // forbidden: space, ^G, comma, NULL, BELL, CR, LF
  return (c == ' ' || c == 0x07 || c == ',' || c == 0x00 || c == 0x0D ||
          c == 0x0A);
}

bool Channel::isValidName(const std::string& name) const {
  /**
   * Should be started with a '&', '#', '!', or '+'
   * Should be between 1 and 50 characters long
   * should not contain spaces, ^G(ASCII7), comma (,), NULL, BELL, CR, LF
   * Names are case insensitive (e.g. #channel and #Channel are the same)
   */
  if (name.length() < 1 || name.length() > 50) {
    return false;
  }

  if (name[0] == '!' && name.length() >= 6) {
    for (int i = 1; i <= 5; ++i) {
      if (!std::isupper(name[i]) && !std::isdigit(name[i])) {
        return false;
      }
    }
    for (std::size_t i = 6; i < name.length(); ++i) {
      if (isInvalidChannelChar(name[i])) {
        return false;
      }
    }
    return true;
  }

  if (name[0] != '&' && name[0] != '#' && name[0] != '+') {
    return false;
  }

  for (std::size_t i = 1; i < name.length(); ++i) {
    if (isInvalidChannelChar(name[i])) {
      return false;
    }
  }
  return true;
}
