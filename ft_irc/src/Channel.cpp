#include "Channel.hpp"

#include <algorithm>

#include "Client.hpp"

// ===== コンストラクタ / デストラクタ =====

Channel::Channel(const std::string& name)
    : _name(name),
      _topic("no topic"),
      _key(""),
      _inviteOnly(false),
      _topicRestricted(false),
      _isUserLimit(false),
      _userLimit(0) {
  // チャンネルの初期化
  _clients.clear();
  _operators.clear();
}

Channel::~Channel() {
  _clients.clear();
  _operators.clear();
}

// ===== 基本情報の取得 =====

const std::string& Channel::getName() const {
  return _name;
}

const std::string& Channel::getTopic() const {
  return _topic;
}

const std::string& Channel::getKey() const {
  return _key;
}

bool Channel::hasKey() const {
  return !_key.empty();
}

// ===== クライアント管理 =====

void Channel::addClient(Client* client) {
  _clients.insert(client);
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

  std::string msg = ":" + client->getNickname() + "!" + client->getUsername() +
                    "@" + client->getHostname() + " PART " + _name + "\r\n";
}

bool Channel::hasClient(Client* client) const {
  return _clients.count(client) > 0;
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

const std::set<Client*>& Channel::getClients() const {
  return _clients;
}

int Channel::getClientCount() const {
  return _clients.size();
}

// ===== オペレータ管理 =====

void Channel::addOperator(const std::string& operatorName) {
  _operators.push_back(operatorName);
}

void Channel::removeOperator(const std::string& operatorName) {
  _operators.erase(
      std::remove(_operators.begin(), _operators.end(), operatorName),
      _operators.end());
}

bool Channel::isOperator(const std::string& operatorName) const {
  return std::find(_operators.begin(), _operators.end(), operatorName) !=
         _operators.end();
}

const std::vector<std::string>& Channel::getOperators() const {
  return _operators;
}

// ===== トピック操作 =====

void Channel::setTopic(const std::string& topic) {
  _topic = topic;
}

// ===== メッセージ送信 =====

void Channel::sendToAll(const std::string& message) const {
  for (std::set<Client*>::const_iterator it = _clients.begin();
       it != _clients.end(); ++it) {
    (*it)->sendMessage(message);
  }
}

// ===== モード設定 =====

void Channel::setKey(const std::string& key) {
  _key = key;
}

void Channel::setInviteOnly(bool inviteOnly) {
  _inviteOnly = inviteOnly;
}

bool Channel::isInviteOnly() const {
  return _inviteOnly;
}

void Channel::setTopicRestricted(bool topicRestricted) {
  _topicRestricted = topicRestricted;
}

bool Channel::isTopicRestricted() const {
  return _topicRestricted;
}

void Channel::setUserLimit(int userLimit, bool isUserLimit) {
  _isUserLimit = isUserLimit;
  if (isUserLimit) {
    _userLimit = userLimit;
  } else {
    _userLimit = -1;
  }
}

std::size_t Channel::getUserLimit() const {
  if (_isUserLimit) {
    return _userLimit;
  }
  return -1;
}
