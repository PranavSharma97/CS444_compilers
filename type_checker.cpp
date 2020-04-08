#include "type_checker.h"
#include <queue>
#include <set>
#include <iostream>
#include <sstream>

using namespace std;

TCType typeCheck(Token* current, Token* currentClass, environment localEnv, TCType returnType) {
  
  set<string> numericTypes = {"int", "char", "short", "byte"};

  switch (current->m_type) {
    case BlockStatements: {
      cerr<<"Inside BlockStatements "<<endl;
      TCType checkedType;
      for (Token& token: current->m_generated_tokens) {
        // Don't care about return value since it's a statement
        checkedType = typeCheck(&token, currentClass, localEnv, returnType);
      }

      return checkedType;
    }

    case Block: {
      cerr<<"Inside BLOCK"<<endl;
      if (current->m_generated_tokens[1].m_type == BlockStatements) {
        return typeCheck(&current->m_generated_tokens[1], currentClass, localEnv, returnType);
      }

      TCType checkedType;
      checkedType.type = -1;

      return checkedType;
    }

    case LocalVariableDeclarationStatement: {
      cerr<<"Inside LocalVariableDeclarationStatement"<<endl;
      return typeCheck(&current->m_generated_tokens[0], currentClass, localEnv, returnType);
    }

    case LocalVariableDeclaration: {
      cerr<<"Inside LocalVariableDeclaration"<<endl;
      TCType declaredType = getType(&current->m_generated_tokens[0]);
      TCType initializerType = typeCheck(&current->m_generated_tokens[1].m_generated_tokens[2], currentClass, localEnv, returnType);

      current->m_generated_tokens[1].m_generated_tokens[2].checkedType = initializerType;

      cerr<<"Declared Type: "<<declaredType.type<<" "<<declaredType.primitive<<endl;
      cerr<<"Initializer Type: "<<initializerType.type<<" "<<initializerType.primitive<<endl;
      for (Token& t: current->m_generated_tokens) {
        cerr<<t<<endl;
      }

      if (isAssignable(declaredType, initializerType) == false) {
        throw std::logic_error("Type checking error: LocalVariableDeclaration not assignable");
      }
     
      return declaredType;
    }

    case IfThenStatement: {
      cerr<<"Inside ifthenstatement"<<endl;
      TCType checkedType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType);
      // Type must be boolean
      if (checkedType.type != 0 || checkedType.primitive != "boolean") {
        throw std::logic_error("Type checking error: IfThenStatement");
      }

      current->m_generated_tokens[2].checkedType = checkedType;
  
      return checkedType;
    }

    case IfThenElseStatement:
    case IfThenElseStatementNoShortIf: {
      cerr<<"Inside ifthenelse"<<endl;
      TCType checkedType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType);
      // Type must be boolean
      if (checkedType.type != 0 || checkedType.primitive != "boolean") {
        throw std::logic_error("Type checking error: IfThenElseStatement");
      }

      current->m_generated_tokens[2].checkedType = checkedType;
      // Don't care about return value since it's a statement
      typeCheck(&current->m_generated_tokens[4], currentClass, localEnv, returnType);
      typeCheck(&current->m_generated_tokens[6], currentClass, localEnv, returnType);

      return checkedType;
    }

    case WhileStatement:
    case WhileStatementNoShortIf: {
      cerr<<"Inside while"<<endl;
      TCType checkedType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType);
      // Type must be boolean
      if (checkedType.type != 0 || checkedType.primitive != "boolean") {
        throw std::logic_error("Type checking error: WhileStatement");
      }
      
      current->m_generated_tokens[2].checkedType = checkedType;
      // Don't care about return value since it's a statement 
      typeCheck(&current->m_generated_tokens[4], currentClass, localEnv, returnType);

      return checkedType;
    }

    case ForStatement:
    case ForStatementNoShortIf: {
      cerr<<"Inside for statement"<<endl;
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
          throw std::logic_error("Type checking error: ForStatement stopping condition");
        }

        current->m_generated_tokens[3].checkedType = checkedType;
      }
      else if (current->m_generated_tokens[4].m_type != T_SEMICOLON) {
        TCType checkedType = typeCheck(&current->m_generated_tokens[4], currentClass, localEnv, returnType);
        // Type must be boolean
        if (checkedType.type != 0 || checkedType.primitive != "boolean") {
          throw std::logic_error("Type checking error: ForStatement stopping condition");
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
    }

    case ReturnStatement: {
      cerr<<"Inside return statement"<<endl;
      cerr<<"Class is: "<<currentClass->m_generated_tokens[2].m_lex<<endl;
      TCType checkedType;
      if (current->m_generated_tokens[1].m_type != T_SEMICOLON) {
        checkedType = typeCheck(&current->m_generated_tokens[1], currentClass, localEnv, returnType);
        
        current->m_generated_tokens[1].checkedType = checkedType;

        if (isAssignable(returnType, checkedType) == false) {
          throw std::logic_error("Type checking error: ReturnStatement not assignable");
        } 

        /*
        // Check if Type doesn't match with return type
        if (returnType.type != checkedType.type) {
          cerr<<"Types: "<<returnType.type<<" "<<checkedType.type<<endl;
          throw std::logic_error("Type checking error: ReturnStatement types don't match");
        }
        // Check if return type is primitive or primitive array and don't match
        else if ((returnType.type == 0 || returnType.type == 2) && (returnType.primitive != checkedType.primitive)) {
          throw std::logic_error("Type checking error: ReturnStatement primitive mismatch"); 
        }
        // Check if return type is reference or reference array and don't match
        else if ((returnType.type == 1 || returnType.type == 3) && (returnType.reference != checkedType.reference)) {
          cerr<<"Return type is: "<<returnType.reference<<endl;
          cerr<<"Checked type is: "<<checkedType.reference<<endl;
          throw std::logic_error("Type checking error: ReturnStatement reference mismatch");
        }
        */
      }
      else if (returnType.type != 4) {
        // Throw error since method return type is not VOID but return statement is blank
        throw std::logic_error("Type checking error: ReturnStatement is void, return type not void");
      }

      return returnType;
    }

    case ExpressionStatement:
      cerr<<"Inside expressionStatement"<<endl;
      return typeCheck(&current->m_generated_tokens[0], currentClass, localEnv, returnType);

    case Assignment: {
      cerr<<"Inside assignment"<<endl;
      TCType leftCheckedType = typeCheck(&current->m_generated_tokens[0], currentClass, localEnv, returnType);
      TCType rightCheckedType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType);

      current->m_generated_tokens[0].checkedType = leftCheckedType;
      current->m_generated_tokens[2].checkedType = rightCheckedType;

      // Check for assignability
      if (isAssignable(leftCheckedType, rightCheckedType) == false) {
        throw std::logic_error("Type checking error: Assignment"); 
      }
     
      current->checkedType = leftCheckedType;

      return leftCheckedType;
    }

    case T_IDENTIFIER: {
      cerr<<"Inside T_identifier"<<endl;
      TCType identifierType;
    
      if (current->declaration == nullptr) {
        cerr<<"\n\n\n";
        cerr<<current->m_lex;
        cerr<<endl<<endl<<endl;
      }

      // First check if declaration pointer is not null
      if (current->declaration->m_type == ClassDeclaration || current->declaration->m_type == InterfaceDeclaration) {
        identifierType.type = 1;
        identifierType.reference = current->declaration;

        current->checkedType = identifierType;
        return identifierType;
      }

      // Check if it's a local variable or formal parameter
      if (current->declaration->m_type == LocalVariableDeclaration || current->declaration->m_type == FormalParameter) {
        identifierType = getType(&current->declaration->m_generated_tokens[0]);
      }
      // Check if it's a field
      else if (current->declaration->m_type == FieldDeclaration) {
        identifierType = getType(&current->declaration->m_generated_tokens[1]);
      }

      current->checkedType = identifierType;

      return identifierType;
    }

    case NULL_LITERAL: {
      cerr<<"Inside NULL"<<endl;
      TCType checkedType;
      checkedType.type = 5;

      return checkedType;
    }

    case CHAR_LITERAL: {
      cerr<<"Inside CHAR literal"<<endl;
      TCType checkedType;
      checkedType.type = 0;
      checkedType.primitive = "char";

      return checkedType;
    }

    case INT_LITERAL: {
      cerr<<"Inside INT literal"<<endl;
      TCType checkedType;
      checkedType.type = 0;
      checkedType.primitive = "int";

      return checkedType;
    }

    case BOOL_LITERAL: {
      cerr<<"Inside BOOL literal"<<endl;
      TCType checkedType;
      checkedType.type = 0;
      checkedType.primitive = "boolean";

      return checkedType;
    }

    case STRING_LITERAL: {
      cerr<<"Inside STRING literal"<<endl;
      cerr<<current->declaration<<endl;
      TCType checkedType;
      checkedType.type = 1;
      checkedType.reference = current->declaration;

      return checkedType;
    }

    case T_THIS: {
      cerr<<"Inside T_THIS"<<endl;
      TCType checkedType;
      checkedType.type = 1;
      checkedType.reference = currentClass;

      return checkedType;
    }

    case ConditionalOrExpression:
    case ConditionalAndExpression:
    case InclusiveOrExpression:
    case AndExpression: {
      cerr<<"Inside ConditionalOrExpression"<<endl; 
      TCType leftCheckedType = typeCheck(&current->m_generated_tokens[0], currentClass, localEnv, returnType);
      TCType rightCheckedType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType);

      current->m_generated_tokens[0].checkedType = leftCheckedType;
      current->m_generated_tokens[2].checkedType = rightCheckedType;

      // Throw if expressions are not boolean
      if (leftCheckedType.primitive != "boolean" || rightCheckedType.primitive != "boolean") {
        throw std::logic_error("Type checking error: ConditionalOrExpression");
      }

      TCType checkedType;
      checkedType.type = 0;
      checkedType.primitive = "boolean";

      return checkedType;
    }

    case EqualityExpression: {
      cerr<<"Inside Equality expression"<<endl; 
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
        throw std::logic_error("Type checking error: EqualityExpression");
      }

      TCType checkedType;
      checkedType.type = 0;
      checkedType.primitive = "boolean";

      return checkedType;
    }

    case RelationalExpression: {
      cerr<<"Inside Relational Expression"<<endl; 
      TCType leftCheckedType = typeCheck(&current->m_generated_tokens[0], currentClass, localEnv, returnType);
      TCType rightCheckedType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType);

      current->m_generated_tokens[0].checkedType = leftCheckedType;
      current->m_generated_tokens[2].checkedType = rightCheckedType;

      set<string> numericTypes = {"int", "char", "short", "byte"};

      if (current->m_generated_tokens[1].m_type == T_INSTANCE_OF) {
        if ((isAssignable(leftCheckedType, rightCheckedType) == false) && (isAssignable(rightCheckedType, leftCheckedType) == false))  {
          throw std::logic_error("Type checking error: RelationalExpression instanceof");
        }
      }
      // Error if left and right expressions are not numericTypes
      else if (numericTypes.find(leftCheckedType.primitive) == numericTypes.end() || numericTypes.find(rightCheckedType.primitive) == numericTypes.end()) {
        throw std::logic_error("Type checking error: RelationalExpression not numeric"); 
      }

      TCType checkedType;
      checkedType.type = 0;
      checkedType.primitive = "boolean";

      return checkedType;
    }

    case AdditiveExpression: {
      cerr<<"Inside Additive Expression"<<endl; 
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
        throw std::logic_error("Type checking error: AdditiveExpression not numeric");
      }

      TCType checkedType;
      checkedType.type = 0;
      checkedType.primitive = "int";

      return checkedType;
    }

    case MultiplicativeExpression: {
      cerr<<"Inside Multiplicate Expression"<<endl; 
      TCType leftCheckedType = typeCheck(&current->m_generated_tokens[0], currentClass, localEnv, returnType);
      TCType rightCheckedType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType);

      current->m_generated_tokens[0].checkedType = leftCheckedType;
      current->m_generated_tokens[2].checkedType = rightCheckedType;

      // Both operands must be numeric
      if (numericTypes.find(leftCheckedType.primitive) == numericTypes.end() || numericTypes.find(rightCheckedType.primitive) == numericTypes.end()) {
        throw std::logic_error("Type checking error: MultiplicativeExpression not numeric");
      }

      TCType checkedType;
      checkedType.type = 0;
      checkedType.primitive = "int";

      return checkedType;
    }

    case CastExpression: {
      cerr<<"Inside Cast Expression"<<endl;
      if (current->m_generated_tokens[current->m_generated_tokens.size() - 1].m_type == T_IDENTIFIER) {
        cerr<<endl<<current->m_generated_tokens[current->m_generated_tokens.size() - 1].m_lex<<" "<<current->m_generated_tokens[current->m_generated_tokens.size() - 1].declaration<<endl;
        cerr<<"Class name: "<<currentClass->m_generated_tokens[2].m_lex<<endl;
      }
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
        throw std::logic_error("Type checking error: CastExpression not assignable");
      }

      return castType;
    }

    case T_BYTE:
    case T_INT:
    case T_SHORT:
    case T_CHAR:
    case T_BOOLEAN: {
      cerr<<"Inside T_BYTE or T_INT or ..."<<endl; 
      TCType checkedType;
      checkedType.type = 0;
      checkedType.primitive = current->m_lex;

      return checkedType;
    }

    case QualifiedName: {
      cerr<<"Inside QualifiedName"<<endl; 
      TCType checkedType;

      // Should not be nullptr
      if (current->declaration == nullptr) {
        cerr<<"Problem with qualified name";
        throw std::logic_error("Type checking error: QualifiedName declaration nullptr");
      }

      // Check if QualifiedName is array.length
      if (current->declaration->m_type == T_INT && current->declaration->m_lex == "length") {
        checkedType.type = 0;
        checkedType.primitive = "int";
      }
      // Check if it's a local variable
      else if (current->declaration->m_type == LocalVariableDeclaration) {
        checkedType = getType(&current->declaration->m_generated_tokens[0]);
      }
      // Check if it's a field
      else if (current->declaration->m_type == FieldDeclaration) {
        checkedType = getType(&current->declaration->m_generated_tokens[1]);
      }
      // Implies it's a Type
      else {
        checkedType.type = 1;
        checkedType.reference = current->declaration;
      }

      current->checkedType = checkedType;

      return checkedType;
    }

    case UnaryExpressionNotPlusMinus: {
      cerr<<"Inside Unary Expression Not plus minus"<<endl;
      TCType checkedType = typeCheck(&current->m_generated_tokens[1], currentClass, localEnv, returnType);

      current->m_generated_tokens[1].checkedType = checkedType;

      // Type must be boolean
      if ((checkedType.type != 0) || checkedType.primitive != "boolean") {
        throw std::logic_error("Type checking error: UnaryExpressionNotPlusMinus not boolean");
      }

      return checkedType;
    }

    case UnaryExpression: {
      cerr<<"Inside Unary Expression"<<endl;
      TCType expressionType = typeCheck(&current->m_generated_tokens[1], currentClass, localEnv, returnType);

      current->m_generated_tokens[1].checkedType = expressionType;

      // Must be numeric type
      if (numericTypes.find(expressionType.primitive) == numericTypes.end()) {
        throw std::logic_error("Type checking error: UnaryExpression not numeric");
      }

      TCType checkedType;
      checkedType.type = 0;
      checkedType.primitive = "int";

      return checkedType;
    }

    case ArrayAccess: {
      cerr<<"Inside Array Access"<<endl;
      TCType leftCheckedType = typeCheck(&current->m_generated_tokens[0], currentClass, localEnv, returnType);
      TCType expressionType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType);

      current->m_generated_tokens[0].checkedType = leftCheckedType;
      current->m_generated_tokens[2].checkedType = expressionType;

      // Expression must be numeric
      if (numericTypes.find(expressionType.primitive) == numericTypes.end()) {
        throw std::logic_error("Type checking error: ArrayAccess expression not numeric");
      }

      // Type must be array type
      if ((leftCheckedType.type != 2) && (leftCheckedType.type != 3)) {
        throw std::logic_error("Type checking error: ArrayAccess not array type");
      }

      TCType checkedType;

      if (leftCheckedType.type == 2) {
        checkedType.type = 0;
        checkedType.primitive = leftCheckedType.primitive;
      }
      else if (leftCheckedType.type == 3) {
        checkedType.type = 1;
        checkedType.reference = leftCheckedType.reference;
      }

      return checkedType;
    }

    case FieldAccess: {
      cerr<<"Inside Field access"<<endl;
      TCType expressionType = typeCheck(&current->m_generated_tokens[0], currentClass, localEnv, returnType);

      current->m_generated_tokens[0].checkedType = expressionType;

      // Must be reference type
      if (expressionType.type != 1) {
        cerr<<"Problem with Field Access";
        throw std::logic_error("Type checking error: FieldAccess expression type is primitive");
      }

      // Now Check that field is present in class environment
      string identifierName = current->m_generated_tokens[2].m_lex;

      if (expressionType.reference->scope.fields.find(identifierName) == expressionType.reference->scope.fields.end()) {
        throw std::logic_error("Type checking error: FieldAccess cannot find field");
      }
      // Check field should not be static
      else if (expressionType.reference->scope.fields[identifierName]->m_generated_tokens[0].SearchByTypeBFS(T_STATIC) != nullptr) {
        throw std::logic_error("Type checking error: FieldAccess field should not be static");
      }

      TCType checkedType = getType(&expressionType.reference->scope.fields[identifierName]->m_generated_tokens[1]);

      return checkedType;
    }

    case MethodInvocation: {
      cerr<<"Inside Method Invocation"<<endl;
      string argSignature = "";

      // Check if any arguments
      if (current->m_generated_tokens[current->m_generated_tokens.size() - 2].m_type == ArgumentList) {
        for (Token& token: current->m_generated_tokens[current->m_generated_tokens.size() - 2].m_generated_tokens) {
          
          //cerr<<"Type of token IS: "<<token.m_type<<endl;
          
          TCType argumentType = typeCheck(&token, currentClass, localEnv, returnType);

          //cerr<<"Type of argument is: "<<argumentType.type<<endl;

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

      cerr<<"signature created..."<<endl;

      cerr<<"Declaration pointer is: ";
      cerr<<current->m_generated_tokens[0].declaration<<endl;
      cerr<<"Type of first child: "<<current->m_generated_tokens[0].m_type<<endl;
      if (current->m_generated_tokens[0].m_type == QualifiedName) {
        cerr<<"Number of children: "<<current->m_generated_tokens[0].m_generated_tokens.size()<<endl;
        for (Token& t: current->m_generated_tokens[0].m_generated_tokens) {
          cerr<<t.m_lex<<endl;
        }
        cerr<<"Name of class: "<<currentClass->m_generated_tokens[2].m_lex<<endl;
      }

      //cerr<<"Type is: "<<current->m_generated_tokens[0].declaration->m_type;
      
      // Distinguish between Primary . Identifier v/s Name
      if (current->m_generated_tokens[1].m_type == T_DOT) {
        cerr<<"Primary . identifier"<<endl;
        TCType checkedType = typeCheck(&current->m_generated_tokens[0], currentClass, localEnv, returnType);
    
        current->m_generated_tokens[0].checkedType = checkedType;

        // Error if not reference type
        if (checkedType.type != 1) {
          throw std::logic_error("Type checking error: MethodInvocation on primitive type");
        }
        
        methodCalleeClass = checkedType.reference;
        methodName = current->m_generated_tokens[2].m_lex;
      }
      // Method is current class's method
      else if (current->m_generated_tokens[0].m_type == T_IDENTIFIER) {
        cerr<<"Identifier..."<<endl;
        methodCalleeClass = currentClass;
        methodName = current->m_generated_tokens[0].m_lex;
      }
      // Qualified Name Static method call
      else if (current->m_generated_tokens[0].declaration->m_type == ClassDeclaration) {
        cerr<<"Static method call"<<endl;
        shouldBeStatic = true;
        methodCalleeClass = current->m_generated_tokens[0].declaration;
        methodName = current->m_generated_tokens[0].m_generated_tokens[current->m_generated_tokens[0].m_generated_tokens.size() - 1].m_lex;
      }
      // Qualified Name method call on variable
      else if (current->m_generated_tokens[0].declaration->m_type == LocalVariableDeclaration || current->m_generated_tokens[0].declaration->m_type == FormalParameter) {
        cerr<<"Variable call"<<endl;
        TCType variableType = getType(&current->m_generated_tokens[0].declaration->m_generated_tokens[0]);
        
        // Error if not reference type
        if (variableType.type != 1) {
          throw std::logic_error("Type checking error: MethodInvocation on variable of primitive type");
        }
        
        methodCalleeClass = variableType.reference;
        methodName = current->m_generated_tokens[0].m_generated_tokens[current->m_generated_tokens[0].m_generated_tokens.size() - 1].m_lex;
      }
      // Qualified Name method call on field
      else if (current->m_generated_tokens[0].declaration->m_type == FieldDeclaration) {
        cerr<<"Field call"<<endl;
        TCType fieldType = getType(&current->m_generated_tokens[0].declaration->m_generated_tokens[1]);
        
        // Error if not reference type
        if (fieldType.type != 1) {
          throw std::logic_error("Type checking error: MethodInvocation on field of primitive type");
        }
        
        methodCalleeClass = fieldType.reference;
        methodName = current->m_generated_tokens[0].m_generated_tokens[current->m_generated_tokens[0].m_generated_tokens.size() - 1].m_lex; 
      }

      // Now search for methodName in methodCalleeClass
      bool foundMethodMatch = false;
      TCType checkedType;

      cerr<<"Now searching for method: "<<methodName<<endl;
      cerr<<"In class: "<<methodCalleeClass->m_generated_tokens[2].m_lex<<endl; 

      for (std::pair<std::string, std::map<std::string,std::vector<Token*>>> method: methodCalleeClass->scope.methodsWithSignatures) {
        cerr<<"Method in environment: "<<method.first<<endl;
        // Check method with signature exists
        for (std::pair<std::string,std::vector<Token*>> signature: method.second) {
          cerr<<"Method signature: "<<signature.first<<endl;
          cerr<<"Arg signature: "<<argSignature<<endl;
          // Found match
          if (methodName == method.first && argSignature == signature.first) {
            foundMethodMatch = true;
            checkedType = getType(&signature.second[0]->m_generated_tokens[0].m_generated_tokens[1]);
            checkedType.implementation = signature.second[0];

            // Check staticness
            if (shouldBeStatic == true && signature.second[0]->m_generated_tokens[0].m_generated_tokens[0].SearchByTypeBFS(T_STATIC) == nullptr) {
              throw std::logic_error("Type checking error: MethodInvocation method should be static");
            }
            else if (shouldBeStatic == false && signature.second[0]->m_generated_tokens[0].m_generated_tokens[0].SearchByTypeBFS(T_STATIC) != nullptr) {
              throw std::logic_error("Type checking error: MethodInvocation method should not be static");
            }
          }
        }
      }

      // Error if no method found
      if (foundMethodMatch == false) {
        throw std::logic_error("Type checking error: MethodInvocation method not found");
      }

      current->checkedType = checkedType;

      return checkedType; 
    }

    case ArrayCreationExpression: {
      cerr<<"Inside Array creation Expression"<<endl;
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

      current->checkedType = checkedType;

      return checkedType;
    }

    case ClassInstanceCreationExpression: {
      cerr<<"Inside Class instance creation Expression"<<endl;
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
        throw std::logic_error("Type checking error: ClassInstanceCreationExpression of primitive type");
      }

      string className = creationType.reference->m_generated_tokens[2].m_lex;
      bool signatureMatch = false;
      TCType checkedType;

      // Check that constructors have same name as class
      for (std::pair<std::string, std::map<std::string,std::vector<Token*>>> constructor: creationType.reference->scope.constructorsWithSignatures) {
        if (constructor.first != className) {
          cerr<<"Constructor has different name than class"<<endl;
          throw std::logic_error("Type checking error: ClassInstanceCreationExpression constructor has different name");
        }

        // Check constructor with signature exists
        for (std::pair<std::string,std::vector<Token*>> signature: constructor.second) {
          if (signature.first == argSignature) {
            signatureMatch = true;
            checkedType.implementation = signature.second[0];
          }
        }
      }

      // Error if no constructor matched
      if (signatureMatch == false) {
        cerr<<"Constructor signature did not match"<<endl;
        throw std::logic_error("Type checking error: ClassInstanceCreationExpression signature did not match");
      }

      string emptySignature = "";

      // Check that superClass has 0 argument constructor
      for (std::pair<std::string, std::map<std::string,std::vector<Token*>>> constructor: creationType.reference->super_class->scope.constructorsWithSignatures) {
        if (constructor.second.find(emptySignature) == constructor.second.end()) {
          throw std::logic_error("Type checking error: ClassInstanceCreationExpression Zero argument superclass constructor does not exist");
        }
      }

      checkedType.type = 1;
      checkedType.reference = creationType.reference;

      current->checkedType = checkedType;

      return checkedType;
    }

    case PrimaryNoNewArray: {
      cerr<<"Inside Primary no new array"<<endl;
      TCType expressionType = typeCheck(&current->m_generated_tokens[1], currentClass, localEnv, returnType);

      current->m_generated_tokens[1].checkedType = expressionType;

      return expressionType;
    }

    default: {
      TCType checkedType;
      checkedType.type = -1;

      cerr<<"Landed in default case: Type is "<<current->m_type<<endl;

      return checkedType;
    }
  }
}

bool isAssignable(TCType lhs, TCType rhs) {
  
  // First check for Array Assignability rules
  if ((rhs.type == 2) || (rhs.type == 3)) {
    // Arrays can be assigned to Object, Cloneable, or Serializable
    if (lhs.type == 1) {
      set<string> valid = {"Object", "Serializable", "Cloneable"};
      if (valid.find(lhs.reference->m_generated_tokens[2].m_lex) == valid.end()) {
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
      cerr<<"Primitive array mismatch problem"<<endl;
      return false;
    }
    // Check assignability if type is reference array
    else if ((rhs.type == 3) && (isSubtype(lhs.reference, rhs.reference) == false)) {
      cerr<<"Is subtype problem"<<endl;
      return false;
    }
  }
  // If LeftHandSide is array, but RightHandSide is not, ERROR
  else if ((lhs.type == 2) || (lhs.type == 3)) {
    return false;
  }
  
  // Now check for non-array assignability rules
  if ((rhs.type == 0) && (rhs.primitive == "byte")) {
    // Byte can only be assigned to byte or short or int
    if ((lhs.type != 0) || ((lhs.primitive != "byte") && (lhs.primitive != "short") && (lhs.primitive != "int"))) {
      return false;
    }
  }
  else if ((rhs.type == 0) && (rhs.primitive == "char")) {
    // Char can only be assigned to char or int
    if ((lhs.type != 0) || ((lhs.primitive != "char") && (lhs.primitive != "int"))) {
      return false;
    }
  }
  else if ((rhs.type == 0) && (rhs.primitive == "short")) {
    // Short can only be assigned to short or int
    if ((lhs.type != 0) || ((lhs.primitive != "short") && (lhs.primitive != "int"))) {
      return false;
    }
  }
  else if (rhs.type == 0) {
    // Any other primitive type can only be assigned to its same primitive type
    if ((lhs.type != 0) || (lhs.primitive != rhs.primitive)) {
      return false;
    }
  }
  else if (rhs.type == 1) {
    // Cannot assign reference to primitive
    if (lhs.type != 1) {
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
    current = tokens.front();
    tokens.pop();
    if (current->m_type == MethodDeclaration) {
      Token* currentClass = current->compilation_unit->SearchByTypeDFS(ClassDeclaration);
      TCType returnType;

      returnType = getType(&current->m_generated_tokens[0].m_generated_tokens[1]);

      cerr<<"Method name: "<<current->m_generated_tokens[0].m_generated_tokens[2].m_generated_tokens[0].m_lex<<endl;
      cerr<<"Method return type: "<<returnType.type<<" ";
      if (current->m_generated_tokens[0].m_generated_tokens[2].m_generated_tokens[2].m_type == FormalParameterList) {
        cerr<<"Method signature: "<<getType(&current->m_generated_tokens[0].m_generated_tokens[2].m_generated_tokens[2].m_generated_tokens[0].m_generated_tokens[0]).primitive<<endl;
      }
      if (returnType.type == 0) {
        cerr<<returnType.primitive<<endl;
      }
      else if (returnType.type == 1) {
        cerr<<returnType.reference->m_generated_tokens[2].m_lex<<endl;
      }

      cerr<<"MethodDeclaration found"<<endl;

      current->checkedType = returnType;
      if (current->m_generated_tokens[1].m_type == Block && current->m_generated_tokens[1].m_generated_tokens[1].m_type == BlockStatements) {
        cerr<<"Calling typeCheck"<<endl;
        TCType checkedType = typeCheck(&current->m_generated_tokens[1].m_generated_tokens[1], currentClass, current->scope, returnType);    
      }
    }
    for (Token& token: current->m_generated_tokens) {
      tokens.push(&token);
    }
  }
}
