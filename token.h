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

  environment scope;
  Token* declaration;
  bool Protected;

  std::vector<Token> m_generated_tokens;

  Token();
  Token(TokenType type, std::string lex);
  Token(TokenType type, int rule,const std::vector<Token>& generated_token);
  Token(const Token& t) = default;

  TokenType type() const;
  // Get the first token with the same type as type, not including itself.
  Token* SearchByTypeBFS(TokenType type);
  // Get the first token with the same type as type, including itself
  Token* SearchByTypeDFS(TokenType type);
  // Get the one direct child that matches type
  Token* SearchOneChild(TokenType type);
  void clear();
  friend std::ostream& operator<<(std::ostream& os, const Token& t);
};
