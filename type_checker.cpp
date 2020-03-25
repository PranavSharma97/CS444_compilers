#include "type_checker.h"
#include <queue>

Type typeCheck(Token* current, Token* currentClass, environment localEnv, Type returnType) {
  switch (current->m_type) {
    case BlockStatements:
      for (Token& token: current->m_generated_tokens) {
        Type checkedType = typeCheck(&token, currentClass, localEnv, returnType);
      }
      break;
    
    case IfThenStatement:
      Type checkedType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType);
      // Type must be boolean
      if (checkedType.type != 0 || checkedType.primitive != "boolean") {
        throw;
      }

      current->m_generated_tokens[2].checkedType = checkedType;
      break;

    case IfThenElseStatement:
    case IfThenElseStatementNoShortIf:
      Type checkedType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType);
      // Type must be boolean
      if (checkedType.type != 0 || checkedType.primitive != "boolean") {
        throw;
      }

      current->m_generated_tokens[2].checkedType = checkedType;
      typeCheck(&current->m_generated_tokens[4], currentClass, localEnv, returnType);
      typeCheck(&current->m_generated_tokens[6], currentClass, localEnv, returnType);
      break;

    case WhileStatement:
    case WhileStatementNoShortIf:
      Type checkedType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType);
      // Type must be boolean
      if (checkedType.type != 0 || checkedType.primitive != "boolean") {
        throw;
      }
      
      current->m_generated_tokens[2].checkedType = checkedType;
      typeCheck(&current->m_generated_tokens[4], currentClass, localEnv, returnType);
      break;

    case ForStatement:
    case ForStatementNoShortIf:
      // Check if ForInit exists
      if (current->m_generated_tokens[2].m_type != T_SEMICOLON) {
        Type checkedType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType);
        current->m_generated_tokens[2].checkedType = checkedType;
      }

      // Check if Stopping condition exists
      if (current->m_generated_tokens[3].m_type != T_SEMICOLON) {
        Type checkedType = typeCheck(&current->m_generated_tokens[3], currentClass, localEnv, returnType);
        // Type must be boolean
        if (checkedType.type != 0 || checkedType.primitive != "boolean") {
          throw;
        }

        current->m_generated_tokens[3].checkedType = checkedType;
      }
      else if (current->m_generated_tokens[4].m_type != T_SEMICOLON) {
        Type checkedType = typeCheck(&current->m_generated_tokens[4], currentClass, localEnv, returnType);
        // Type must be boolean
        if (checkedType.type != 0 || checkedType.primitive != "boolean") {
          throw;
        }

        current->m_generated_tokens[4].checkedType = checkedType;
      }

      // Check if ForUpdate exists 
      if (current->m_generated_tokens.size() == 9) {
        Type checkedType = typeCheck(&current->m_generated_tokens[6], currentClass, localEnv, returnType);
        current->m_generated_tokens[6].checkedType = checkedType;
      }
      else if (current->m_generated_tokens.size() == 8) {
        Type checkedType = typeCheck(&current->m_generated_tokens[5], currentClass, localEnv, returnType);
        current->m_generated_tokens[5].checkedType = checkedType;
      }
      else if (current->m_generated_tokens.size() == 7) {
        Type checkedType = typeCheck(&current->m_generated_tokens[4], currentClass, localEnv, returnType);
        current->m_generated_tokens[4].checkedType = checkedType;
      }

      typeCheck(&current->m_generated_tokens[current->m_generated_tokens.size() - 1], currentClass, localEnv, returnType);
      break;

    case ReturnStatement:
      if (current->m_generated_tokens[1].m_type != T_SEMICOLON) {
        Type checkedType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType);
        // Check if Type doesn't match with return type
        if (returnType.type != checkedType.type) {
          throw;
        }
        // Check if return type is primitive or primitive array and don't match
        else if ((returnType.type == 0 || returnType.type == 2) && (returnType.primitive != checkedType.primitive)) {
          throw; 
        }
        // Check if return type is reference or reference array and don't match
        else if ((returnType.type == 1 || returnType.type == 3) && (returnType.reference != checkedType.reference)) {
          throw;
        }
      }
      else if (returnType.type != 4) {
        // Throw error since method return type is not VOID but return statement is blank
        throw;
      }
      break;
  }
}

bool checkInheritance(Token* superClass, Token* subClass) {

}

bool checkTypes(Token* astRoot) {
  // Find all MethodDeclarations, then call typeCheck to check and
  // assign types to everything
  
  // Do BFS to find all methods

  queue<Token*> tokens;
  Token* current;
  tokens.push(current);
  while (tokens.size() > 0) {
    current = tokens.pop();
    if (current->m_type == MethodDeclaration) {
      Token* currentClass = current->compilation_unit->searchByTypeDFS(ClassDeclaration);
      Type returnType;
     
      if (current->m_generated_tokens[0].m_generated_tokens[1].SearchByTypeDFS(VOID) != nullptr) {
        returnType.type = 4;
      }

      Token* arrayType = current->m_generated_tokens[0].m_generated_tokens[1].SearchByTypeDFS(ArrayType);
      if(arrayType) {
        Token* id = arrayType->SearchByTypeDFS(T_IDENTIFIER);
        if(id) {
          returnType.type = 3;
          returnType.reference = id->declaration;
        } else if(arrayType->SearchByTypeDFS(QualifiedName)) {
          Token* qualifiedName = arrayType->SearchByTypeDFS(QualifiedName);
          returnType.type = 3;
          returnType.reference = qualifiedName->declaration;
        }
        else {
          returnType.type = 2;
          returnType.primitive = arrayType->m_generated_tokens[0].m_lex;
        }
      } else {
        Token* id = current->m_generated_tokens[0].m_generated_tokens[1].SearchByTypeDFS(T_IDENTIFIER);
        if(id) {
          returnType.type = 1;
          returnType.reference = id->declaration;
        } else if(current->m_generated_tokens[0].m_generated_tokens[1].SearchByTypeDFS(QualifiedName)) {
          Token* qualifiedName = current->m_generated_tokens[0].m_generated_tokens[1].SearchByTypeDFS(QualifiedName);
          returnType.type = 1;
          returnType.reference = qualifiedName->declaration;
        }
        else {
          returnType.type = 0;
          returnType.primitive = current->m_generated_tokens[0].m_generated_tokens[1].m_lex;
        }
      }

      current->checkedTyped = returnType;
      if (current->m_generated_tokens[1].m_type == Block && current->m_generated_tokens[1].m_generated_tokens[1] == BlockStatements) {
        Type checkedType = typeCheck(current, currentClass, current->scope, returnType);    
      }
    }
    for (Token& token: current->m_generated_tokens) {
      tokens.push(&token);
    }
  }
}
