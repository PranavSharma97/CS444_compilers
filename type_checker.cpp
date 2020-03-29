#include "type_checker.h"
#include <queue>

Type typeCheck(Token* current, Token* currentClass, environment localEnv, Type returnType) {
  
  set<string> numericTypes = {"int", "char", "short", "byte"};

  switch (current->m_type) {
    case BlockStatements:
      Type checkedType;
      for (Token& token: current->m_generated_tokens) {
        // Don't care about return value since it's a statement
        checkedType = typeCheck(&token, currentClass, localEnv, returnType);
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
      // Don't care about return value since it's a statement
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
      // Don't care about return value since it's a statement 
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

      // Don't care, but storing since need to return something
      Type checkedType = typeCheck(&current->m_generated_tokens[current->m_generated_tokens.size() - 1], currentClass, localEnv, returnType);
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

    case Assignment:
      Type leftCheckedType = typeCheck(&current->m_generated_tokens[0], currentClass, localEnv, returnType);
      Type rightCheckedType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType);

      // Check for assignability
      if (isAssignable(leftCheckedType, rightCheckedType) == false) {
        throw; 
      }
      
      return leftCheckedType;
    
    case T_IDENTIFIER:
      Type identifierType;
      
      // Determine whether identifier is a localVariable, parameter or a field
      if (localEnv.localVariables.find(current->m_lex) != localEnv.localVariables.end()) {
        Token* identifierDeclaration = localEnv.localVariables[current->m_lex];
        identifierType = getType(&identifierDeclaration->m_generated_tokens[0]);
      }
      else if (localEnv.formalParameters.find(current->m_lex) != localEnv.formalParameters.end()) {
        Token* identifierDeclaration = localEnv.formalParameters[current->m_lex];
        identifierType = getType(&identifierDeclaration->m_generated_tokens[0]);
      }
      else if (localEnv.fields.find(current->m_lex) != localEnv.fields.end()) {
        Token* identifierDeclaration = localEnv.fields[current->m_lex];
        identifierType = getType(&identifierDeclaration->m_generated_tokens[1]);
      }
      current->checkedType = identifierType;

      return identifierType;

    case NULL_LITERAL:
      Type checkedType;
      checkedType.type = 6;

      return checkedType;
    // DO: Other literals

    case ConditionalOrExpression:
    case ConditionalAndExpression:
    case InclusiveOrExpression:
    case AndExpression:
      Type leftCheckedType = typeCheck(&current->m_generated_tokens[0], currentClass, localEnv, returnType);
      Type rightCheckedType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType);

      // Throw if expressions are not boolean
      if (leftCheckedType.primitive != "boolean" || rightCheckedType.primitive != "boolean") {
        throw;
      }

      Type checkedType;
      checkedType.type = 0;
      checkedType.primitive = "boolean";

      return checkedType;

    case EqualityExpression:
      Type leftCheckedType = typeCheck(&current->m_generated_tokens[0], currentClass, localEnv, returnType);
      Type rightCheckedType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType);

      set<string> numericTypes = {"int", "char", "short", "byte"};
      
      if (numericTypes.find(leftCheckedType.primitive) != numericTypes.end() && numericTypes.find(rightCheckedType.primitive) != numericTypes.end()) {
        Type checkedType;
        checkedType.type = 0;
        checkedType.primitive = "boolean";

        return checkedType;
      }
      // Error if not assignable
      else if ((isAssignable(leftCheckedType, rightCheckedType) == false) && (isAssignable(rightCheckedType, leftCheckedType) == false))  {
        throw;      
      }

      Type checkedType;
      checkedType.type = 0;
      checkedType.primitive = "boolean";

      return checkedType;

    case RelationalExpression:
      Type leftCheckedType = typeCheck(&current->m_generated_tokens[0], currentClass, localEnv, returnType);
      Type rightCheckedType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType);

      set<string> numericTypes = {"int", "char", "short", "byte"};

      if (current->m_generated_tokens[1].m_type == T_INSTANCE_OF) {
        if ((isAssignable(leftCheckedType, rightCheckedType) == false) && (isAssignable(rightCheckedType, leftCheckedType) == false))  {
          throw;
        }
      }
      // Error if left and right expressions are not numericTypes
      else if (numericTypes.find(leftCheckedType.primitive) == numericTypes.end() || numericTypes.find(rightCheckedType.primitive) == numericTypes.end()) {
        throw; 
      }

      Type checkedType;
      checkedType.type = 0;
      checkedType.primitive = "boolean";

      return checkedType;

    case AdditiveExpression:
      Type leftCheckedType = typeCheck(&current->m_generated_tokens[0], currentClass, localEnv, returnType);
      Type rightCheckedType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType);

      if (current->m_generated_tokens[1].m_type == T_PLUS) {
        // If either is String, then Expression type is String
        if ((leftCheckedType.type == 1) && (leftCheckedType.reference->m_generated_tokens[2].m_lex == "String")) {
          Type checkedType;
          checkedType.type = 1;
          checkedType.reference = leftCheckedType.reference;

          return checkedType;
        }
        else if ((rightCheckedType.type == 1) && (rightCheckedType.reference->m_generated_tokens[2].m_lex == "String")) {
          Type checkedType;
          checkedType.type = 1;
          checkedType.reference = rightCheckedType.reference;

          return checkedType;
        }
      }

      // Both operands must be numeric
      if (numericTypes.find(leftCheckedType.primitive) == numericTypes.end() || numericTypes.find(rightCheckedType.primitive) == numericTypes.end()) {
        throw;
      }

      Type checkedType;
      checkedType.type = 0;
      checkedType.primitive = "int";

      return checkedType;

    case MultiplicativeExpression:
      Type leftCheckedType = typeCheck(&current->m_generated_tokens[0], currentClass, localEnv, returnType);
      Type rightCheckedType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType);

      // Both operands must be numeric
      if (numericTypes.find(leftCheckedType.primitive) == numericTypes.end() || numericTypes.find(rightCheckedType.primitive) == numericTypes.end()) {
        throw;
      }

      Type checkedType;
      checkedType.type = 0;
      checkedType.primitive = "int";

      return checkedType;

    case CastExpression:
      

    case UnaryExpressionNotPlusMinus:
      Type checkedType = typeCheck(&current->m_generated_tokens[1], currentClass, localEnv, returnType);

      // Type must be boolean
      if ((checkedType.type != 1) || checkedType.primitive != "boolean") {
        throw;
      }

      return checkedType;
  }
}

bool isAssignable(Type lhs, Type rhs) {
  
  // First check for Array Assignability rules
  if ((rhs.type == 2) || (rhs.type == 3)) {
    // Arrays can be assigned to Object, Cloneable, or Serializable
    if (lhs.type == 1) {
      set<string> valid = {"Object", "Serializable", "Cloneable"};
      if (valid.find(lhs.reference->m_generate_tokens[2].m_lex) == valid.end()) {
        // Throw error
        return false;
      }
    }
    // Throw error, array is assigned to primitive, or primitiveArray = referenceArray, or referenceArray = primitiveArray
    else if ((lhs.type == 0) || ((lhs.type == 3) && (rhs.type == 2)) || ((lhs.type == 2) && (rhs.type == 3))) {
      return false;
    }
    // Check assignability if type is primitive array
    else if ((rhs.type == 2) && (lhs.primitive != rhs.primitive)) {
      return false;
    }
    // Check assignability if type is reference array
    else if ((rhs.type == 3) && (isSubtype(lhs.reference, rhs.reference) == false)) {
      return false;
    }
  }
  // If LeftHandSide is array, but RightHandSide is not, ERROR
  else if ((lhs.type == 2) || (lhs.type == 3) {
    return false;
  }
  
  // Now check for non-array assignability rules
  if ((rhs.type == 1) && rhs.primitive == "byte") {
    // Byte can only be assigned to byte or short or int
    if ((lhs.type != 1) || (lhs.primitive != "byte") || (lhs.primitive != "short") || (lhs.primitive != "int")) {
      return false;
    }
  }
  else if ((rhs.type == 1) && rhs.primitive == "char") {
    // Char can only be assigned to char or int
    if ((lhs.type != 1) || (lhs.primitive != "char") || (lhs.primitive != "int")) {
      return false;
    }
  }
  else if ((rhs.type == 1) && rhs.primitive == "short") {
    // Short can only be assigned to short or int
    if ((lhs.type != 1) || (lhs.primitive != "short") || (lhs.primitive != "int")) {
      return false;
    }
  }
  else if (rhs.type == 1) {
    // Any other primitive type can only be assigned to its same primitive type
    if ((lhs.type != 1) || (lhs.primitive != rhs.primitive) {
      return false;
    }
  }
  else if (rhs.type == 2) {
    // Cannot assign reference to primitive
    if (lhs.type != 2) {
      return false;
    }

    return isSubtype(lhs.reference, rhs.reference);
  }

  return true;
}

// Checks if one class/interface is a subtype of another
bool isSubtype(Token* superType, Token* subType) {
  // Check if superType and subType are the same type
  if (superType == subType) {
    return true;
  }
  // Check if subType inherits from superType
  else if (subType->super_class == superType) {
    return true;
  }
  // Check if subType implements superType
  for (Token* token: subType->super_interfaces) {
    if (token == superType) {
      return true;
    }
  }

  bool isRecursiveSubType = false;
  if (subType->super_class == nullptr) {
    isRecursiveSubType = isSubtype(superType, subType->super_class);
  }

  for (Token* token: subType->super_interfaces) {
    isRecursiveSubType = isRecursiveSubType || isSubtype(superType, token);
  }

  return isRecursiveSubType;
}

Type getType(Token* current) {
  Type nodeType;

  if (current->SearchByTypeDFS(VOID) != nullptr) {
    nodeType.type = 4;
  }

  Token* arrayType = current->SearchByTypeDFS(ArrayType);
  if(arrayType) {
    Token* id = arrayType->SearchByTypeDFS(T_IDENTIFIER);
    if(id) {
      nodeType.type = 3;
      nodeType.reference = id->declaration;
    } else if(arrayType->SearchByTypeDFS(QualifiedName)) {
      Token* qualifiedName = arrayType->SearchByTypeDFS(QualifiedName);
      nodeType.type = 3;
      nodeType.reference = qualifiedName->declaration;
    }
    else {
      nodeType.type = 2;
      nodeType.primitive = arrayType->m_generated_tokens[0].m_lex;
    }
  } else {
    Token* id = current->SearchByTypeDFS(T_IDENTIFIER);
    if(id) {
      nodeType.type = 1;
      nodeType.reference = id->declaration;
    } else if(current->SearchByTypeDFS(QualifiedName)) {
      Token* qualifiedName = current->SearchByTypeDFS(QualifiedName);
      nodeType.type = 1;
      nodeType.reference = qualifiedName->declaration;
    }
    else {
      nodeType.type = 0;
      nodeType.primitive = current->m_lex;
    }
  }

  return nodeType;
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

      returnType = getType(&current->m_generated_tokens[0].m_generated_tokens[1]);

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
