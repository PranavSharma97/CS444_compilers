/*
#pragma once
#include <map>
#include <string>
#include <vector>

class environment {
 public:
  environment() = default;
  bool merge(environment* src);
  bool overwrite_merge(environment* src);
  std::map<std::string,classDeclarationNode*> classes;
  std::map<std::string,interfaceDeclarationNode*> interfaces;
  std::map<std::string,FieldDeclarationNode*> fields;
  std::map<std::string,std::vector<MethodDeclarationNode*>> methods;
  std::map<std::string,LocalVariableDeclarationNode*> localVariables;
  std::map<std::string,FormalParameterNode*> formalParameters;
  std::map<std::string,std::vector<ConstructorDeclarationNode*>> constructors;
};
*/

#include "environment.h"
#include "astNodes.h"
#include <utility>

bool environment::valid_method(std::pair<std::string,std::vector<MethodDeclarationNode* >>& kv){
  if(methods.find(kv.first) == methods.end() ||
     constructors.find(kv.first) == constructors.end()){
    return true;
  } else {
    // check for method with different parameters
  }
  return false;
}

bool environment::valid_ctor(std::pair<std::string,std::vector<ConsturctorDeclarationNode* >>& kv){
  if(methods.find(kv.first) == methods.end() ||
     constructors.find(kv.first) == constructors.end()){
    return true;
  } else {
    // check for ctors with different parameters
  }
  return false;
}
  

bool environment::merge(environment* src){
  // Merge classes
  for(std::pair<std::string, ClassDeclarationNode*> kv_pair: src->classes){
    if(classes.find(kv_pair.first) == classes.end() &&
       interfaces.find(kv_pair.first) == interfaces.end()){
      classes[kv_pair.first] = kv_pair.second;
    }else{
      RED();
      std::cerr<<"TYPE LINKER ERROR: Class: "<<kv_pair.first;
      std::cerr<<" already defined"<<std::endl;
      DEFAULT();
      return false;
    }
  }
  // Merge interfaces
  for(std::pair<std::string, InterfaceDeclarationNode*> kv_pair: src->interfaces){
    if(classes.find(kv_pair.first) == classes.end() &&
       interfaces.find(kv_pair.first) == interfaces.end()){
      interfaces[kv_pair.first] = kv_pair.second;
    }else{
      RED();
      std::cerr<<"TYPE LINKER ERROR: Interface: "<<kv_pair.first;
      std::cerr<<" already defined"<<std::endl;
      DEFAULT();
      return false;
    }
  }
  
  // Merge methods, current doesn't check the methods with same parameters
  for(std::pair<std::string, MethodDeclarationNode*> kv_pair: src->methods){
    if(valid_method(kv_pair)){
      for(MethodDeclarationNode* n: kv_pair.second){
	methods[kv_pair.first].emplace_back(n);
      }
    }else{
      RED();
      std::cerr<<"TYPE LINKER ERROR: method: "<<kv_pair.first;
      std::cerr<<" already declared"<<std::endl;
      DEFAULT();
      return false;
    }
  }

  // Merge Ctors
  for(std::pair<std::string, ConstructorDeclarationNode*> kv_pair: src->constructors){
    if(valid_ctor(kv_pair)){
      for(ConstructorDeclarationNode* n: kv_pair.second){
	constructors[kv_pair.first].emplace_back(n);
      }
    }else{
      RED();
      std::cerr<<"TYPE LINKER ERROR: method: "<<kv_pair.first;
      std::cerr<<" already declared"<<std::endl;
      DEFAULT();
      return false;
    }
  }
  
  // Merge formal Parameters
  for(std::pair<std::string, FormalParameterNode*> kv_pair: src->formalParameters){
    if(formalParameters.find(kv_pair.first) == formalParameters.end() &&
       localVariables.find(kv_pair.first) == localVariables.end() &&
       fields.find(kv_pair.first) == fields.end()){
      formalParameters[kv_pair.first] = kv_pair.second;
    }else{
      RED();
      std::cerr<<"TYPE LINKER ERROR: parameter: "<<kv_pair.first;
      std::cerr<<" duplicated."<<std::endl;
      DEFAULT();
      return false;
    }
  }

  // Merge fields
  for(std::pair<std::string, FieldDeclarationNode*> kv_pair: src->fields){
    if(formalParameters.find(kv_pair.first) == formalParameters.end() &&
       localVariables.find(kv_pair.first) == localVariables.end() &&
       fields.find(kv_pair.first) == fields.end()){
      fields[kv_pair.first] = kv_pair.second;
    }else{
      RED();
      std::cerr<<"TYPE LINKER ERROR: field: "<<kv_pair.first;
      std::cerr<<" already declared"<<std::endl;
      DEFAULT();
      return false;
    }
  }

  // Merge localVariables
  for(std::pair<std::string, LocalVariableDeclarationNode*> kv_pair: src->localVariables){
    if(formalParameters.find(kv_pair.first) == formalParameters.end() &&
       localVariables.find(kv_pair.first) == localVariables.end() &&
       fields.find(kv_pair.first) == fields.end()){
      localVariables[kv_pair.first] = kv_pair.second;
    }else{
      RED();
      std::cerr<<"TYPE LINKER ERROR: variable: "<<kv_pair.first;
      std::cerr<<" already declared"<<std::endl;
      DEFAULT();
      return false;
    }
  }
}

void environment::overwrite_merge(environment* src){
  // Merge classes
  for(std::pair<std::string, ClassDeclarationNode*> kv_pair: src->classes){
    if(classes.find(kv_pair.first) == classes.end() &&
       interfaces.find(kv_pair.first) == interfaces.end()){
      classes[kv_pair.first] = kv_pair.second;
    }else{
      classes[kv_pair.first] = nullptr;
    }
  }
  // Merge interfaces
  for(std::pair<std::string, InterfaceDeclarationNode*> kv_pair: src->interfaces){
    if(classes.find(kv_pair.first) == classes.end() &&
       interfaces.find(kv_pair.first) == interfaces.end()){
      interfaces[kv_pair.first] = kv_pair.second;
    }else{
      interfaces[kv_pair.first] = nullptr;
    }
  }
  
  // Merge methods, current doesn't check the methods with same parameters
  for(std::pair<std::string, MethodDeclarationNode*> kv_pair: src->methods){
    if(valid_method(kv_pair)){
      for(MethodDeclarationNode* n: kv_pair.second){
	methods[kv_pair.first].emplace_back(n);
      }
    }else{
      vector<MethodDeclarationNode*> new_vec;
      methods[kv_pair.first] = new_vec;
    }
  }

  // Merge Ctors
  for(std::pair<std::string, ConstructorDeclarationNode*> kv_pair: src->constructors){
    if(valid_ctor(kv_pair)){
      for(ConstructorDeclarationNode* n: kv_pair.second){
	constructors[kv_pair.first].emplace_back(n);
      }
    }else{ 
      vector<ConstructorDeclarationNode*> new_vec;
      constructors[kv_pair.first] = new_vec;
     
    }
  }
  
  // Merge formal Parameters
  for(std::pair<std::string, FormalParameterNode*> kv_pair: src->formalParameters){
    if(formalParameters.find(kv_pair.first) == formalParameters.end() &&
       localVariables.find(kv_pair.first) == localVariables.end() &&
       fields.find(kv_pair.first) == fields.end()){
      formalParameters[kv_pair.first] = kv_pair.second;
    }else{
      formalParameters[kv_pair.first] = nullptr;
    }
  }

  // Merge fields
  for(std::pair<std::string, FieldDeclarationNode*> kv_pair: src->fields){
    if(formalParameters.find(kv_pair.first) == formalParameters.end() &&
       localVariables.find(kv_pair.first) == localVariables.end() &&
       fields.find(kv_pair.first) == fields.end()){
      fields[kv_pair.first] = kv_pair.second;
    }else{
      fields[kv_pair.first] = nullptr;
    }
  }

  // Merge localVariables
  for(std::pair<std::string, LocalVariableDeclarationNode*> kv_pair: src->localVariables){
    if(formalParameters.find(kv_pair.first) == formalParameters.end() &&
       localVariables.find(kv_pair.first) == localVariables.end() &&
       fields.find(kv_pair.first) == fields.end()){
      localVariables[kv_pair.first] = kv_pair.second;
    }else{
      localVariables[kv_pair.first] = nullptr;
    }
  }
}

ASTNode* environment::GetType(std::string & name){
  if(classes.find(name) == classes.end()){
    if(interfaces.find(name) == interfaces.end()){
      return nullptr;
    }
    return ((ASTNode*) interfaces[name]);
  }
  return ((ASTNode*) classes[name]);
}
