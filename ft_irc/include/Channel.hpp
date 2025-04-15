#pragma once

#include <set>
#include <string>

class Channel {
 public:
  Channel(const std::string& name);
  ~Channel();

 private:
  std::string _name;
  std::set<int> _members;
};
