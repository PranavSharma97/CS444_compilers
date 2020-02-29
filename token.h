#pragma once

#include "token_types.h"
#include "environment.h"
#include <string>
#include <vector>

class Token{
 public:
  TokenType m_type;
  std::string m_lex;
  std::string m_display_name;

  int m_rule;

  environment *scope;

  std::vector<Token> m_generated_tokens;

  Token();
  Token(TokenType type, std::string lex);
  Token(TokenType type, int rule,const std::vector<Token>& generated_token);
  Token(const Token& t) = default;
  void clear();
  friend std::ostream& operator<<(std::ostream& os, const Token& t);
};
