#include "token.h"
#include "type.h"
#include "environment.h"

bool checkTypes(Token* astRoot);
Type typeCheck(Token* astRoot, Token* currentClass, environment localEnv, Type returnType);
bool checkInheritance(Token* superClass, Token* subClass);
