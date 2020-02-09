#pragma once

#include "token.h"

#include <string>
#include <vector>
#include <map>

class Weeder{
  std::string class_name;
  Token m_parse_tree;

  void shrink(Token& node);
  bool search(Token& node, TokenType key);
  bool weed(Token& node,std::map<TokenType,int>& conditions);
  void split(const std::string& s, char delim, std::vector<std::string>& lst);
 public:
  Weeder(const std::string& file_name, const Token& parse_tree);
  bool weed();
};
