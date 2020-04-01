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
  dec = envs[1]->GetType(name);
  if(dec != nullptr) return dec;
  dec = envs[2]->GetType(name);
  if(dec != nullptr) return dec;
  dec = envs[3]->GetType(name);
  if(dec != nullptr) return dec;
  RED();
  std::cerr<<"GetTypeFromEnv ERROR:"<<name<<" is ambiguous or undefined."<<std::endl;
  DEFAULT();
  return nullptr;
}
