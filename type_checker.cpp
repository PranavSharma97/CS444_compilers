#include "type_checker.h"
#include <queue>
#include <set>
#include <iostream>
#include <sstream>

using namespace std;

TCType typeCheck(Token* current, Token* currentClass, environment localEnv, TCType returnType, bool isStatic) {
  
  set<string> numericTypes = {"int", "char", "short", "byte"};

  switch (current->m_type) {
    case BlockStatements: {
      //cerr<<"Inside BlockStatements "<<endl;
      TCType checkedType;
      for (Token& token: current->m_generated_tokens) {
        // Don't care about return value since it's a statement
        checkedType = typeCheck(&token, currentClass, localEnv, returnType, isStatic);
      }

      current->checkedType = checkedType;
      return checkedType;
    }

    case Block: {
      //cerr<<"Inside BLOCK"<<endl;
      if (current->m_generated_tokens[1].m_type == BlockStatements) {
        return typeCheck(&current->m_generated_tokens[1], currentClass, localEnv, returnType, isStatic);
      }

      TCType checkedType;
      checkedType.type = -1;

      current->checkedType = checkedType;
			return checkedType;
    }

    case LocalVariableDeclarationStatement: {
      //cerr<<"Inside LocalVariableDeclarationStatement"<<endl;
      return typeCheck(&current->m_generated_tokens[0], currentClass, localEnv, returnType, isStatic);
    }

    case LocalVariableDeclaration: {
      //cerr<<"Inside LocalVariableDeclaration"<<endl;
      TCType declaredType = getType(&current->m_generated_tokens[0]);
      TCType initializerType = typeCheck(&current->m_generated_tokens[1].m_generated_tokens[2], currentClass, localEnv, returnType, isStatic);

      ////cerr<<"Declared Type: "<<declaredType.type<<" "<<declaredType.primitive<<endl;
      ////cerr<<"Initializer Type: "<<initializerType.type<<" "<<initializerType.primitive<<endl;
      for (Token& t: current->m_generated_tokens) {
        //cerr<<t<<endl;
      }

      if (isAssignable(declaredType, initializerType) == false) {
        throw std::logic_error("Type checking error: LocalVariableDeclaration not assignable");
      }
      
      current->checkedType = declaredType;
      return declaredType;
    }

    case IfThenStatement: {
      //cerr<<"Inside ifthenstatement"<<endl;
      TCType checkedType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType, isStatic);
      // Type must be boolean
      if (checkedType.type != 0 || checkedType.primitive != "boolean") {
        throw std::logic_error("Type checking error: IfThenStatement");
      }

      current->checkedType = checkedType;
			return checkedType;
    }

    case IfThenElseStatement:
    case IfThenElseStatementNoShortIf: {
      //cerr<<"Inside ifthenelse"<<endl;
      TCType checkedType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType, isStatic);
      // Type must be boolean
      if (checkedType.type != 0 || checkedType.primitive != "boolean") {
        throw std::logic_error("Type checking error: IfThenElseStatement");
      }

      // Don't care about return value since it's a statement
      TCType ifType = typeCheck(&current->m_generated_tokens[4], currentClass, localEnv, returnType, isStatic);
      TCType elseType = typeCheck(&current->m_generated_tokens[6], currentClass, localEnv, returnType, isStatic);

      current->checkedType = checkedType;
			return checkedType;
    }

    case WhileStatement:
    case WhileStatementNoShortIf: {
      //cerr<<"Inside while"<<endl;
      TCType checkedType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType, isStatic);
      // Type must be boolean
      if (checkedType.type != 0 || checkedType.primitive != "boolean") {
        throw std::logic_error("Type checking error: WhileStatement");
      }
      
      // Don't care about return value since it's a statement 
      TCType statementType = typeCheck(&current->m_generated_tokens[4], currentClass, localEnv, returnType, isStatic);

      current->checkedType = checkedType;
			return checkedType;
    }

    case ForStatement:
    case ForStatementNoShortIf: {
      //cerr<<"Inside for statement"<<endl;
      // Check if ForInit exists
      if (current->m_generated_tokens[2].m_type != T_SEMICOLON) {
        TCType checkedType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType, isStatic);
      }

      // Check if Stopping condition exists
      if (current->m_generated_tokens[3].m_type != T_SEMICOLON) {
        TCType checkedType = typeCheck(&current->m_generated_tokens[3], currentClass, localEnv, returnType, isStatic);
        // Type must be boolean
        if (checkedType.type != 0 || checkedType.primitive != "boolean") {
          throw std::logic_error("Type checking error: ForStatement stopping condition");
        }
      }
      else if (current->m_generated_tokens[4].m_type != T_SEMICOLON) {
        TCType checkedType = typeCheck(&current->m_generated_tokens[4], currentClass, localEnv, returnType, isStatic);
        // Type must be boolean
        if (checkedType.type != 0 || checkedType.primitive != "boolean") {
          throw std::logic_error("Type checking error: ForStatement stopping condition");
        }
      }

      // Check if ForUpdate exists 
      if (current->m_generated_tokens.size() == 9) {
        TCType checkedType = typeCheck(&current->m_generated_tokens[6], currentClass, localEnv, returnType, isStatic);
      }
      else if (current->m_generated_tokens.size() == 8) {
        TCType checkedType = typeCheck(&current->m_generated_tokens[5], currentClass, localEnv, returnType, isStatic);
      }
      else if (current->m_generated_tokens.size() == 7) {
        TCType checkedType = typeCheck(&current->m_generated_tokens[4], currentClass, localEnv, returnType, isStatic);
      }

      // Don't care, but storing since need to return something
      TCType checkedType = typeCheck(&current->m_generated_tokens[current->m_generated_tokens.size() - 1], currentClass, localEnv, returnType, isStatic);

      current->checkedType = checkedType;
			return checkedType;
    }

    case ReturnStatement: {
      //cerr<<"Inside return statement"<<endl;
      ////cerr<<"Class is: "<<currentClass->m_generated_tokens[2].m_lex<<endl;
      TCType checkedType;
      if (current->m_generated_tokens[1].m_type != T_SEMICOLON) {
        checkedType = typeCheck(&current->m_generated_tokens[1], currentClass, localEnv, returnType, isStatic);

        if (isAssignable(returnType, checkedType) == false) {
          throw std::logic_error("Type checking error: ReturnStatement not assignable");
        } 
      }
      else if (returnType.type != 4) {
        // Throw error since method return type is not VOID but return statement is blank
        throw std::logic_error("Type checking error: ReturnStatement is void, return type not void");
      }

      current->checkedType = returnType;
      return returnType;
    }

    case ExpressionStatement:
      //cerr<<"Inside expressionStatement"<<endl;
      return typeCheck(&current->m_generated_tokens[0], currentClass, localEnv, returnType, isStatic);

    case Assignment: {
      //cerr<<"Inside assignment"<<endl;
      TCType leftCheckedType = typeCheck(&current->m_generated_tokens[0], currentClass, localEnv, returnType, isStatic);
      TCType rightCheckedType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType, isStatic);

      // Need to check no assignment to array.length
      if (current->m_generated_tokens[0].m_type == QualifiedName) {
        if (current->m_generated_tokens[0].declaration->m_lex == "length") {
          throw std::logic_error("Type checking error: Assignment cannot assign to array.length"); 
        }
      }

      // Check for assignability
      if (isAssignable(leftCheckedType, rightCheckedType) == false) {
        throw std::logic_error("Type checking error: Assignment"); 
      }
     
      current->checkedType = leftCheckedType;

      return leftCheckedType;
    }

    case T_IDENTIFIER: {
      //cerr<<"Inside T_identifier"<<endl;
      TCType identifierType;
    
      if (current->declaration == nullptr) {
        cerr<<"\n\n\n";
        cerr<<current->m_lex<<endl;
        cerr<<"Class: "<<currentClass->m_generated_tokens[2].m_lex<<" ";
        
        cerr<<endl<<endl<<endl;
        throw std::logic_error("Type checking error: T_IDENTIFIER declaration nullptr");
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
        
        if (isStatic == true) {
          throw std::logic_error("Type checking error: T_IDENTIFIER cannot access field in static context");
        }

        identifierType = getType(&current->declaration->m_generated_tokens[1]);
      }

      current->checkedType = identifierType;
      return identifierType;
    }

    case NULL_LITERAL: {
      //cerr<<"Inside NULL"<<endl;
      TCType checkedType;
      checkedType.type = 5;

      current->checkedType = checkedType;
			return checkedType;
    }

    case CHAR_LITERAL: {
      //cerr<<"Inside CHAR literal"<<endl;
      TCType checkedType;
      checkedType.type = 0;
      checkedType.primitive = "char";

      current->checkedType = checkedType;
			return checkedType;
    }

    case INT_LITERAL: {
      //cerr<<"Inside INT literal"<<endl;
      TCType checkedType;
      checkedType.type = 0;
      checkedType.primitive = "int";

      current->checkedType = checkedType;
			return checkedType;
    }

    case BOOL_LITERAL: {
      //cerr<<"Inside BOOL literal"<<endl;
      TCType checkedType;
      checkedType.type = 0;
      checkedType.primitive = "boolean";

      current->checkedType = checkedType;
			return checkedType;
    }

    case STRING_LITERAL: {
      //cerr<<"Inside STRING literal"<<endl;
      //cerr<<current->declaration<<endl;
      TCType checkedType;
      checkedType.type = 1;
      checkedType.reference = current->declaration;

      current->checkedType = checkedType;
			return checkedType;
    }

    case T_THIS: {
      //cerr<<"Inside T_THIS"<<endl;
      TCType checkedType;
      
      // If method is static, error
      if (isStatic == true) {
        throw std::logic_error("Type checking error: T_THIS cannot be accesses in a static context");
      }

      checkedType.type = 1;
      checkedType.reference = currentClass;

      current->checkedType = checkedType;
			return checkedType;
    }

    case ConditionalOrExpression:
    case ConditionalAndExpression:
    case InclusiveOrExpression:
    case AndExpression: {
      //cerr<<"Inside ConditionalOrExpression"<<endl; 
      TCType leftCheckedType = typeCheck(&current->m_generated_tokens[0], currentClass, localEnv, returnType, isStatic);
      TCType rightCheckedType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType, isStatic);

      // Throw if expressions are not boolean
      if (leftCheckedType.primitive != "boolean" || rightCheckedType.primitive != "boolean") {
        throw std::logic_error("Type checking error: ConditionalOrExpression");
      }

      TCType checkedType;
      checkedType.type = 0;
      checkedType.primitive = "boolean";

      current->checkedType = checkedType;
			return checkedType;
    }

    case EqualityExpression: {
      //cerr<<"Inside Equality expression"<<endl; 
      TCType leftCheckedType = typeCheck(&current->m_generated_tokens[0], currentClass, localEnv, returnType, isStatic);
      TCType rightCheckedType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType, isStatic);

      set<string> numericTypes = {"int", "char", "short", "byte"};
      
      if (numericTypes.find(leftCheckedType.primitive) != numericTypes.end() && numericTypes.find(rightCheckedType.primitive) != numericTypes.end()) {
        TCType checkedType;
        checkedType.type = 0;
        checkedType.primitive = "boolean";

        current->checkedType = checkedType;
			return checkedType;
      }
      // Error if not assignable
      else if ((isAssignable(leftCheckedType, rightCheckedType) == false) && (isAssignable(rightCheckedType, leftCheckedType) == false))  {
        throw std::logic_error("Type checking error: EqualityExpression");
      }

      TCType checkedType;
      checkedType.type = 0;
      checkedType.primitive = "boolean";

      current->checkedType = checkedType;
			return checkedType;
    }

    case RelationalExpression: {
      TCType leftCheckedType = typeCheck(&current->m_generated_tokens[0], currentClass, localEnv, returnType, isStatic);
      TCType rightCheckedType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType, isStatic);

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

      current->checkedType = checkedType;
			return checkedType;
    }

    case AdditiveExpression: {
      //cerr<<"Inside Additive Expression"<<endl; 
      TCType leftCheckedType = typeCheck(&current->m_generated_tokens[0], currentClass, localEnv, returnType, isStatic);
      TCType rightCheckedType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType, isStatic);

      if (current->m_generated_tokens[1].m_type == T_PLUS) {
        // If either is String, then Expression type is String
        if ((leftCheckedType.type == 1) && (leftCheckedType.reference->m_generated_tokens[2].m_lex == "String")) {
          TCType checkedType;
          checkedType.type = 1;
          checkedType.reference = leftCheckedType.reference;

          // Other argument must not be void
          if (rightCheckedType.type == 4) {
            throw std::logic_error("Type checking error: AdditiveExpression cannot concatenate void with String");
          }

          current->checkedType = checkedType;
			    return checkedType;
        }
        else if ((rightCheckedType.type == 1) && (rightCheckedType.reference->m_generated_tokens[2].m_lex == "String")) {
          TCType checkedType;
          checkedType.type = 1;
          checkedType.reference = rightCheckedType.reference;

          // Other argument must not be void
          if (leftCheckedType.type == 4) {
            throw std::logic_error("Type checking error: AdditiveExpression cannot concatenate void with String");
          }

          current->checkedType = checkedType;
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

      current->checkedType = checkedType;
			return checkedType;
    }

    case MultiplicativeExpression: {
      //cerr<<"Inside Multiplicate Expression"<<endl; 
      TCType leftCheckedType = typeCheck(&current->m_generated_tokens[0], currentClass, localEnv, returnType, isStatic);
      TCType rightCheckedType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType, isStatic);

      // Both operands must be numeric
      if (numericTypes.find(leftCheckedType.primitive) == numericTypes.end() || numericTypes.find(rightCheckedType.primitive) == numericTypes.end()) {
        throw std::logic_error("Type checking error: MultiplicativeExpression not numeric");
      }

      TCType checkedType;
      checkedType.type = 0;
      checkedType.primitive = "int";

      current->checkedType = checkedType;
			return checkedType;
    }

    case CastExpression: {
      //cerr<<"Inside Cast Expression"<<endl;
      /*
      if (current->m_generated_tokens[current->m_generated_tokens.size() - 1].m_type == T_IDENTIFIER) {
        cerr<<endl<<current->m_generated_tokens[current->m_generated_tokens.size() - 1].m_lex<<" "<<current->m_generated_tokens[current->m_generated_tokens.size() - 1].declaration<<endl;
        cerr<<"Class name: "<<currentClass->m_generated_tokens[2].m_lex<<endl;
      }
      */
      TCType expressionType = typeCheck(&current->m_generated_tokens[current->m_generated_tokens.size() - 1], currentClass, localEnv, returnType, isStatic);
      TCType castType = typeCheck(&current->m_generated_tokens[1], currentClass, localEnv, returnType, isStatic);

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

      current->checkedType = castType;
      return castType;
    }

    case T_BYTE:
    case T_INT:
    case T_SHORT:
    case T_CHAR:
    case T_BOOLEAN: {
      //cerr<<"Inside T_BYTE or T_INT or ..."<<endl; 
      TCType checkedType;
      checkedType.type = 0;
      checkedType.primitive = current->m_lex;

      current->checkedType = checkedType;
			return checkedType;
    }

    case QualifiedName: {
      //cerr<<"Inside QualifiedName"<<endl; 
      TCType checkedType;

      // Should not be nullptr
      if (current->declaration == nullptr) {
        throw std::logic_error("Type checking error: QualifiedName declaration nullptr");
      }

      // Check if QualifiedName is array.length
      if (current->declaration->m_lex == "length") {
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
      //cerr<<"Inside Unary Expression Not plus minus"<<endl;
      TCType checkedType = typeCheck(&current->m_generated_tokens[1], currentClass, localEnv, returnType, isStatic);

      // Type must be boolean
      if ((checkedType.type != 0) || checkedType.primitive != "boolean") {
        throw std::logic_error("Type checking error: UnaryExpressionNotPlusMinus not boolean");
      }

      current->checkedType = checkedType;
			return checkedType;
    }

    case UnaryExpression: {
      //cerr<<"Inside Unary Expression"<<endl;
      TCType expressionType = typeCheck(&current->m_generated_tokens[1], currentClass, localEnv, returnType, isStatic);

      // Must be numeric type
      if (numericTypes.find(expressionType.primitive) == numericTypes.end()) {
        throw std::logic_error("Type checking error: UnaryExpression not numeric");
      }

      TCType checkedType;
      checkedType.type = 0;
      checkedType.primitive = "int";

      current->checkedType = checkedType;
			return checkedType;
    }

    case ArrayAccess: {
      //cerr<<"Inside Array Access"<<endl;
      TCType leftCheckedType = typeCheck(&current->m_generated_tokens[0], currentClass, localEnv, returnType, isStatic);
      TCType expressionType = typeCheck(&current->m_generated_tokens[2], currentClass, localEnv, returnType, isStatic);

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

      current->checkedType = checkedType;
			return checkedType;
    }

    case FieldAccess: {
      //cerr<<"Inside Field access"<<endl;
      TCType expressionType = typeCheck(&current->m_generated_tokens[0], currentClass, localEnv, returnType, isStatic);

      // Check for array.length
      if ((expressionType.type == 2 || expressionType.type == 3) && current->m_generated_tokens[2].m_lex == "length") {
        TCType checkedType;
        checkedType.type = 0;
        checkedType.primitive = "int";

        current->checkedType = checkedType;
        return checkedType;
      }

      // Must be reference type
      if (expressionType.type != 1) {
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
      else if (expressionType.reference->scope.fields[identifierName]->m_generated_tokens[0].SearchByTypeBFS(T_PROTECTED) != nullptr) {
        // Error if not in the same package and not a subclass
        if (expressionType.reference->compilation_unit->SearchByTypeDFS(PackageDeclaration) != currentClass->compilation_unit->SearchByTypeDFS(PackageDeclaration) && ((expressionType.reference->compilation_unit->SearchByTypeDFS(PackageDeclaration) == nullptr || currentClass->compilation_unit->SearchByTypeDFS(PackageDeclaration) == nullptr) || (currentClass->compilation_unit->SearchByTypeDFS(PackageDeclaration)->m_generated_tokens[1].m_lex != expressionType.reference->compilation_unit->SearchByTypeDFS(PackageDeclaration)->m_generated_tokens[1].m_lex))) {
          if (expressionType.reference->scope.fields[identifierName]->compilation_unit->SearchByTypeDFS(ClassDeclaration)->m_generated_tokens[2].m_lex == expressionType.reference->m_generated_tokens[2].m_lex || isSubtype(expressionType.reference, currentClass) == false) {
            throw std::logic_error("Type checking error: FieldAccess Protected field error");      
          }
        }
      }

      TCType checkedType = getType(&expressionType.reference->scope.fields[identifierName]->m_generated_tokens[1]);

      current->checkedType = checkedType;
			return checkedType;
    }

    case MethodInvocation: {
      //cerr<<"Inside Method Invocation"<<endl;
      string argSignature = "";

      // Check if any arguments
      if (current->m_generated_tokens[current->m_generated_tokens.size() - 2].m_type == ArgumentList) {
        for (Token& token: current->m_generated_tokens[current->m_generated_tokens.size() - 2].m_generated_tokens) {
          //cerr<<"Type of token IS: "<<token.m_type<<endl;
          if (token.m_type == T_COMMA) {
            continue;
          }
          TCType argumentType = typeCheck(&token, currentClass, localEnv, returnType, isStatic);
          
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
          else if (argumentType.type == 3) {
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

      //cerr<<"signature created..."<<endl;

      //cerr<<"Declaration pointer is: ";
      //cerr<<current->m_generated_tokens[0].declaration<<endl;
      //cerr<<"Type of first child: "<<current->m_generated_tokens[0].m_type<<endl;
      if (current->m_generated_tokens[0].m_type == QualifiedName) {
        //cerr<<"Number of children: "<<current->m_generated_tokens[0].m_generated_tokens.size()<<endl;
        for (Token& t: current->m_generated_tokens[0].m_generated_tokens) {
          //cerr<<t.m_lex<<endl;
        }
        //cerr<<"Name of class: "<<currentClass->m_generated_tokens[2].m_lex<<endl;
      }

      //cerr<<"Type is: "<<current->m_generated_tokens[0].declaration->m_type;
      
      // Distinguish between Primary . Identifier v/s Name
      if (current->m_generated_tokens[1].m_type == T_DOT) {
        //cerr<<"Primary . identifier"<<endl;
        TCType checkedType = typeCheck(&current->m_generated_tokens[0], currentClass, localEnv, returnType, isStatic);
    
        // Error if not reference type
        if (checkedType.type != 1) {
          throw std::logic_error("Type checking error: MethodInvocation on primitive type");
        }
        
        methodCalleeClass = checkedType.reference;
        methodName = current->m_generated_tokens[2].m_lex;
      }
      // Method is current class's method
      else if (current->m_generated_tokens[0].m_type == T_IDENTIFIER) {
        //cerr<<"Identifier..."<<endl;
        methodCalleeClass = currentClass;
        methodName = current->m_generated_tokens[0].m_lex;
      }
      // Qualified Name Static method call
      else if (current->m_generated_tokens[0].declaration->m_type == ClassDeclaration) {
        //cerr<<"Static method call"<<endl;
        shouldBeStatic = true;
        methodCalleeClass = current->m_generated_tokens[0].declaration;
        methodName = current->m_generated_tokens[0].m_generated_tokens[current->m_generated_tokens[0].m_generated_tokens.size() - 1].m_lex;
      }
      // Qualified Name method call on variable
      else if (current->m_generated_tokens[0].declaration->m_type == LocalVariableDeclaration || current->m_generated_tokens[0].declaration->m_type == FormalParameter) {
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
        //cerr<<"Field call"<<endl;
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

      if (methodCalleeClass == nullptr) {
        throw std::logic_error("Type checking error: MethodInvocation invalid method call - class null");
      }

      //cerr<<"Now searching for method: "<<methodName<<endl;
      //cerr<<"In class: "<<methodCalleeClass->m_generated_tokens[2].m_lex<<endl; 

      for (std::pair<std::string, std::map<std::string,std::vector<Token*>>> method: methodCalleeClass->scope.methodsWithSignatures) {
        //cerr<<"Method in environment: "<<method.first<<endl;
        // Check method with signature exists
        for (std::pair<std::string,std::vector<Token*>> signature: method.second) {
          //cerr<<"Method signature: "<<signature.first<<endl;
          //cerr<<"Arg signature: "<<argSignature<<endl;
          // Found match
          if (methodName == method.first && argSignature == signature.first) {
            foundMethodMatch = true;
            checkedType = getType(&signature.second[0]->m_generated_tokens[0].m_generated_tokens[1]);
            checkedType.implementation = signature.second[0];

            
            if (signature.second[0]->m_generated_tokens[0].m_generated_tokens[0].SearchByTypeBFS(T_PROTECTED) != nullptr) {
              // Error if not in the same package, and not an inherited member
              
               if (methodCalleeClass->compilation_unit->SearchByTypeDFS(PackageDeclaration) != currentClass->compilation_unit->SearchByTypeDFS(PackageDeclaration) && ((methodCalleeClass->compilation_unit->SearchByTypeDFS(PackageDeclaration) == nullptr || currentClass->compilation_unit->SearchByTypeDFS(PackageDeclaration) == nullptr) || (currentClass->compilation_unit->SearchByTypeDFS(PackageDeclaration)->m_generated_tokens[1].m_lex != methodCalleeClass->compilation_unit->SearchByTypeDFS(PackageDeclaration)->m_generated_tokens[1].m_lex))) {
                if (methodCalleeClass->scope.methodsWithSignaturesInherited.find(methodName) == methodCalleeClass->scope.methodsWithSignaturesInherited.end() || methodCalleeClass->scope.methodsWithSignaturesInherited[methodName].find(argSignature) == methodCalleeClass->scope.methodsWithSignaturesInherited[methodName].end()) {
                  if (methodCalleeClass->scope.methodsWithSignaturesInherited[methodName][argSignature][0]->SearchByTypeDFS(ClassDeclaration) && isSubtype(methodCalleeClass->scope.methodsWithSignaturesInherited[methodName][argSignature][0]->SearchByTypeDFS(ClassDeclaration), currentClass) == false) {
                    throw std::logic_error("Type checking error: MethodInvocation Protected method error"); 
                  }
                  else if (methodCalleeClass->scope.methodsWithSignaturesInherited[methodName][argSignature][0]->SearchByTypeDFS(InterfaceDeclaration) && isSubtype(methodCalleeClass->scope.methodsWithSignaturesInherited[methodName][argSignature][0]->SearchByTypeDFS(InterfaceDeclaration), currentClass) == false) {
                    throw std::logic_error("Type checking error: MethodInvocation Protected method error");
                  }
                }
              } 
            }

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
      //cerr<<"Inside Array creation Expression"<<endl;
      TCType creationType = typeCheck(&current->m_generated_tokens[1], currentClass, localEnv, returnType, isStatic);
      TCType checkedType;

      TCType expressionType = typeCheck(&current->m_generated_tokens[2].m_generated_tokens[1], currentClass, localEnv, returnType, isStatic);

      if (numericTypes.find(expressionType.primitive) == numericTypes.end()) {
        throw std::logic_error("Type checking error: ArrayCreationExpression expression not numeric");
      }

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
      //cerr<<"Inside Class instance creation Expression"<<endl;
      TCType creationType = typeCheck(&current->m_generated_tokens[1], currentClass, localEnv, returnType, isStatic);

      string argSignature = "";

      // Check if any arguments
      if (current->m_generated_tokens[current->m_generated_tokens.size() - 2].m_type == ArgumentList) {
        for (Token& token: current->m_generated_tokens[current->m_generated_tokens.size() - 2].m_generated_tokens) {
          TCType argumentType = typeCheck(&token, currentClass, localEnv, returnType, isStatic);
          
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
          else if (argumentType.type == 3) {
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
          throw std::logic_error("Type checking error: ClassInstanceCreationExpression constructor has different name");
        }

        // Check constructor with signature exists
        for (std::pair<std::string,std::vector<Token*>> signature: constructor.second) {
          if (signature.first == argSignature) {
            
            // Check for protected constructor
            if (signature.second[0]->m_generated_tokens[0].SearchByTypeBFS(T_PROTECTED) != nullptr) {
              // Error if not the same package or Subtype
              if (creationType.reference->compilation_unit->SearchByTypeDFS(PackageDeclaration) != currentClass->compilation_unit->SearchByTypeDFS(PackageDeclaration) && ((creationType.reference->compilation_unit->SearchByTypeDFS(PackageDeclaration) == nullptr || currentClass->compilation_unit->SearchByTypeDFS(PackageDeclaration) == nullptr) || (currentClass->compilation_unit->SearchByTypeDFS(PackageDeclaration)->m_generated_tokens[1].m_lex != creationType.reference->compilation_unit->SearchByTypeDFS(PackageDeclaration)->m_generated_tokens[1].m_lex))) {
                throw std::logic_error("Type checking error: ClassInstanceCreationExpression Protected constructor error");
              }
            }
            signatureMatch = true;
            checkedType.implementation = signature.second[0];
          }
        }
      }

      // Error if no constructor matched
      if (signatureMatch == false) {
        throw std::logic_error("Type checking error: ClassInstanceCreationExpression signature did not match");
      }

      string emptySignature = "";

      // If not Object class, then need to check for 0 argument superclass constructor
      if (creationType.reference->m_generated_tokens[2].m_lex != "Object") {
        // Check that superClass has 0 argument constructor
        for (std::pair<std::string, std::map<std::string,std::vector<Token*>>> constructor: creationType.reference->super_class->scope.constructorsWithSignatures) {
          if (constructor.second.find(emptySignature) == constructor.second.end()) {
            throw std::logic_error("Type checking error: ClassInstanceCreationExpression Zero argument superclass constructor does not exist");
          }
        }
      }

      checkedType.type = 1;
      checkedType.reference = creationType.reference;

      current->checkedType = checkedType;
			return checkedType;
    }

    case PrimaryNoNewArray: {
      //cerr<<"Inside Primary no new array"<<endl;
      TCType expressionType = typeCheck(&current->m_generated_tokens[1], currentClass, localEnv, returnType, isStatic);

      current->checkedType = expressionType;
      return expressionType;
    }

    default: {
      TCType checkedType;
      checkedType.type = -1;

      //cerr<<"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
      //cerr<<"Landed in default case: Type is "<<current->m_type<<endl;

      current->checkedType = checkedType;
			return checkedType;
    }
  }
}

bool isAssignable(TCType lhs, TCType rhs) {

  // If either of them is VOID, return false
  if (lhs.type == 4 || rhs.type == 4) {
    return false;
  }

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
      return false;
    }
    // Check assignability if type is reference array
    else if ((rhs.type == 3) && (isSubtype(lhs.reference, rhs.reference) == false)) {
      return false;
    }
  }
  // If LeftHandSide is array, but RightHandSide is not, ERROR
  else if (((lhs.type == 2) || (lhs.type == 3)) && rhs.type != 5) {
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
    if ((lhs.type != 0) || ((lhs.primitive != "char") && (lhs.primitive != "int") && (lhs.primitive != "byte") && (lhs.primitive != "short"))) {
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
  else if (rhs.type == 5) {
    if (lhs.type != 1 && lhs.type != 2 && lhs.type != 3) {
      return false;
    }
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

  // Special case ObjectINTERFACE and Object
  if (superType->m_generated_tokens[2].m_lex == "Object" && subType->m_generated_tokens[2].m_lex == "ObjectINTERFACE") {
    return true;
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
    return nodeType;
  }

  Token* arrayType = current->SearchByTypeDFS(ArrayType);
  if(arrayType) {
    if(arrayType->SearchByTypeDFS(QualifiedName)) {
      Token* qualifiedName = arrayType->SearchByTypeDFS(QualifiedName);
      nodeType.type = 3;
      nodeType.reference = qualifiedName->declaration;
    }
    else if(arrayType->SearchByTypeDFS(T_IDENTIFIER)) {
      Token* id = arrayType->SearchByTypeDFS(T_IDENTIFIER);
      nodeType.type = 3;
      nodeType.reference = id->declaration;
    }
    else {
      nodeType.type = 2;
      nodeType.primitive = arrayType->m_generated_tokens[0].m_lex;
    }
  } else {
    if(current->SearchByTypeDFS(QualifiedName)) {
      Token* qualifiedName = current->SearchByTypeDFS(QualifiedName);
      nodeType.type = 1;
      nodeType.reference = qualifiedName->declaration;
    }
    else if(current->SearchByTypeDFS(T_IDENTIFIER)) {
      Token* id = current->SearchByTypeDFS(T_IDENTIFIER);
      nodeType.type = 1;
      nodeType.reference = id->declaration;
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

  vector<Token*> tokens;
  Token* current;
  tokens.push_back(astRoot);
  while (tokens.size() > 0) {
    current = tokens[0];
    tokens.erase(tokens.begin() + 0);
    if (current->m_type == MethodDeclaration) {
      Token* currentClass = current->compilation_unit->SearchByTypeDFS(ClassDeclaration);
      TCType returnType;

      returnType = getType(&current->m_generated_tokens[0].m_generated_tokens[1]);
      bool isStatic = false;

      if (current->m_generated_tokens[0].m_generated_tokens[0].SearchByTypeBFS(T_STATIC) != nullptr) {
        isStatic = true;
      }

      //cerr<<"Method name: "<<current->m_generated_tokens[0].m_generated_tokens[2].m_generated_tokens[0].m_lex<<endl;
      //cerr<<"Method return type: "<<returnType.type<<" ";
      string sig;
      if (current->m_generated_tokens[0].m_generated_tokens[2].m_generated_tokens[2].m_type == FormalParameterList) {
        //cerr<<"Method signature: "<<getType(&current->m_generated_tokens[0].m_generated_tokens[2].m_generated_tokens[2].m_generated_tokens[0].m_generated_tokens[0]).primitive<<endl;
        sig = getType(&current->m_generated_tokens[0].m_generated_tokens[2].m_generated_tokens[2].m_generated_tokens[0].m_generated_tokens[0]).primitive;
      }
      if (returnType.type == 0) {
        //cerr<<returnType.primitive<<endl;
      }
      else if (returnType.type == 1) {
        //cerr<<returnType.reference->m_generated_tokens[2].m_lex<<endl;
      }

      //cerr<<"MethodDeclaration found"<<endl;

      /*
      if (current->m_generated_tokens[0].m_generated_tokens[2].m_generated_tokens[0].m_lex == "valueOf" && sig == "short") {
        cerr<<"Testing valueOf(short i) in class String now "<<endl;
        cerr<<"Printing declaration of id: "<<current->m_generated_tokens[1].m_generated_tokens[1].m_generated_tokens[0].m_generated_tokens[1].m_generated_tokens[2].m_generated_tokens[0].m_generated_tokens[current->m_generated_tokens[1].m_generated_tokens[1].m_generated_tokens[0].m_generated_tokens[1].m_generated_tokens[2].m_generated_tokens[0].m_generated_tokens.size() - 1].m_type<<endl;
        cerr<<"Printing declaration of id: "<<current->m_generated_tokens[1].m_generated_tokens[1].m_generated_tokens[0].m_generated_tokens[1].m_generated_tokens[2].m_generated_tokens[0].m_generated_tokens[current->m_generated_tokens[1].m_generated_tokens[1].m_generated_tokens[0].m_generated_tokens[1].m_generated_tokens[2].m_generated_tokens[0].m_generated_tokens.size() - 1].declaration<<endl;
      }
      */
      current->checkedType = returnType;
      if (current->m_generated_tokens[1].m_type == Block && current->m_generated_tokens[1].m_generated_tokens[1].m_type == BlockStatements) {
        //cerr<<"Calling typeCheck"<<endl;
        TCType checkedType = typeCheck(&current->m_generated_tokens[1].m_generated_tokens[1], currentClass, current->scope, returnType, isStatic);
      }
    }
    else if (current->m_type == ConstructorDeclaration) {
      Token* currentClass = current->compilation_unit->SearchByTypeDFS(ClassDeclaration);
      TCType returnType;
      
      returnType.type = 4;

      string emptySignature = "";

      // If not Object class, then need to check for 0 argument superclass constructor
      if (currentClass->m_generated_tokens[2].m_lex != "Object") {
        // Check that superClass has 0 argument constructor
        for (std::pair<std::string, std::map<std::string,std::vector<Token*>>> constructor: currentClass->super_class->scope.constructorsWithSignatures) {
          if (constructor.second.find(emptySignature) == constructor.second.end()) {
            throw std::logic_error("Type checking error: ConstructorDeclaration Zero argument superclass constructor does not exist");
          }
        }
      }

      current->checkedType = returnType;
      if (current->m_generated_tokens[2].m_generated_tokens[1].m_type == BlockStatements) {
        TCType checkedType = typeCheck(&current->m_generated_tokens[2].m_generated_tokens[1], currentClass, current->scope, returnType, false);
      }
    }
    else if (current->m_type == FieldDeclaration) {
      // cerr<<"Encountered field declaration..."<<endl;
      Token* currentClass = current->compilation_unit->SearchByTypeDFS(ClassDeclaration);
      TCType returnType;

      returnType.type = 4;

      TCType declaredType = getType(&current->m_generated_tokens[1]);
      bool isStatic = false;

      if (current->m_generated_tokens[0].SearchByTypeBFS(T_STATIC) != nullptr) {
        isStatic = true;
      }

      if (current->m_generated_tokens[2].m_type == VariableDeclarator) {
        TCType initializerType = typeCheck(&current->m_generated_tokens[2].m_generated_tokens[2], currentClass, currentClass->scope, returnType, isStatic);
        
        if (isAssignable(declaredType, initializerType) == false) {
          throw std::logic_error("Type checking error: FieldDeclaration not assignable");
        }
      }

      current->checkedType = declaredType;
    }
    for (Token& token: current->m_generated_tokens) {
      tokens.push_back(&token);
    }
  }
}
