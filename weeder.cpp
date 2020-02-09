#include "color_print.h"
#include "weeder.h"

#include <exception>
#include <sstream>
#include <iostream>

Weeder::Weeder(const std::string& file_name, const Token& parse_tree):
  m_parse_tree(parse_tree)
{
  std::vector<std::string> slash_vec;
  //std::vector<std::string> dot_vec;
  split(file_name,'/',slash_vec);
  class_name = slash_vec[slash_vec.size()-1];
  // Get rid of .java 
  class_name.erase(class_name.end()-5,class_name.end());
  
}

void Weeder::split(const std::string& s, const char delim, std::vector<std::string>& lst){
  std::stringstream ss(s);
  std::string piece;
  while(std::getline(ss,piece,delim)){
    lst.emplace_back(piece);
  }
}

bool Weeder::weed(Token& node){

  // Check before loop
  //int type_n = static_cast<int>(node.m_type);
  switch(node.m_type){
  case TokenType::ClassDeclaration:
    // Check if the file name matches source
    if(node.m_generated_tokens[2].m_lex.compare(class_name)){
      RED();
      std::cerr<<"WEEDER ERROR: class name and file name doesn't match!";
      std::cerr<<std::endl;
      DEFAULT();
      return false;
    }
    break;
  default:
    break;
  }

  // Recursive call
  for(Token& t: node.m_generated_tokens){
    if(!weed(t)) return false;
  }
  
  return true;
}

bool Weeder::weed(){
  return weed(m_parse_tree);
}
