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
