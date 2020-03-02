#pragma once
#include <map>
#include <string>
#include <vector>


class Token;

class environment {

  // Hierarchy checking on methods
  bool valid_method(std::pair<std::string,std::vector<Token*>>& kv);

  // Hierarchy checking on constructors
  bool valid_ctor(std::pair<std::string,std::vector<Token*>>& kv);
 public:
  environment() = default;
  environment(const environment& other);
  ~environment() = default;
  bool merge(environment src);
  void overwrite_merge(environment& src);
  
  Token* GetType(std::string & name);
  Token* GetClass(std::string & name);
  Token* GetInterface(std::string & name);
  
  // Used for inheritance;
  bool replace_merge(environment& src);
  void clear();
  
  std::map<std::string,Token*> classes;
  std::map<std::string,Token*> interfaces;
  std::map<std::string,Token*> fields;
  std::map<std::string,std::vector<Token*>> methods;
  std::map<std::string,Token*> localVariables;
  std::map<std::string,Token*> formalParameters;
  std::map<std::string,std::vector<Token*>> constructors;
};
