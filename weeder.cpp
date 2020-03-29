#include "color_print.h"
#include "helper_functions.h"
#include "weeder.h"

#include <exception>
#include <sstream>
#include <iostream>
#include <utility> // std::pair


void Flatten(Token& node, TokenType victim){
  std::vector<Token> v(node.m_generated_tokens);/*
  std::copy(node.m_generated_tokens.begin(),
	    node.m_generated_tokens.end(),
	    v.begin());
						*/
  for(int i = 0; i<v.size();i++){
    // expand the victim
    if(v[i].m_type == victim){
      int u_size = v[i].m_generated_tokens.size();
      // remove it
      std::vector<Token> u = v[i].m_generated_tokens;
      for(int j = u_size-1;j>=0;j--){
	v.emplace(v.begin()+i,u[j]);
      }
      v.erase(v.begin()+i+u_size);
      // re read
      i--;
    }
  }
  node.m_generated_tokens = v;
}


void Weeder::BuildAST(Token& node){
  bool do_cut = true;
  // 
  while(node.m_generated_tokens.size() == 1){
    switch(node.m_type){
          
    case TokenType::FormalParameterList:
    case TokenType::BlockStatements:
    case TokenType::InterfaceTypeList:
    case TokenType::ExtendsInterfaces:
    case TokenType::ClassTypeList:
    case TokenType::Modifiers:
    case TokenType::ArgumentList:
    case TokenType::Super:
      do_cut = false;
      break;
    case TokenType::CompilationUnit:
      node.m_lex = class_name;
      // unpact all import declarations
      Flatten(node,TokenType::ImportDeclarations);
      do_cut = false;
      break;
      //case TokenType::ForInit:
      //case TokenType::ForUpdate:
    }
    if(do_cut && node.m_generated_tokens.size() == 1){
      //std::cerr<<node<<" -> "<<node.m_generated_tokens[0];
      node = node.m_generated_tokens[0];
      //std::cerr<<" = "<<node<<" | ";
    } else break;
  }

  // handle the ones with multiple children
  switch(node.m_type){    
  case TokenType::FormalParameterList:
  case TokenType::BlockStatements:
  case TokenType::InterfaceTypeList:
  case TokenType::ExtendsInterfaces:
  case TokenType::ClassTypeList:
  case TokenType::ClassBodyDeclarations:
  case TokenType::Modifiers:
    Flatten(node,node.m_type);
    break;
  case TokenType::QualifiedName:
    // convert all name to either qualified name or T_IDENTIFIER
    shrink(node);
      // unpack all T_IDENTIFIER in order
    Flatten(node,TokenType::QualifiedName);
    // get all those names as my m_lex
    node.m_lex = "";
    for(Token& n:node.m_generated_tokens){
      node.m_lex = node.m_lex + n.m_lex; 
    }
    break;
  case TokenType::CompilationUnit:
    // unpact all import declarations
    Flatten(node,TokenType::ImportDeclarations);
    
    //std::cerr<<std::endl;
    break;
  
  default: break;
  }
  //std::cerr<<"Finished:"<<node<<std::endl;
  for(Token& t:node.m_generated_tokens){
    BuildAST(t);
  }
}


Weeder::Weeder(const std::string& file_name, const Token& parse_tree):
  m_parse_tree(parse_tree),
  m_ast_tree(parse_tree)
{
  std::vector<std::string> slash_vec;
  //std::vector<std::string> dot_vec;
  string_split(file_name,'/',slash_vec);
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
    if(search(node.m_generated_tokens[0],TokenType::T_ABSTRACT)){
      std::map<TokenType,int> illegal_modifiers = {{TokenType::T_STATIC,1},{TokenType::T_FINAL,1}};
      if (search_all(node.m_generated_tokens[0],illegal_modifiers)){
        RED();
        std::cerr<<"WEEDER ERROR: an abstract method cannot be static or final!";
        std::cerr<<std::endl;
        DEFAULT();
        return false;
      }
    }
    if(search(node.m_generated_tokens[0],TokenType::T_STATIC) &&
      search(node.m_generated_tokens[0],TokenType::T_FINAL)){
      RED();
      std::cerr<<"WEEDER ERROR: a static method cannot be final!";
      std::cerr<<std::endl;
      DEFAULT();
      return false;
    }
    if(search(node.m_generated_tokens[0],TokenType::T_NATIVE) &&
      !search(node.m_generated_tokens[0],TokenType::T_STATIC)){
      RED();
      std::cerr<<"WEEDER ERROR: a native method must be static!";
      std::cerr<<std::endl;
      DEFAULT();
      return false;
    }
    if(search(node.m_generated_tokens[0],TokenType::T_PRIVATE)){
      RED();
      std::cerr<<"WEEDER ERROR: private methods are not allowed in JOOS!";
      std::cerr<<std::endl;
      DEFAULT();
      return false;
    }
    if(!search(node.m_generated_tokens[0],TokenType::T_PUBLIC) &&
       !search(node.m_generated_tokens[0],TokenType::T_PROTECTED)){
      RED();
      std::cerr<<"WEEDER ERROR: package private methods are not allowed in JOOS!";
      std::cerr<<std::endl;
      DEFAULT();
      return false;
    }
    break;
  case TokenType::ConstructorDeclaration:
    if(!search(node.m_generated_tokens[0],TokenType::T_PUBLIC) &&
       !search(node.m_generated_tokens[0],TokenType::T_PROTECTED)){
      RED();
      std::cerr<<"WEEDER ERROR: package private constructors are not allowed in JOOS!";
      std::cerr<<std::endl;
      DEFAULT();
      return false;
    }
    if(search(node.m_generated_tokens[0],TokenType::T_PRIVATE)){
      RED();
      std::cerr<<"WEEDER ERROR: private constructors are not allowed in JOOS!";
      std::cerr<<std::endl;
      DEFAULT();
      return false;
    }
    break;
  case TokenType::InterfaceDeclaration:
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

    // Search for constructor
    if(!search(node,TokenType::ConstructorDeclaration)){
      RED();
      std::cerr<<"WEEDER ERROR: class need a constructor.";
      std::cerr<<std::endl;
      DEFAULT();
      return false;
    }
    
   
    break;
    
  case TokenType::INT_LITERAL:
    // Check if starts with 0
    if(node.m_lex[0] == '0' && node.m_lex.length()>1){
      CYAN();
      std::cout<<node.m_lex<<std::endl;
      RED();
      std::cerr<<"WEEDER ERROR: int cannot start with 0"<<std::endl;
      DEFAULT();
      return false;
    }
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
    search_map[TokenType::T_PRIVATE] = 1;
    if(search_any(node.m_generated_tokens[0],search_map)){
      RED();
      std::cerr<<"WEEDER ERROR: interface method cannot be static, final";
      std::cerr<<",native or private."<<std::endl;
      DEFAULT();
      return false;
    }

    // If not protected or public, it's a private method
    if(!search(node.m_generated_tokens[0],TokenType::T_PROTECTED) && !(node.m_generated_tokens[0],TokenType::T_PUBLIC)){
      RED();
      std::cerr<<"WEEDER ERROR: interface method cannot be private."<<std::endl;
      DEFAULT();
      return false;
    }
    break;
  case TokenType::FieldDeclaration:
    search_map.clear();
    search_map[TokenType::T_PUBLIC];
    search_map[TokenType::T_PROTECTED];

    // block private field
    if(!search_any(node.m_generated_tokens[0],search_map)){
      RED();
      std::cerr<<"WEEDER ERROR: private field not allowed."<<std::endl;
      DEFAULT();
      return false;
    }
    search_map.clear();
    search_map[TokenType::T_PRIVATE]  = 1;
    search_map[TokenType::T_FINAL] = 1;
    // block final field
    if(search_any(node.m_generated_tokens[0],search_map)){
      RED();
      std::cerr<<"WEEDER ERROR: final or private field not allowed."<<std::endl;
      DEFAULT();
      return false;
    }
    
    // block implicit int constant cast
    if(!search(node.m_generated_tokens[1],TokenType::T_INT) &&
       search(node.m_generated_tokens[3],TokenType::INT_LITERAL)&&
       !search(node.m_generated_tokens[3],TokenType::CastExpression)){
      RED();
      std::cerr<<"WEEDER ERROR: implicit int constant cast not allowed."<<std::endl;
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
  if(weed(m_parse_tree,cond_table)){
    BuildAST(m_ast_tree);
    return true;
  }else return false;
}
