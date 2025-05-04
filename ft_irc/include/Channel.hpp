#pragma once

#include <set>
#include <string>
#include <vector>

class Client;

/**
 * @brief IRCのチャネルを表すクラス
 * モード情報（i, t, k, l）や、参加クライアント、トピックなどを管理します。
 */
class Channel {
 private:
  // ===== チャネル基本情報 =====

  std::string _name;
  std::string _topic;
  std::string _key;  // 入室パスワード（k モード）

  // ===== クライアント・オペレータ管理 =====

  std::set<Client*> _clients;
  std::vector<std::string> _operators;

  // ===== モード状態 =====

  bool _inviteOnly;
  bool _topicRestricted;
  bool _isUserLimit;
  int _userLimit;

 public:
  // ===== コンストラクタ / デストラクタ =====

  Channel(const std::string& name);
  ~Channel();

  // ===== 基本情報の取得 =====

  const std::string& getName() const;
  const std::string& getTopic() const;
  const std::string& getKey() const;
  bool hasKey() const;

  // ===== クライアント管理 =====

  void addClient(Client* client);
  void removeClient(Client* client);
  bool hasClient(Client* client) const;
  Client* getClient(const std::string& nickname) const;
  const std::set<Client*>& getClients() const;
  int getClientCount() const;

  // ===== オペレータ管理 =====

  void addOperator(const std::string& operatorName);
  void removeOperator(const std::string& operatorName);
  bool isOperator(const std::string& operatorName) const;
  const std::vector<std::string>& getOperators() const;

  // ===== トピック操作 =====

  void setTopic(const std::string& topic);

  // ===== メッセージ送信 =====

  void sendToAll(const std::string& message) const;

  // ===== モード設定 =====

  void setKey(const std::string& key);
  void setInviteOnly(bool inviteOnly);
  bool isInviteOnly() const;
  void setTopicRestricted(bool topicRestricted);
  bool isTopicRestricted() const;
  void setUserLimit(int userLimit, bool isUserLimit);
  std::size_t getUserLimit() const;
};
