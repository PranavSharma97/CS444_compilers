#include <sstream>
#include <iostream>
#include "color_print.h"

#include "helper_functions.h"

void string_split(const std::string& s, const char delim, std::vector<std::string>& lst){
  std::stringstream ss(s);
  std::string piece;
  while(std::getline(ss,piece,delim)){
    lst.emplace_back(piece);
  }
}

Token* GetTypeFromEnv(std::string& name, environment ** envs){
  Token* dec = envs[0]->GetType(name);
  if(dec != nullptr) return dec;
  std::cout<<envs[1]->classes.size()<<std::endl;
  dec = envs[1]->GetType(name);
  if(dec != nullptr) return dec;
  if (envs[2]){
    dec = envs[2]->GetType(name);
    if(dec != nullptr) return dec;
  } else {
    std::cout<<"GET TYEP FROM ENV:env2 is null"<<std::endl;
  }
  dec = envs[3]->GetType(name);
  if(dec != nullptr) return dec;
  RED();
  std::cerr<<"GetTypeFromEnv ERROR:"<<name<<" is ambiguous or undefined."<<std::endl;
  DEFAULT();
  return nullptr;
}

bool IsSubClass(Token* sub, Token* super){
  bool result;
  if(sub == super) return true;
  if(sub == nullptr) return false;
  if(super == nullptr){
    RED();
    std::cerr<<"IsSubClass ERROR: super is null"<<std::endl;
    DEFAULT();
    return false;
  }
  if(IsSubClass(sub->super_class,super)) return true;
  if(sub->super_interfaces.size() > 0){
    for(Token* t: sub->super_interfaces){
      if(IsSubClass(t,super)) return true;
    }
  }
  return false;
}
