#pragma once

#include <vector>
#include <stack>
#include <map>
#include <utility>
#include <string>

#include "lr1_stack_layer.h"

class ParseTable{
  
  // If m_states_count is -1, cfg not read in
  int m_states_count;
  // Rules are in reverse order
  std::vector<std::vector<TokenType>> m_reduce_rules;

  // Given current stte, the next token, do a reduce or shift
  // m_parse_table.size() == m_states_count
  std::vector<std::map<TokenType,std::pair<char,int>>> m_parse_table;

  // The actual map used for determining if a token is terminal token
  // If this is the same as the numbering in tokens.h, use the number of token.
  std::map<TokenType,bool> is_terminal_token;
  std::map<TokenType,bool> is_non_terminal_token;

  // Eventually we should reach to this token
  TokenType m_start_token;
  
  void readin(std::string input_file);

  std::stack<LR1StackLayer> m_stack;
  
  // public:
  public:
  Token parse_tree;
  
  ParseTable();
  ParseTable(std::string& input_file);

  // The actual parsing part. Takes in vector of tokens produced by scanner.
  // Produces true or false.
  bool parse(std::vector<Token>& token_vec);
};
