#include "token.h"

Token::Token():
  m_type(TokenType::TOKEN_FAILURE),
  m_lex("")
{
  m_rule = -1;
  m_display_name = "TOKEN_FILURE";
}

Token::Token(TokenType type, std::string& lex):
  m_type(type),
  m_lex(lex)
{
  m_rule = -1;
  // Determine the display name
  int type_int = static_cast<int>(type);
  switch(type_int){
  case 0:
    m_display_name = "COMMENT";
    break;
  case 1:
    m_display_name = "ASSIGNMENT_OP";
    break;
  case 2:
    m_display_name = "INFIX_OP";
    break;
  case 3:
    m_display_name = "PRE_POST_FIX_OP";
    break;
  case 4:
    m_display_name = "PREFIX_OP";
    break;
  case 5:
    m_display_name = "T_LEFT_ROUND_BRACKET";
    break;
  case 6:
    m_display_name = "T_RIGHT_ROUND_BRACKET";
    break;
  case 7:
    m_display_name = "T_LEFT_SQUARE_BRACKET";
    break;
  case 8:
    m_display_name = "T_RIGHT_SQUARE_BRACKET";
    break;
  case 9:
    m_display_name = "T_LEFT_CURLY_BRACKET";
    break;
  case 10:
    m_display_name = "T_RIGHT_CULRY_BRACKET";
    break;
  case 11:
    m_display_name = "T_DOT";
    break;
  case 12:
    m_display_name = "T_COMMA";
    break;
  case 13:
    m_display_name = "T_QUESTION";
    break;
  case 14:
    m_display_name = "T_COLON";
    break;
  case 15:
    m_display_name = "T_SEMICOLON";
    break;
  case 16:
    m_display_name = "INT_LITERAL";
    break;
  case 17:
    m_display_name = "FLOAT_LITERAL";
    break;
  case 18:
    m_display_name = "CHAR_LITERAL";
    break;
  case 19:
    m_display_name = "STRING_LITERAL";
    break;
  case 20:
    m_display_name = "BOOL_LITERAL";
    break;
  case 21:
    m_display_name = "NULL_LITERAL";
    break;
  case 22:
    m_display_name = "BASIC_TYPE";
    break;
  case 23:
    m_display_name = "VOID";
    break;
  case 24:
    m_display_name = "MODIFIER";
    break;
  case 25:
    m_display_name = "KEYWORDS";
    break;
  case 26:
    m_display_name = "T_IDENTIFIER";
    break;
  case 27:
    m_display_name = "T_STAR";
    break;
  case 28:
    m_display_name = "T_MINUS";
    break;
  case 29:
    m_display_name = "T_PLUS";
    break;
  default:
    m_display_name = "TOKEN_FAILURE";
    break;
  }
}

Token::Token(TokenType type, int rule, std::vector<Token>& generated_token):
  m_type(type),
  m_rule(rule),
  m_generated_tokens(generated_token)
{
  // Need huge switch statement to determine the display name
}


std::ostream& operator<<(std::ostream& os, const Token& t){
  os << t.m_display_name;
  return os;
}
