#include "token.h"
#include "type.h"
#include "environment.h"

bool checkTypes(Token* astRoot);
TCType typeCheck(Token* current, Token* currentClass, environment localEnv, TCType returnType, bool isStatic);
bool isAssignable(TCType lhs, TCType rhs);
bool isSubtype(Token* superType, Token* subType);
TCType getType(Token* current);
