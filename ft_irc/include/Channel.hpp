#pragma once
#include <set>
#include <string>

#include "Client.hpp"

class Channel {
 private:
  std::string _name;
  std::set<Client*> _clients;

 public:
  Channel(const std::string& name);
  ~Channel();

  const std::string& getName() const;
  void addClient(Client* client);
  void removeClient(Client* client);
  const std::set<Client*>& getClients() const;
  bool hasClient(Client* client) const;
};
