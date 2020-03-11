#pragma once
#include <map>
#include <string>
#include <vector>


class Token;

class environment {

  // Hierarchy checking on constructors
  bool valid_ctor(std::pair<std::string,std::vector<Token*>>& kv);

  bool valid_method(std::pair<std::string,std::map<std::string,std::vector<Token*>>>& srcMethod);
 public:
  environment() = default;
  environment(const environment& other);
  ~environment() = default;
  bool merge(environment src,Token** clash_token = nullptr);
  void force_merge(environment src,Token** clash_token = nullptr);
  void overwrite_merge(environment& src);
  // used for checking self import
  bool check_exist(Token* t);
  
  Token* GetType(std::string & name);
  Token* GetClass(std::string & name);
  Token* GetInterface(std::string & name);
  
  // Used for inheritance;
  bool replace_merge(environment& src);
  void build_declared_set();
     
  bool checkMethods();

  bool checkConstructors();

  bool check_return_types(Token* src, Token* current);

  void postProcessMethodMap();

  void postProcessConstructorMap();
  void clear();
  
  // Used to resolve names
  Token* GetDeclaration(std::string& name);
  std::vector<Token*> GetInvocationDeclaration(std::string& name);

  std::map<std::string,Token*> classes;
  std::map<std::string,Token*> interfaces;
  std::map<std::string,Token*> fields;
  std::map<std::string,std::vector<Token*>> methods;
  std::map<std::string,Token*> localVariables;
  std::map<std::string,Token*> formalParameters;
  std::map<std::string,std::vector<Token*>> constructors;
  std::map<std::string,std::map<std::string,std::vector<Token*>>> methodsWithSignatures;
  std::map<std::string,std::map<std::string,std::vector<Token*>>> methodsWithSignaturesDeclared;
  std::map<std::string,std::map<std::string,std::vector<Token*>>> methodsWithSignaturesInherited;
  std::map<std::string,std::map<std::string,std::vector<Token*>>> constructorsWithSignatures;
};
