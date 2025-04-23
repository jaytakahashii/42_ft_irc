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
  _clients.erase(client);
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
