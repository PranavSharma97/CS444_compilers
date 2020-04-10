#include <string>
#include <vector>

#include "environment.h"
#include "token.h"


void string_split(const std::string& s, const char delim, std::vector<std::string>& lst);

Token* GetTypeFromEnv(std::string& name, environment** envs);

bool IsSubClass(Token* sub, Token* super);
