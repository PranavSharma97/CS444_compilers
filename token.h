#pragma once

#include "token_types.h"
#include <string>
#include <iostream>

class Token{
 public:
  TokenType m_type;
  std::string m_lex;
  std::string m_display_name;

  int m_rule;
  std::vector<Token> m_generated_tokens;
  
  Token(TokenType type, std::string& lex);
  Token(TokenType type, int rule, std::vector<Token>& generated_token);
  friend std::ostream& operator<<(std::ostream& os, const Token& t);
};
