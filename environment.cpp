#include "environment.h"
#include "token.h"
#include "color_print.h"
#include <utility>
#include <iostream>

/*
 * Copy CTOR
 */

environment::environment(const environment& other)/*:
  classes(other.classes.size()),
  interfaces(other.interfaces.size()),
  fields(other.fields.size()),
  methods(other.methods.size()),
  localVariables(other.localVariables.size()),
  formalParameters(other.formalParameters.size()),
  constructors(other.constructors.size())*/
{
  for(std::pair<std::string,Token*> kv_pair: other.classes){
    classes[kv_pair.first] = kv_pair.second;
  }

  for(std::pair<std::string,Token*> kv_pair: other.interfaces){
    interfaces[kv_pair.first] = kv_pair.second; //other.interfaces[kv_pair.first];
  }

  for(std::pair<std::string,Token*> kv_pair: other.fields){
    fields[kv_pair.first] = kv_pair.second;//other.fields[kv_pair.first];
  }

  for(std::pair<std::string,std::vector<Token*>> kv_pair: other.methods){
    std::vector<Token*> new_vec;
    methods[kv_pair.first] = new_vec;
    for(Token* t: kv_pair.second){
      methods[kv_pair.first].emplace_back(t);
    }
  }

  for(std::pair<std::string,Token*> kv_pair: other.localVariables){
    localVariables[kv_pair.first] = kv_pair.second;//other.localVariables[kv_pair.first];
  }
  
  for(std::pair<std::string,Token*> kv_pair: other.formalParameters){
    formalParameters[kv_pair.first] = kv_pair.second;//other.formalParameters[kv_pair.first];
  }

  for(std::pair<std::string,std::vector<Token*>> kv_pair: other.constructors){
    std::vector<Token*> new_vec;
    constructors[kv_pair.first] = new_vec;
    for(Token* t: kv_pair.second){
      constructors[kv_pair.first].emplace_back(t);
    }
  }
  
  
  
}

Token* GetNodeByType(std::vector<Token*>& nodes, TokenType type){
  for(Token* node: nodes){
    if(node->type() == type) return node;
  }
  return nullptr;
}

bool environment::valid_method(std::pair<std::string,std::vector<Token* >>& kv){
  if(methods.find(kv.first) == methods.end() ||
     constructors.find(kv.first) == constructors.end()){
    return true;
  } else {
    
    // check for method with different parameters
    return true;
  }
  return false;
}

bool environment::valid_ctor(std::pair<std::string,std::vector<Token* >>& kv){
  if(methods.find(kv.first) == methods.end() ||
     constructors.find(kv.first) == constructors.end()){
    return true;
  } else {
    // check for ctors with different parameters
    return true;
  }
  return false;
}

bool environment::replace_merge(environment& src){
  return true;
}

bool environment::merge(environment src){
  // Merge classes
  int counter = 0;
  YELLOW();
  std::cout<<"MERGING ... " <<std::endl;
  DEFAULT();
  std::cout<<"STEP 1"<<std::endl;
  for(std::pair<std::string, Token*> kv_pair: src.classes){
    std::cout<<"STEP 2:"<<kv_pair.first<<std::endl;
    if(classes.find(kv_pair.first) == classes.end() ){
       //interfaces.find(kv_pair.first) == interfaces.end()){
       
      std::cout<<"CLASS ENV: Added ["<<kv_pair.first<<","<<kv_pair.second;
      std::cout<<","<<*(kv_pair.second)<<"] "<<counter++<<std::endl;
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
  for(std::pair<std::string, Token*> kv_pair: src.interfaces){
    if(classes.find(kv_pair.first) == classes.end() &&
       interfaces.find(kv_pair.first) == interfaces.end()){
      
      std::cout<<"INTERFACE ENV: Added ["<<kv_pair.first<<","<<(*(kv_pair.second));
      std::cout<<","<<kv_pair.second->m_lex<<"]"<<std::endl;
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
  for(std::pair<std::string, std::vector<Token*>> kv_pair: src.methods){
    if(valid_method(kv_pair)){
      for(Token* n: kv_pair.second){
	
      std::cout<<"METHOD ENV: Added ["<<kv_pair.first<<","<<(*n);
      std::cout<<","<<n->m_lex<<"]"<<std::endl;
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
  for(std::pair<std::string, std::vector<Token*>> kv_pair: src.constructors){
    if(valid_ctor(kv_pair)){
      for(Token* n: kv_pair.second){
	
      std::cout<<"CTOR ENV: Added ["<<kv_pair.first<<","<<(*n);
      std::cout<<","<<n->m_lex<<"]"<<std::endl;
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
  for(std::pair<std::string, Token*> kv_pair: src.formalParameters){
    if(formalParameters.find(kv_pair.first) == formalParameters.end() &&
       localVariables.find(kv_pair.first) == localVariables.end()){
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
  for(std::pair<std::string, Token*> kv_pair: src.fields){
    if(fields.find(kv_pair.first) == fields.end()){
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
  for(std::pair<std::string, Token*> kv_pair: src.localVariables){
    if(formalParameters.find(kv_pair.first) == formalParameters.end() &&
       localVariables.find(kv_pair.first) == localVariables.end()){
      localVariables[kv_pair.first] = kv_pair.second;
    }else{
      RED();
      std::cerr<<"TYPE LINKER ERROR: variable: "<<kv_pair.first;
      std::cerr<<" already declared"<<std::endl;
      DEFAULT();
      return false;
    }
  }
  return true;
}

void environment::overwrite_merge(environment& src){
  // Merge classes
  for(std::pair<std::string, Token*> kv_pair: src.classes){
    if(classes.find(kv_pair.first) == classes.end() &&
       interfaces.find(kv_pair.first) == interfaces.end()){
      classes[kv_pair.first] = kv_pair.second;
    }else{
      classes[kv_pair.first] = nullptr;
    }
  }
  // Merge interfaces
  for(std::pair<std::string, Token*> kv_pair: src.interfaces){
    if(classes.find(kv_pair.first) == classes.end() &&
       interfaces.find(kv_pair.first) == interfaces.end()){
      interfaces[kv_pair.first] = kv_pair.second;
    }else{
      interfaces[kv_pair.first] = nullptr;
    }
  }
  
  // Merge methods, current doesn't check the methods with same parameters
  for(std::pair<std::string, std::vector<Token*>> kv_pair: src.methods){
    if(valid_method(kv_pair)){
      for(Token* n: kv_pair.second){
	methods[kv_pair.first].emplace_back(n);
      }
    }else{
      std::vector<Token*> new_vec;
      methods[kv_pair.first] = new_vec;
    }
  }

  // Merge Ctors
  for(std::pair<std::string, std::vector<Token*>> kv_pair: src.constructors){
    if(valid_ctor(kv_pair)){
      for(Token* n: kv_pair.second){
	constructors[kv_pair.first].emplace_back(n);
      }
    }else{ 
      std::vector<Token*> new_vec;
      constructors[kv_pair.first] = new_vec;
     
    }
  }
  
  // Merge formal Parameters
  for(std::pair<std::string, Token*> kv_pair: src.formalParameters){
    if(formalParameters.find(kv_pair.first) == formalParameters.end() &&
       localVariables.find(kv_pair.first) == localVariables.end()){
      formalParameters[kv_pair.first] = kv_pair.second;
    }else{
      formalParameters[kv_pair.first] = nullptr;
    }
  }

  // Merge fields
  for(std::pair<std::string, Token*> kv_pair: src.fields){
    if(fields.find(kv_pair.first) == fields.end()){
      fields[kv_pair.first] = kv_pair.second;
    }else{
      fields[kv_pair.first] = nullptr;
    }
  }

  // Merge localVariables
  for(std::pair<std::string, Token*> kv_pair: src.localVariables){
    if(formalParameters.find(kv_pair.first) == formalParameters.end() &&
       localVariables.find(kv_pair.first) == localVariables.end()){
      localVariables[kv_pair.first] = kv_pair.second;
    }else{
      localVariables[kv_pair.first] = nullptr;
    }
  }
}

Token* environment::GetType(std::string & name){
  if(classes.find(name) == classes.end()){
    if(interfaces.find(name) == interfaces.end()){
      return nullptr;
    }
    return ((Token*) interfaces[name]);
  }
  return ((Token*) classes[name]);
}

Token* environment::GetClass(std::string & name){
  if(classes.find(name) == classes.end()) return nullptr;
  return classes[name];
}

Token* environment::GetInterface(std::string& name){
  if(interfaces.find(name) == interfaces.end()) return nullptr;
  return interfaces[name];
}

void environment::clear(){
  classes.clear();
  interfaces.clear();
  fields.clear();
  methods.clear();
  localVariables.clear();
  formalParameters.clear();
  constructors.clear();
}
