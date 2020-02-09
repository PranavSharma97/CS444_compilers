#pragma once

#include "token.h"

#include <string>
#include <vector>
#include <map>

class Weeder{
  std::string class_name;
  Token m_parse_tree;

  void shrink(Token& node);
  bool weed(Token& node,std::map<TokenType,int>& conditions);
  void split(const std::string& s, char delim, std::vector<std::string>& lst);
  // Return true if any of the key is found
  bool search_any(Token& node, std::map<TokenType,int>& keys);
  // Return true of all of the key is found for specified times.
  // I.E. the values in the map are all 0.
  bool search_all(Token& node, std::map<TokenType,int>& keys);
  bool search(Token& node, TokenType key);
 public:
  Weeder(const std::string& file_name, const Token& parse_tree);
  bool weed();
};
