#pragma once

#include "token.h"

#include <string>
#include <vector>
#include <map>

class Weeder{
  std::string class_name;
  void shrink(Token& node);
  void BuildAST(Token& node);
  bool weed(Token& node,std::map<TokenType,int>& conditions);
  // Return true if any of the key is found
  bool search_any(Token& node, std::map<TokenType,int>& keys);
  // Return true of all of the key is found for specified times.
  // I.E. the values in the map are all 0.
  bool search_all(Token& node, std::map<TokenType,int>& keys);
  bool search(Token& node, TokenType key);
 public:
  
  Weeder(const std::string& file_name, const Token& parse_tree);
  bool weed();

  Token m_parse_tree;
  Token m_ast_tree;
};
