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

  // Simple Name resolving
  environment* GetCurrentPackage(CompilationUnitNode* CUN);
  ASTNode* GetByType(std::vector<ASTNode*>& ndoes, TokenType type);
  ASTNode* GetFromEnv(std::string &name, environments** envs);
  
  // Type Linking 
  bool ResolveAST(ASTNode* root, environment** envs);
  bool DoLinkType(IdentifierNode* id, environment** envs);

  // Hierarchy Checking
  bool ResolveInheritance(ASTNode* node,environment** envs);
  bool DoInherit(ASTNode* node, environment** envs);
  bool DoInherit(ASTNode* sub,ASTNode* super,ASTNode* start,environment** envs);
 public:
  TypeLinker(const std::vector<ASTNode*>& asts);
  ~TypeLinker();
  bool Link();
};
