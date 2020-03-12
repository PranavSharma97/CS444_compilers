#pragma once

#include <vector>
#include <map>
#include <string>
#include "token.h"
#include "package.h"
#include "environment.h"
#include "name_checker.h"

class TypeLinker{
  const std::string default_package_name;
  // Need to be freed
  Package*  m_packages;
  Token* java_lang_object_interface;
  std::vector<Token*> m_asts;
  NameChecker* m_name_checker;
  
  bool ConstructPackage();
  bool HasEnv(Token* root);
  bool HasEnv(TokenType t);

  // Simple Name resolving
  environment* GetCurrentPackage(Token* CUN);
  Token* GetFromEnv(std::string &name, environment** envs);
  
  // Type Linking
  bool ResolvePackage(Token* cun, environment** envs);
  bool ResolveAST(Token* root, environment** envs);
  bool DoLinkType(Token* id, environment** envs);
  bool CheckName(std::string& name,environment** envs,bool ondemand = false);

  // Hierarchy Checking
  bool ResolveInheritance(Token* node,environment** envs);
  bool DoInherit(Token* node, environment** envs);
  bool DoInheritClass(Token* sub,Token* super,std::map<Token*,bool>& duplicate,
		      environment** envs);
  
  bool DoInheritInterface(Token* sub,Token* interfaces,
			  std::map<Token*,bool>& duplicate,
			  environment** envs);

  // Resolving names
  bool ResolveNameSpaces(Token* node,environment** envs);
  bool ResolveFieldDeclarations(Token* root, environment** envs);
  bool ResolveExpressions(Token* node, environment** envs, bool methodOrConstructor);

  
  void init_name_checker(environment* local,environment* single,
			 environment** pack,environment* ondemand,size_t file_count);
  public:
  TypeLinker(const std::vector<Token*>& asts);
  ~TypeLinker();
  bool Link();
  void set_object_interface(Token* t,NameChecker* name_checker);
};
