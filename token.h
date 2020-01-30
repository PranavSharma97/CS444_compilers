#pragma once

#include "token_types.h"
#include <string>
#include <iostream>

class Token{
 public:
  TokenType m_type;
  std::string m_lex;
  std::string m_display_name;

  Token(TokenType type, std::string& lex);
  friend std::ostream& operator<<(std::ostream& os, const Token& t);
};

std::ostream& operator<<(std::ostream& os, const Token& t){
  os << t.m_display_name;
  return os;
}
