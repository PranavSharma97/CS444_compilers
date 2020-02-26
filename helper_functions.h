#pragma once

#include <string>
#include <vector>
#include <sstream>

void string_split(const std::string& s, const char delim, std::vector<std::string>& lst){
  std::stringstream ss(s);
  std::string piece;
  while(std::getline(ss,piece,delim)){
    lst.emplace_back(piece);
  }
}
