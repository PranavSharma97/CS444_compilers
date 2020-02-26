#pragma_once

#include <vector>
#include <map>
#include <string>
#include "astNodes.h"
#include "package.h"

class TypeLinker{
  // Need to be freed
  Package*  m_packages;
  std::vector<ASTNode*> m_asts;

  ASTNode* SearchNodeType(ASTNode* root, TokenType type);
  bool MergeEnvironment(environment* src, environment* std);
  bool ConstructPackage();
 public:
  TypeLinker(const std::vector<ASTNode*>& asts);
  ~TypeLinker();
  bool Link();
};
