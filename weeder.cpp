#include "color_print.h"
#include "weeder.h"

#include <exception>
#include <sstream>
#include <iostream>
#include <utility> // std::pair

Weeder::Weeder(const std::string& file_name, const Token& parse_tree):
  m_parse_tree(parse_tree)
{
  std::vector<std::string> slash_vec;
  //std::vector<std::string> dot_vec;
  split(file_name,'/',slash_vec);
  class_name = slash_vec[slash_vec.size()-1];
  // Get rid of .java 
  class_name.erase(class_name.end()-5,class_name.end());
  // Shrink tree
  shrink(m_parse_tree);
  
}


void Weeder::shrink(Token& node){
  // Shrink tree
  while(node.m_generated_tokens.size() == 1){
    node = node.m_generated_tokens[0];
  }
  for(Token& t:node.m_generated_tokens){
    shrink(t);
  }
}

bool Weeder::search(Token& node, TokenType key){
  bool result = false;
  if(node.m_type!=key){
    for(Token& t:node.m_generated_tokens){
      result = search(t,key);
      if(result) break;
    }
  }else{
    result = true;
  }
  return result;
}

bool Weeder::weed(Token& node,std::map<TokenType,int>& conditions){

  std::istringstream iss;
  std::map<TokenType,int> sub_cond = conditions;
  std::map<TokenType,int> search_map;
  /*
  std::cout<<"-----------------------"<<std::endl;
  if(conditions.find(TokenType::T_MINUS)==conditions.end()){
    RED();
    std::cout<<"NO ENTRY"<<std::endl;
  }else{
    std::cout<<conditions[TokenType::T_MINUS]<<std::endl;
  }
  */
  
  // Check before loop
  //int type_n = static_cast<int>(node.m_type);
  switch(node.m_type){
  case TokenType::MethodDeclaration:
    if (search(node.m_generated_tokens[0],TokenType::T_ABSTRACT) ||
      search(node.m_generated_tokens[0],TokenType::T_NATIVE)){
      if (search(node.m_generated_tokens[1],TokenType::Block)){
        RED();
        std::cerr<<"WEEDER ERROR: an abstract or native method cannot have a body!";
        std::cerr<<std::endl;
        DEFAULT();
        return false;
      }
    }
    if (search(node.m_generated_tokens[0],TokenType::T_ABSTRACT)){
      std::map<TokenType,int> illegal_modifiers = {{TokenType::T_STATIC,1},{TokenType::T_FINAL,1}};
      if (search_all(node.m_generated_tokens[0],illegal_modifiers)){
        RED();
        std::cerr<<"WEEDER ERROR: an abstract method cannot be static or final!";
        std::cerr<<std::endl;
        DEFAULT();
        return false;
      }
    }
    if (search(node.m_generated_tokens[0],TokenType::T_STATIC) &&
      search(node.m_generated_tokens[0],TokenType::T_FINAL)){
      RED();
      std::cerr<<"WEEDER ERROR: a static method cannot be final!";
      std::cerr<<std::endl;
      DEFAULT();
      return false;
    }
    if (search(node.m_generated_tokens[0],TokenType::T_NATIVE) &&
      !search(node.m_generated_tokens[0],TokenType::T_STATIC)){
      RED();
      std::cerr<<"WEEDER ERROR: a native method must be statuc!";
      std::cerr<<std::endl;
      DEFAULT();
      return false;
    }
    break;
  case TokenType::InterfaceDeclaration:
  case TokenType::ClassDeclaration:
    // Search for public key word in modifiers, if found check class name
    if(search(node.m_generated_tokens[0],TokenType::T_PUBLIC)){
      if(node.m_generated_tokens[2].m_lex.compare(class_name)){
	  RED();
	  std::cerr<<"WEEDER ERROR: class name and file name doesn't match!";
	  std::cerr<<std::endl;
	  DEFAULT();
	  return false;
	}
    }
    break;
    
  case TokenType::INT_LITERAL:
    // Check int range;
    if(node.m_lex.length() > 10){
      RED();
      std::cerr<<"WEEDER ERROR: int "<<node.m_lex<<" out of range.";
      std::cerr<<std::endl;
      DEFAULT();
      return false;
    }

    iss.str(node.m_lex);
    unsigned int large,threshold;
    // Threshold differs for positive and negative numbers
    if(conditions.find(TokenType::T_MINUS)!=conditions.end() &&
       conditions[TokenType::T_MINUS] == 1){
      threshold = 1<<31;
    } else threshold = (1<<31)-1;
			 
    iss>>large;
    if(large > threshold){
      RED();
      std::cerr<<"WEEDER ERROR: int "<<node.m_lex<<" out of range.";
      std::cerr<<std::endl;
      DEFAULT();
      return false;
    }
    break;
    
  case TokenType::UnaryExpression:
    //std::cout<<"UNARY:";
    // If unary expression, add the entry into sub_cond
    if(conditions.find(TokenType::T_MINUS)==conditions.end()){
      //std::cout<<" Add Entry";
      sub_cond[TokenType::T_MINUS] = 0;
    }
    //std::cout<<std::endl;
    break;

  case TokenType::T_PLUS:
    //std::cout<<"T_PLUS"<<std::endl;
    // If this is unary plus, set the flag to true
    if(conditions.find(TokenType::T_MINUS)!= conditions.end()){
      //std::cout<<": UNARY, to true";
      conditions[TokenType::T_MINUS] = 0;
    }
    //std::cout<<std::endl;
    break;
  case TokenType::T_MINUS:
    // If this is unary plus, set the flag to true
    //std::cout<<"T_MINUS:";
    if(conditions.find(TokenType::T_MINUS)!= conditions.end()){
      // std::cout<<" Unary, to false";
      conditions[TokenType::T_MINUS] = 1;
    }
    //std::cout<<std::endl;
    break;
  case TokenType::PrimaryNoNewArray:
    // Check for bracketed components and reset plus minus flag
    if(node.m_generated_tokens.size()>1){
      sub_cond[TokenType::T_MINUS] = 0;
    }
    break;
  case TokenType::CastExpression:
    search_map.clear();
    search_map[TokenType::Assignment] = 1;
    search_map[TokenType::ConditionalAndExpression] = 1;
    search_map[TokenType::ConditionalOrExpression] = 1;
    search_map[TokenType::InclusiveOrExpression] = 1;
    search_map[TokenType::AndExpression] = 1;
    search_map[TokenType::EqualityExpression] = 1;
    search_map[TokenType::RelationalExpression] = 1;
    search_map[TokenType::AssignmentOperator] = 1;
    search_map[TokenType::AdditiveExpression] = 1;
    search_map[TokenType::MultiplicativeExpression] = 1;
    search_map[TokenType::UnaryExpression] = 1;
    search_map[TokenType::PrimaryNoNewArray] = 1;
    search_map[TokenType::ArrayCreationExpression] = 1;
    search_map[TokenType::ClassInstanceCreationExpression] = 1;
    search_map[TokenType::MethodInvocation] = 1;
    search_map[TokenType::ArrayAccess] = 1;


    if(search_any(node.m_generated_tokens[1],search_map)){
      RED();
      std::cerr<<"WEEDER ERROR: cast to invalid type."<<std::endl;
      DEFAULT();
      return false;
    }
    
    break;
  case TokenType::Modifiers:
    // Check if final and abstract is used
    if(node.m_generated_tokens.size()>1){
      std::map<TokenType,int> illegal_modifiers = {{TokenType::T_FINAL, 1},{TokenType::T_ABSTRACT, 1}};
      if(search_all(node.m_generated_tokens[0], illegal_modifiers)) {
        RED();
        std::cerr<<"WEEDER ERROR: cannot have abstract and final modifier!";
        std::cerr<<std::endl;
        DEFAULT();
        return false;
      }
    }
    break;
  case TokenType::AbstractMethodDeclaration:
    search_map.clear();
    search_map[TokenType::T_FINAL] = 1;
    search_map[TokenType::T_STATIC] = 1;
    search_map[TokenType::T_NATIVE] = 1;
    
    if(search_any(node.m_generated_tokens[0],search_map)){
      RED();
      std::cerr<<"WEEDER ERROR: interface method cannot be static, final";
      std::cerr<<", or native."<<std::endl;
      DEFAULT();
      return false;
    }
    break;
  default:
    break;
  }
  //DEFAULT();
  //std::cout<<"---------------------"<<std::endl;
  

  
  
  // Recursive call
  for(Token& t: node.m_generated_tokens){
    if(!weed(t,sub_cond)) return false;
  }
  
  return true;
}

void Weeder::split(const std::string& s, const char delim, std::vector<std::string>& lst){
  std::stringstream ss(s);
  std::string piece;
  while(std::getline(ss,piece,delim)){
    lst.emplace_back(piece);
  }
}

bool Weeder::search_any(Token& node, std::map<TokenType,int>& keys){
  bool result = false;
  if(keys.find(node.m_type)==keys.end()){
    for(Token& t:node.m_generated_tokens){
      result = search_any(t,keys);
      if(result) break;
    }
  }else{
    result = true;
  }
  return result;
}

bool Weeder::search_all(Token&node, std::map<TokenType,int>& keys){
  bool result = true;
  // If found the key and the entry value is larger than 0.
  // Deduct the entry
  if(keys.find(node.m_type)!=keys.end() && keys[node.m_type] > 0){
    keys[node.m_type] --;
  }
  
  // Check if all keys are 0.
  for(std::pair<TokenType,int> kv_pair:keys){
    if(kv_pair.second != 0 ) { result = false; break; }
  }
  // If we have not got result yet, keep searching
  if(!result){
    for(Token& t:node.m_generated_tokens){
      result = search_any(t,keys);
      if(result) break;
    }
  }
  
  return result;
}


bool Weeder::weed(){
  std::map<TokenType,int> cond_table;
  return weed(m_parse_tree,cond_table);
}
