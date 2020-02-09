#pragma once

#include "token.h"

#include <string>
#include <vector>


class Weeder{
  std::string class_name;
  Token m_parse_tree;

  
  bool weed(Token& node);
  void split(const std::string& s, char delim, std::vector<std::string>& lst);
 public:
  Weeder(const std::string& file_name, const Token& parse_tree);
  bool weed();
};
