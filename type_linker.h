#pragma_once

#include <vector>
#include <map>
#include <string>
#include "astNodes.h"
#include "package.h"
#include "environment.h"

class TypeLinker{
  const std::string default_package_name;
  // Need to be freed
  Package*  m_packages;
  std::vector<ASTNode*> m_asts;
  
  ASTNode* SearchNodeType(ASTNode* root, TokenType type);
  bool ConstructPackage();
  bool HasEnv(ASTNode* root);
  bool HasEnv(TokenType t);
  bool ResolveAST(ASTNode* root, environment** envs);
  //bool ResolveType(ASTNode* root, environment* env);
  bool DoLinkClass(IdentifierNode* id, environment** envs);
  environment* GetCurrentPackage(CompilationUnitNode* CUN);
 public:
  TypeLinker(const std::vector<ASTNode*>& asts);
  ~TypeLinker();
  bool Link();
};
