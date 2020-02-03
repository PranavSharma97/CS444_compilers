#include <vector>
#include <iostream>
#include <string>
#include "extra_token_logic.h"
#include "token_types.h"
#include "token.h"
using namespace std;

void updateType(vector<Token>::iterator token, TokenType type, string name) {
  token->m_type = type;
  token->m_display_name = name;
}

vector<Token> extraTokenLogic(vector<Token> tokens) {
	for(vector<Token>::iterator it = tokens.begin(); it != tokens.end(); it++) {
		switch(it->m_type) {
			case ASSIGNMENT_OP:
			  if (it->m_lex == "=") { updateType(it, T_EQUAL, "T_EQUAL"); }
        else if (it->m_lex == "+=") { updateType(it, T_PLUS_EQUAL, "T_PLUS_EQUAL"); }
        else if (it->m_lex == "-=") { updateType(it, T_MINUS_EQUAL, "T_MINUS_EQUAL"); }
        else if (it->m_lex == "*=") { updateType(it, T_STAR_EQUAL, "T_STAR_EQUAL"); }
        else if (it->m_lex == "/=") { updateType(it, T_SLASH_EQUAL, "T_SLASH_EQUAL"); }
        else if (it->m_lex == "&=") { updateType(it, T_AND_EQUAL, "T_AND_EQUAL"); }  
        else if (it->m_lex == "|=") { updateType(it, T_OR_EQUAL, "T_OR_EQUAL"); } 
        else if (it->m_lex == "^=") { updateType(it, T_EXP_EQUAL, "T_EXP_EQUAL"); } 
        else if (it->m_lex == "%=") { updateType(it, T_PERCENT_EQUAL, "T_PERCENT_EQUAL"); } 
        else if (it->m_lex == "<<=") { updateType(it, T_LESS_LESS_EQUAL, "T_AND_EQUAL"); } 
        else if (it->m_lex == ">>=") { updateType(it, T_GREATER_GREATER_EQUAL, "T_GREATER_GREATER_EQUAL"); } 
        else if (it->m_lex == ">>>=") { updateType(it, T_GREATER_GREATER_GREATER_EQUAL, "T_GREATER_GREATER_GREATER_EQUAL"); } 
        break;
      case INFIX_OP:
        if (it->m_lex == "||") { updateType(it, T_OR_OR, "T_OR_OR"); }
        else if (it->m_lex == "&&") { updateType(it, T_AND_AND, "T_AND_AND"); }
        else if (it->m_lex == "|") { updateType(it, T_OR, "T_OR"); }
        else if (it->m_lex == "^") { updateType(it, T_EXP, "T_EXP"); }
        else if (it->m_lex == "&") { updateType(it, T_AND, "T_AND"); }  
        else if (it->m_lex == "==") { updateType(it, T_EQUAL_EQUAL, "T_EQUAL_EQUAL"); } 
        else if (it->m_lex == "!=") { updateType(it, T_NOT_EQUAL, "T_NOT_EQUAL"); } 
        else if (it->m_lex == "<") { updateType(it, T_LESS, "T_LESS"); } 
        else if (it->m_lex == ">") { updateType(it, T_GREATER, "T_GREATER"); } 
        else if (it->m_lex == "<=") { updateType(it, T_LESS_EQUAL, "T_LESS_EQUAL"); } 
        else if (it->m_lex == ">=") { updateType(it, T_GREATER_EQUAL, "T_GREATER_EQUAL"); } 
        else if (it->m_lex == "<<") { updateType(it, T_LESS_LESS, "T_LESS_LESS"); } 
        else if (it->m_lex == ">>") { updateType(it, T_GREATER_GREATER, "T_GREATER_GREATER"); } 
        else if (it->m_lex == ">>>") { updateType(it, T_GREATER_GREATER_GREATER, "T_GREATER_GREATER_GREATER"); } 
        else if (it->m_lex == "/") { updateType(it, T_SLASH, "T_SLASH"); } 
        else if (it->m_lex == "%") { updateType(it, T_PERCENT, "T_PERCENT"); } 
        break;
      case (PRE_POST_FIX_OP):
        if (it->m_lex == "++") { updateType(it, T_PLUS_PLUS, "T_PLUS_PLUS"); }  
        else if (it->m_lex == "--") { updateType(it, T_MINUS_MINUS, "T_MINUS_MINUS"); }
        break;
      case(PREFIX_OP):
        if (it->m_lex == "!") { updateType(it, T_NOT, "T_NOT"); }
        else if (it->m_lex == "~") { updateType(it, T_TILDE, "T_TILDE"); }
        break;
      case(BASIC_TYPE):
        if (it->m_lex == "byte") { updateType(it, T_BYTE, "T_BYTE"); }
        else if (it->m_lex == "short") { updateType(it, T_SHORT, "T_SHORT"); }
        else if (it->m_lex == "char") { updateType(it, T_CHAR, "T_CHAR"); }
        else if (it->m_lex == "int") { updateType(it, T_INT, "T_INT"); }
        else if (it->m_lex == "long") { updateType(it, T_LONG, "T_LONG"); }
        else if (it->m_lex == "float") { updateType(it, T_FLOAT, "T_FLOAT"); }
        else if (it->m_lex == "double") { updateType(it, T_DOUBLE, "T_DOUBLE"); }
        else if (it->m_lex == "boolean") { updateType(it, T_BOOLEAN, "T_BOOLEAN"); }        
        break;
      case(MODIFIER):
        if (it->m_lex == "public") { updateType(it, T_PUBLIC, "T_PUBLIC"); }
        else if (it->m_lex == "protected") { updateType(it, T_PROTECTED, "T_PROTECTED"); }
        else if (it->m_lex == "private") { updateType(it, T_PRIVATE, "T_PRIVATE"); }
        else if (it->m_lex == "static") { updateType(it, T_STATIC, "T_STATIC"); }
        else if (it->m_lex == "abstract") { updateType(it, T_ABSTRACT, "T_ABSTRACT"); }
        else if (it->m_lex == "final") { updateType(it, T_FINAL, "T_FINAL"); }
        else if (it->m_lex == "native") { updateType(it, T_NATIVE, "T_NATIVE"); }
        else if (it->m_lex == "synchronized") { updateType(it, T_SYNCHRONIZED, "T_SYNCHRONIZED"); }
        else if (it->m_lex == "transient") { updateType(it, T_TRANSIENT, "T_TRANSIENT"); }
        else if (it->m_lex == "volatile") { updateType(it, T_VOLATILE, "T_VOLATILE"); }
        else if (it->m_lex == "strictfp") { updateType(it, T_STRICTFP, "T_STRICTFP"); }
        break;
      case(KEYWORDS):
        if (it->m_lex == "interface") { updateType(it, T_INTERFACE, "T_INTERFACE"); }
        else if (it->m_lex == "extends") { updateType(it, T_EXTENDS, "T_EXTENDS"); }
        else if (it->m_lex == "super") { updateType(it, T_SUPER, "T_SUPER"); }
        else if (it->m_lex == "this") { updateType(it, T_THIS, "T_THIS"); }
        else if (it->m_lex == "new") { updateType(it, T_NEW, "T_NEW"); }
        else if (it->m_lex == "do") { updateType(it, T_DO, "T_DO"); }
        else if (it->m_lex == "try") { updateType(it, T_TRY, "T_TRY"); }
        else if (it->m_lex == "switch") { updateType(it, T_SWITCH, "T_SWITCH"); }
        else if (it->m_lex == "return") { updateType(it, T_RETURN, "T_RETURN"); }
        else if (it->m_lex == "throw") { updateType(it, T_THROW, "T_THROW"); }
        else if (it->m_lex == "throws") { updateType(it, T_THROWS, "T_THROWS"); }
        else if (it->m_lex == "catch") { updateType(it, T_CATCH, "T_CATCH"); }
        else if (it->m_lex == "break") { updateType(it, T_BREAK, "T_BREAK"); }
        else if (it->m_lex == "continue") { updateType(it, T_CONTINUE, "T_CONTINUE"); }
        else if (it->m_lex == "if") { updateType(it, T_IF, "T_IF"); }
        else if (it->m_lex == "else") { updateType(it, T_ELSE, "T_ELSE"); }
        else if (it->m_lex == "while") { updateType(it, T_WHILE, "T_WHILE"); }
        else if (it->m_lex == "finally") { updateType(it, T_FINALLY, "T_FINALLY"); }
        else if (it->m_lex == "case") { updateType(it, T_CASE, "T_CASE"); }
        else if (it->m_lex == "default") { updateType(it, T_DEFAULT, "T_DEFAULT"); }
        else if (it->m_lex == "for") { updateType(it, T_FOR, "T_FOR"); }
        else if (it->m_lex == "import") { updateType(it, T_IMPORT, "T_IMPORT"); }
        else if (it->m_lex == "package") { updateType(it, T_PACKAGE, "T_PACKAGE"); }
        else if (it->m_lex == "class") { updateType(it, T_CLASS, "T_CLASS"); }
        else if (it->m_lex == "instanceof") { updateType(it, T_INSTANCE_OF, "T_INSTANCE_OF"); }
        else if (it->m_lex == "implements") { updateType(it, T_IMPLEMENTS, "T_IMPLEMENTS"); }
        break;
      default:
        break;
		}
	}
	return tokens;
}
