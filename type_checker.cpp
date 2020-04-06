#include "type_checker.h"
#include <queue>

using namespace std;

TCType typeCheck(Token* current, Token* currentClass, environment localEnv, TCType returnType) {
  
  set<string> numericTypes = {"int", "char", "short", "byte"};

  switch (current->m_type) {
    case BlockStatements:
      TCType checkedType;
      for (Token& token: current->m_generated_tokens) {
        // Don't care about return value since it's a statement
        checkedType = typeCheck(&token, currentClass, localEnv, returnType);
      }

      return checkedType;
    
    case IfThenStatement:
      TCType checkedType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType);
      // Type must be boolean
      if (checkedType.type != 0 || checkedType.primitive != "boolean") {
        throw;
      }

      current->m_generated_tokens[2].checkedType = checkedType;
  
      return checkedType;

    case IfThenElseStatement:
    case IfThenElseStatementNoShortIf:
      TCType checkedType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType);
      // Type must be boolean
      if (checkedType.type != 0 || checkedType.primitive != "boolean") {
        throw;
      }

      current->m_generated_tokens[2].checkedType = checkedType;
      // Don't care about return value since it's a statement
      typeCheck(&current->m_generated_tokens[4], currentClass, localEnv, returnType);
      typeCheck(&current->m_generated_tokens[6], currentClass, localEnv, returnType);

      return checkedType;

    case WhileStatement:
    case WhileStatementNoShortIf:
      TCType checkedType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType);
      // Type must be boolean
      if (checkedType.type != 0 || checkedType.primitive != "boolean") {
        throw;
      }
      
      current->m_generated_tokens[2].checkedType = checkedType;
      // Don't care about return value since it's a statement 
      typeCheck(&current->m_generated_tokens[4], currentClass, localEnv, returnType);

      return checkedType;

    case ForStatement:
    case ForStatementNoShortIf:
      // Check if ForInit exists
      if (current->m_generated_tokens[2].m_type != T_SEMICOLON) {
        TCType checkedType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType);
        current->m_generated_tokens[2].checkedType = checkedType;
      }

      // Check if Stopping condition exists
      if (current->m_generated_tokens[3].m_type != T_SEMICOLON) {
        TCType checkedType = typeCheck(&current->m_generated_tokens[3], currentClass, localEnv, returnType);
        // Type must be boolean
        if (checkedType.type != 0 || checkedType.primitive != "boolean") {
          throw;
        }

        current->m_generated_tokens[3].checkedType = checkedType;
      }
      else if (current->m_generated_tokens[4].m_type != T_SEMICOLON) {
        TCType checkedType = typeCheck(&current->m_generated_tokens[4], currentClass, localEnv, returnType);
        // Type must be boolean
        if (checkedType.type != 0 || checkedType.primitive != "boolean") {
          throw;
        }

        current->m_generated_tokens[4].checkedType = checkedType;
      }

      // Check if ForUpdate exists 
      if (current->m_generated_tokens.size() == 9) {
        TCType checkedType = typeCheck(&current->m_generated_tokens[6], currentClass, localEnv, returnType);
        current->m_generated_tokens[6].checkedType = checkedType;
      }
      else if (current->m_generated_tokens.size() == 8) {
        TCType checkedType = typeCheck(&current->m_generated_tokens[5], currentClass, localEnv, returnType);
        current->m_generated_tokens[5].checkedType = checkedType;
      }
      else if (current->m_generated_tokens.size() == 7) {
        TCType checkedType = typeCheck(&current->m_generated_tokens[4], currentClass, localEnv, returnType);
        current->m_generated_tokens[4].checkedType = checkedType;
      }

      // Don't care, but storing since need to return something
      TCType checkedType = typeCheck(&current->m_generated_tokens[current->m_generated_tokens.size() - 1], currentClass, localEnv, returnType);

      return checkedType;

    case ReturnStatement:
      if (current->m_generated_tokens[1].m_type != T_SEMICOLON) {
        TCType checkedType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType);
        
        current->m_generated_tokens[2].checkedType = checkedType;

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

      return checkedType;

    case Assignment:
      TCType leftCheckedType = typeCheck(&current->m_generated_tokens[0], currentClass, localEnv, returnType);
      TCType rightCheckedType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType);

      current->m_generated_tokens[0].checkedType = leftCheckedType;
      current->m_generated_tokens[2].checkedType = rightCheckedType;

      // Check for assignability
      if (isAssignable(leftCheckedType, rightCheckedType) == false) {
        throw; 
      }
     
      return leftCheckedType;
    
    case T_IDENTIFIER:
      TCType identifierType;
      
      // First check if declaration pointer is not null
      if (current->declaration != nullptr) {
        identifierType.type = 1;
        identifierType.reference = current->declaration;

        current->checkedType = identifierType;
        return identifierType;
      }

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
      TCType checkedType;
      checkedType.type = 5;

      return checkedType;
  
    case CHAR_LITERAL:
      TCType checkedType;
      checkedType.type = 0;
      checkedType.primitive = "char";

      return checkedType;

    case INT_LITERAL:
      TCType checkedType;
      checkedType.type = 0;
      checkedType.primitive = "int";

      return checkedType;

    case BOOL_LITERAL:
      TCType checkedType;
      checkedType.type = 0;
      checkedType.primitive = "boolean";

      return checkedType;

    case STRING_LITERAL:
      TCType checkedType;
      checkedType.type = 1;
      checkedType.reference = current->declaration;

      return checkedType;

    case T_THIS:
      TCType checkedType;
      checkedType.type = 1;
      checkedType.reference = currentClass;

      return checkedType;

    case ConditionalOrExpression:
    case ConditionalAndExpression:
    case InclusiveOrExpression:
    case AndExpression:
      TCType leftCheckedType = typeCheck(&current->m_generated_tokens[0], currentClass, localEnv, returnType);
      TCType rightCheckedType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType);

      current->m_generated_tokens[0].checkedType = leftCheckedType;
      current->m_generated_tokens[2].checkedType = rightCheckedType;

      // Throw if expressions are not boolean
      if (leftCheckedType.primitive != "boolean" || rightCheckedType.primitive != "boolean") {
        throw;
      }

      TCType checkedType;
      checkedType.type = 0;
      checkedType.primitive = "boolean";

      return checkedType;

    case EqualityExpression:
      TCType leftCheckedType = typeCheck(&current->m_generated_tokens[0], currentClass, localEnv, returnType);
      TCType rightCheckedType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType);

      current->m_generated_tokens[0].checkedType = leftCheckedType;
      current->m_generated_tokens[2].checkedType = rightCheckedType;

      set<string> numericTypes = {"int", "char", "short", "byte"};
      
      if (numericTypes.find(leftCheckedType.primitive) != numericTypes.end() && numericTypes.find(rightCheckedType.primitive) != numericTypes.end()) {
        TCType checkedType;
        checkedType.type = 0;
        checkedType.primitive = "boolean";

        return checkedType;
      }
      // Error if not assignable
      else if ((isAssignable(leftCheckedType, rightCheckedType) == false) && (isAssignable(rightCheckedType, leftCheckedType) == false))  {
        throw;      
      }

      TCType checkedType;
      checkedType.type = 0;
      checkedType.primitive = "boolean";

      return checkedType;

    case RelationalExpression:
      TCType leftCheckedType = typeCheck(&current->m_generated_tokens[0], currentClass, localEnv, returnType);
      TCType rightCheckedType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType);

      current->m_generated_tokens[0].checkedType = leftCheckedType;
      current->m_generated_tokens[2].checkedType = rightCheckedType;

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

      TCType checkedType;
      checkedType.type = 0;
      checkedType.primitive = "boolean";

      return checkedType;

    case AdditiveExpression:
      TCType leftCheckedType = typeCheck(&current->m_generated_tokens[0], currentClass, localEnv, returnType);
      TCType rightCheckedType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType);

      current->m_generated_tokens[0].checkedType = leftCheckedType;
      current->m_generated_tokens[2].checkedType = rightCheckedType;

      if (current->m_generated_tokens[1].m_type == T_PLUS) {
        // If either is String, then Expression type is String
        if ((leftCheckedType.type == 1) && (leftCheckedType.reference->m_generated_tokens[2].m_lex == "String")) {
          TCType checkedType;
          checkedType.type = 1;
          checkedType.reference = leftCheckedType.reference;

          return checkedType;
        }
        else if ((rightCheckedType.type == 1) && (rightCheckedType.reference->m_generated_tokens[2].m_lex == "String")) {
          TCType checkedType;
          checkedType.type = 1;
          checkedType.reference = rightCheckedType.reference;

          return checkedType;
        }
      }

      // Both operands must be numeric
      if (numericTypes.find(leftCheckedType.primitive) == numericTypes.end() || numericTypes.find(rightCheckedType.primitive) == numericTypes.end()) {
        throw;
      }

      TCType checkedType;
      checkedType.type = 0;
      checkedType.primitive = "int";

      return checkedType;

    case MultiplicativeExpression:
      TCType leftCheckedType = typeCheck(&current->m_generated_tokens[0], currentClass, localEnv, returnType);
      TCType rightCheckedType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType);

      current->m_generated_tokens[0].checkedType = leftCheckedType;
      current->m_generated_tokens[2].checkedType = rightCheckedType;

      // Both operands must be numeric
      if (numericTypes.find(leftCheckedType.primitive) == numericTypes.end() || numericTypes.find(rightCheckedType.primitive) == numericTypes.end()) {
        throw;
      }

      TCType checkedType;
      checkedType.type = 0;
      checkedType.primitive = "int";

      return checkedType;

    case CastExpression:
      TCType expressionType = typeCheck(&current->m_generated_tokens[current->m_generated_tokens.size() - 1], currentClass, localEnv, returnType);
      TCType castType = typeCheck(&current->m_generated_tokens[1], currentClass, localEnv, returnType);

      current->m_generated_tokens[current->m_generated_tokens.size() - 1].checkedType = expressionType;
      current->m_generated_tokens[1].checkedType = castType;

      // Check if Casting type is arrayType
      if (current->m_generated_tokens[2].m_type == Dims) {
        if (castType.type == 0) {
          castType.type = 2;
        }
        else if (castType.type == 1) {
          castType.type = 3;
        }
      }
      
      // Must be assignable in atleast 1 direction
      if ((isAssignable(expressionType, castType) == false) && (isAssignable(castType, expressionType) == false))  {
        throw;
      }

      return castType;

    case T_BYTE:
    case T_INT:
    case T_SHORT:
    case T_CHAR:
    case T_BOOLEAN:
      TCType checkedType;
      checkedType.type = 0;
      checkedType.primitive = current->m_lex;

      return checkedType;

    // TODO
    case QualifiedName:
      TCType checkedType;
      checkedType.type = 1;
      checkedType.reference = current->declaration;

      // Should not be nullptr
      if (current->declaration == nullptr) {
        cerr<<"Problem with qualified name";
        throw;
      }

      return checkedType;

    case UnaryExpressionNotPlusMinus:
      TCType checkedType = typeCheck(&current->m_generated_tokens[1], currentClass, localEnv, returnType);

      current->m_generated_tokens[1].checkedType = checkedType;

      // Type must be boolean
      if ((checkedType.type != 0) || checkedType.primitive != "boolean") {
        throw;
      }

      return checkedType;
    
    case UnaryExpression:
      TCType expressionType = typeCheck(&current->m_generated_tokens[1], currentClass, localEnv, returnType);

      current->m_generated_tokens[1].checkedType = expressionType;

      // Must be numeric type
      if (numericTypes.find(expressionType.primitive) == numericTypes.end()) {
        throw;
      }

      TCType checkedType;
      checkedType.type = 0;
      checkedType.primitive = "int";

      return checkedType;

    case ArrayAccess:
      TCType leftCheckedType = typeCheck(&current->m_generated_tokens[0], currentClass, localEnv, returnType);
      TCType expressionType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType);

      current->m_generated_tokens[0].checkedType = leftCheckedType;
      current->m_generated_tokens[2].checkedType = expressionType;

      // Expression must be numeric
      if (numericTypes.find(expressionType.primitive) == numericTypes.end()) {
        throw;
      }

      TCType checkedType = leftCheckedType;

      return checkedType;

    case FieldAccess:
      TCType expressionType = typeCheck(&current->m_generated_tokens[0], currentClass, localEnv, returnType);

      current->m_generated_tokens[0].checkedType = expressionType;

      // Must be reference type
      if (expressionType.type != 1) {
        cerr<<"Problem with Field Access";
        throw;
      }

      // Now Check that field is present in class environment
      string identifierName = current->m_generated_tokens[2].m_lex;

      if (expressionType.reference->scope.fields.find(identifierName) == expressionType.reference->scope.fields.end()) {
        throw;
      }
      // Check field should not be static
      else if (expressionType.reference->scope.fields[identifierName]->m_generated_tokens[0].SearchByTypeBFS(T_STATIC) != nullptr) {
        throw;
      }

      TCType checkedType = getType(expressionType.reference->scope.fields[identifierName]->m_generated_tokens[1]);

      return checkedType;

    case MethodInvocation:
      // TODO
      string argSignature = "";

      // Check if any arguments
      if (current->m_generated_tokens[current->m_generated_tokens.size() - 2].m_type == ArgumentList) {
        for (Token& token: current->m_generated_tokens[current->m_generated_tokens.size() - 2].m_generated_tokens) {
          TCType argumentType = typeCheck(&token, currentClass, localEnv, returnType);

          // Compute signature
          if (argumentType.type == 0) {
            argSignature += argumentType.primitive;
          }
          else if (argumentType.type == 1) {
            const void * address = static_cast<const void*>(argumentType.reference);
            std::stringstream ss;
            ss << address;
            argSignature += ss.str();
          }
          else if (argumentType.type == 2) {
            argSignature += argumentType.primitive;
            argSignature += "[]";
          }
          else if (argumentType.type == 2) {
            const void * address = static_cast<const void*>(argumentType.reference);
            std::stringstream ss;
            ss << address;
            argSignature += ss.str();
            argSignature += "[]";
          }
        }
      }

      Token* methodCalleeClass;
      string methodName;
      bool shouldBeStatic = false;

      // Distinguish between Primary . Identifier v/s Name
      if (current->m_generated_tokens[2].m_type == T_IDENTIFIER) {
        Type checkedType = typeCheck(&current->m_generated_tokens[0], currentClass, localEnv, returnType);
        
        // Error if not reference type
        if (checkedType.type != 1) {
          throw;
        }
        
        methodCalleeClass = checkedType.reference;
        methodName = current->m_generated_tokens[2].m_lex;
      }
      // Method is current class's method
      else if (current->m_generated_tokens[0].m_type == T_IDENTIFIER) {
        methodCalleeClass = currentClass;
        methodName = current->m_generated_tokens[0].m_lex;
      }
      // Qualified Name method call
      else {
      
      }

    case ArrayCreationExpression:
      TCType creationType = typeCheck(&current->m_generated_tokens[1], currentClass, localEnv, returnType);
      TCType checkedType;

      current->m_generated_tokens[1].checkedType = creationType;

      if (creationType.type == 0) {
        checkedType.type = 2;
        checkedType.primitive = creationType.primitive;
      }
      else if (creationType.type == 1) {
        checkedType.type = 3;
        checkedType.reference = creationType.reference;
      }

      return checkedType;

    case ClassInstanceCreationExpression:
      TCType creationType = typeCheck(&current->m_generated_tokens[1], currentClass, localEnv, returnType);

      current->m_generated_tokens[1].checkedType = creationType;

      string argSignature = "";

      // Check if any arguments
      if (current->m_generated_tokens[current->m_generated_tokens.size() - 2].m_type == ArgumentList) {
        for (Token& token: current->m_generated_tokens[current->m_generated_tokens.size() - 2].m_generated_tokens) {
          TCType argumentType = typeCheck(&token, currentClass, localEnv, returnType);
          
          // Compute signature
          if (argumentType.type == 0) {
            argSignature += argumentType.primitive;
          }
          else if (argumentType.type == 1) {
            const void * address = static_cast<const void*>(argumentType.reference);
            std::stringstream ss;
            ss << address;
            argSignature += ss.str();  
          }
          else if (argumentType.type == 2) {
            argSignature += argumentType.primitive;
            argSignature += "[]";
          }
          else if (argumentType.type == 2) {
            const void * address = static_cast<const void*>(argumentType.reference);
            std::stringstream ss;
            ss << address;
            argSignature += ss.str();
            argSignature += "[]";
          }
        }
      }
      
      // Must be reference type
      if (creationType.type != 1) {
        throw;
      }

      string className = creationType.reference->m_generated_tokens[2].m_lex;

      // Check that constructors have same name as class
      for (std::pair<std::string, std::map<std::string,std::vector<Token*>>> constructor: creationType.reference->scope.constructorsWithSignatures) {
        if (constructor.first != className) {
          cerr<<"Constructor has different name than class"<<endl;
          throw;
        }

        bool signatureMatch = false;

        TCType checkedType;

        // Check constructor with signature exists
        for (std::pair<std::string,std::vector<Token*>> signature: constructor.second) {
          if (signature.first == argSignature) {
            signatureMatch = true;
            checkedType.implementation = signature.second;
          }
        }
      }

      // Error if no constructor matched
      if (signatureMatch == false) {
        cerr<<"Constructor signature did not match"<<endl;
        throw;
      }

      string emptySignature = "";

      // Check that superClass has 0 argument constructor
      for (std::pair<std::string, std::map<std::string,std::vector<Token*>>> constructor: creationType.reference->super_class->scope.constructorsWithSignatures) {
        if (constructor.second.find(emptySignature) == constructor.second.end()) {
          cerr<<"Zero argument superclass constructor does not exist"<<endl;
          throw;
        }
      }

      checkedType.type = 1;
      checkedType.reference = creationType.reference;

      return checkedType;

    case PrimaryNoNewArray:
      TCType expressionType = typeCheck(&current->m_generated_tokens[1], currentClass, localEnv, returnType);

      current->m_generated_tokens[1].checkedType = expressionType;

      return expressionType;

    default:
      TCType checkedType;
      checkedType.type = -1;

      return checkedType;
  }
}

bool isAssignable(TCType lhs, TCType rhs) {
  
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
  if (subType->super_class != nullptr) {
    isRecursiveSubType = isSubtype(superType, subType->super_class);
  }

  for (Token* token: subType->super_interfaces) {
    isRecursiveSubType = isRecursiveSubType || isSubtype(superType, token);
  }

  return isRecursiveSubType;
}

TCType getType(Token* current) {
  TCType nodeType;

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
  tokens.push(astRoot);
  while (tokens.size() > 0) {
    current = tokens.pop();
    if (current->m_type == MethodDeclaration) {
      Token* currentClass = current->compilation_unit->searchByTypeDFS(ClassDeclaration);
      TCType returnType;

      returnType = getType(&current->m_generated_tokens[0].m_generated_tokens[1]);

      current->checkedTyped = returnType;
      if (current->m_generated_tokens[1].m_type == Block && current->m_generated_tokens[1].m_generated_tokens[1] == BlockStatements) {
        TCType checkedType = typeCheck(current->m_generated_tokens[1].m_generated_tokens[1], currentClass, current->scope, returnType);    
      }
    }
    for (Token& token: current->m_generated_tokens) {
      tokens.push(&token);
    }
  }
}
