#pragma once
#include <map>
#include <string>
#include <vector>

class ASTNode;

class environment {

  // Hierarchy checking on methods
  bool valid_method(std::pair<std::string,std::vector<MethodDeclarationNode*>>& kv);

  // Hierarchy checking on constructors
  bool valid_ctor(std::pair<std::string,std::vector<ConstructorDeclarationNode*>>& kv);
 public:
  environment() = default;
  bool merge(environment* src);
  void overwrite_merge(environment* src);
  
  ASTNode* GetType(std::string & name);
  ASTNode* GetClass(std::string & name);
  ASTNode* GetInterface(std::string & name);
  
  // Used for inheritance;
  bool replace_merge(environment* src);
  
  std::map<std::string,ClassDeclarationNode*> classes;
  std::map<std::string,InterfaceDeclarationNode*> interfaces;
  std::map<std::string,FieldDeclarationNode*> fields;
  std::map<std::string,std::vector<MethodDeclarationNode*>> methods;
  std::map<std::string,LocalVariableDeclarationNode*> localVariables;
  std::map<std::string,FormalParameterNode*> formalParameters;
  std::map<std::string,std::vector<ConstructorDeclarationNode*>> constructors;
};
