#pragma once
#include <set>
#include <string>
#include <vector>

#include "Client.hpp"

class Channel {
 private:
  std::string _name;
  std::set<Client*> _clients;
  std::vector<std::string> _operators;

 public:
  Channel(const std::string& name);
  ~Channel();

  const std::string& getName() const;
  void addClient(Client* client);
  void removeClient(Client* client);
  const std::set<Client*>& getClients() const;
  bool hasClient(Client* client) const;
  int getClientCount() const;
  void addOperator(const std::string& operatorName);
  bool isOperator(const std::string& operatorName) const;
  void removeOperator(const std::string& operatorName);
  const std::vector<std::string>& getOperators() const;
};
