#include "Channel.hpp"

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
