#pragma once
#include "token.h"
#include "package.h"
#include "environment.h"
#include <vector>

class NameChecker{
  bool GetAllValidType(Token* root, Token* last_resolved, int idx, environment** envs, int* dot_indices, bool is_method, bool search_type = false);
  bool ResolveQualifiedPart(Token* node, environment** envs, bool is_method = false);
 public:
  Package* m_packages;
  environment* local_envs;
  environment* single_types;
  environment** pack_envs;
  environment* on_demand;

  std::vector<Token *> m_asts;

  bool ResolveNameSpaces(Token* node,environment** envs);
  bool ResolveFieldDeclarations(Token* root, environment** envs);
  bool ResolveExpressions(Token* node, environment** envs, bool methodOrConstructor);

  NameChecker();
  ~NameChecker();
  bool CheckNames();
};
