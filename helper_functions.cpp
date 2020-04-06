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
