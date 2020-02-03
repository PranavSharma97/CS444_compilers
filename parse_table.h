#pragma once

#include <vector>
#include <map>
#include <utility>
#include <string>

#include "token_types.h"

class ParseTable{
  // If m_states_count is -1, cfg not read in
  int m_states_count;
 public:
  std::vector<std::vector<TokenType>> m_reduce_rules;
  std::vector<std::map<TokenType,std::pair<char,int>>> m_parse_table;

  // The actual map used for determining if a token is terminal token
  std::map<TokenType,bool> is_terminal_token;
  std::map<TokenType,bool> is_non_terminal_token;

  TokenType m_start_token;
  
  void readin(std::string input_file);
  
  // public:
  ParseTable();
  ParseTable(std::string& input_file);
  
};
