#pragma once
#include <set>
#include <string>
#include <vector>

class Client;

class Channel {
 private:
  std::string _name;
  std::set<Client*> _clients;
  std::vector<std::string> _operators;
  std::string _topic;
  std::string _key;

 public:
  Channel(const std::string& name);
  ~Channel();

  const std::string& getName() const;
  void addClient(Client* client);
  Client* getClient(const std::string& nickname) const;
  void removeClient(Client* client);
  const std::set<Client*>& getClients() const;
  bool hasClient(Client* client) const;
  int getClientCount() const;
  void addOperator(const std::string& operatorName);
  bool isOperator(const std::string& operatorName) const;
  void removeOperator(const std::string& operatorName);
  const std::vector<std::string>& getOperators() const;
  void setTopic(const std::string& topic);
  const std::string& getTopic() const;
  void sendToAll(const std::string& message) const;

  void setKey(const std::string& key);
  const std::string& getKey() const;
  bool hasKey() const;
};
