#include <map>
#include <string>

class ClassDeclarationNode;
class InterfaceDeclarationNode;
class FieldDeclarationNode;
class MethodDeclarationNode;
class LocalVariableDeclarationNode;
class FormalParameterNode;
class ConstructorDeclarationNode;

struct environment {
  bool merge(environment* src);
  std::map<std::string,ClassDeclarationNode*> classes;
  std::map<std::string,InterfaceDeclarationNode*> interfaces;
  std::map<std::string,FieldDeclarationNode*> fields;
  std::map<std::string,std::vector<MethodDeclarationNode*>> methods;
  std::map<std::string,LocalVariableDeclarationNode*> localVariables;
  std::map<std::string,FormalParameterNode*> formalParameters;
  std::map<std::string,std::vector<ConstructorDeclarationNode*>> constructors;
};
