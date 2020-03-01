#include <iostream>
#include "type_linker.h"

#include "color_print.h"


/*
 * Local Helper Functions
 */

Token* GetClassFromEnv(std::string& name,environment ** envs){
  Token *dec = envs[0]->GetClass(name);
  if(dec == nullptr){
    dec = envs[1]->GetClass(name);
    if(dec == nullptr){
      dec = envs[2]->GetClass(name);
      if(dec == nullptr){
	dec = envs[3]->GetClass(name);
	if(dec == nullptr){
	  RED();
	  std::cerr<<"GetClass ERROR: "<<(name)<<" is ambiguous or undefined."<<std::endl;
	  DEFAULT();
	}
      }
    }
  }
  return dec;
}

Token* GetInterfaceFromEnv(std::string& name,environment ** envs){
  Token *dec = envs[0]->GetInterface(name);
  if(dec == nullptr){
    dec = envs[1]->GetInterface(name);
    if(dec == nullptr){
      dec = envs[2]->GetInterface(name);
      if(dec == nullptr){
	dec = envs[3]->GetInterface(name);
	if(dec == nullptr){
	  RED();
	  std::cerr<<"GetInterface ERROR: "<<(name)<<" is ambiguous or undefined."<<std::endl;
	  DEFAULT();
	}
      }
    }
  }
  return dec;
}

Token* GetTypeFromEnv(std::string& name, environment ** envs){
  Token *dec = envs[0]->GetType(name);
  if(dec == nullptr){
    dec = envs[1]->GetType(name);
    if(dec == nullptr){
      dec = envs[2]->GetType(name);
      if(dec == nullptr){
	dec = envs[3]->GetType(name);
	if(dec == nullptr){
	  RED();
	  std::cerr<<"GetType ERROR: "<<(name)<<" is ambiguous or undefined."<<std::endl;
	  DEFAULT();
	}
      }
    }
  }
  return dec;
}

// If ast implements a package, bind it to a package.

bool TypeLinker::ConstructPackage(){
  for(Token* n: m_asts){
    Token* cun = n->SearchByTypeBFS(TokenType::CompilationUnit);
    // search for PackageDeclaration nodes and add to package
    Token* p = n->SearchByTypeBFS(TokenType::PackageDeclaration);
    std::string pname = (p==nullptr)? default_package_name:p->m_generated_tokens[1].m_lex;
    CYAN();
    std::cout<<"ConstructPackage: File "<<cun->m_lex<<" in "<<pname<<std::endl;
    DEFAULT();
    if(p!=nullptr){
      // If cannot add package to package environment, break.
      CYAN();
      std::cout<<"ConstrcutPackage: adding "<<&(cun->scope)<<std::endl; 
      DEFAULT();
      if(!m_packages->AddToPackage(p->m_generated_tokens[1].m_lex,&(cun->scope))){
	RED();
	std::cerr<<"PACKAGE ERROR: Cannot add environment to package ";
	std::cerr<<p->m_generated_tokens[1].m_lex<<std::endl;
	DEFAULT();
	return false;
      }
    } else {
      // else add to default package
      CYAN();
      std::cout<<"ConstrcutPackage: adding "<<&(cun->scope)<<std::endl; 
      DEFAULT();
      if(!m_packages->AddToPackage(default_package_name,&(cun->scope))){
	RED();
	std::cerr<<"PACKAGE ERROR: Cannot add environment to default package."<<std::endl;
	DEFAULT();
	return false;
      }
    }
 
  }
  return true;
}

environment* TypeLinker::GetCurrentPackage(Token* CUN){
  environment* pack_env = nullptr;
  char pack_count = 0;
  std::string pack_name;
  for(Token& c: CUN->m_generated_tokens){
    if(c.type() == TokenType::PackageDeclaration){
      pack_count ++;
      if(pack_count>1){
	RED();
	std::cerr<<"TypeLink ERROR: a file cannot be in two packages.";
	std::cerr<<std::endl;
	DEFAULT();
	break;
      }
      pack_name = c.m_generated_tokens[1].m_lex;
    }
  }

  // IF no package is declared, use the default package name
  if(pack_count == 0){
    pack_name = default_package_name;
  }
  // If duplicated packages.
  if(pack_count > 1) return nullptr;

  // Otherwise try to get package environment.
  // BFS print packages
  /*
  std::vector<Package*> queue;
  queue.emplace_back(m_packages);
  int counter = 1;
  int layer = 0;
  while(queue.size()>0){
    Package* pp = queue[0];
    std::cout<<(pp->package_name)<<"|";
    queue.erase(queue.begin());
    counter --;
    for(std::pair<std::string,Package*> kv_pair:pp->m_sub_packs){
      queue.emplace_back(kv_pair.second);
    }
    if(counter == 0) { layer += 1; counter = queue.size();
      std::cout<<std::endl<<"L"<<layer<<":";
    }
  }
  */
  pack_env = m_packages->GetPack(pack_name);
  if(pack_env == nullptr){
    RED();
    std::cerr<<"Type Link ERROR: Package "<<pack_name<<" is not constructed.";
    std::cerr<<std::endl;
    DEFAULT();
  }
  
  return pack_env;
}

bool TypeLinker::Link(){
  // Construct packages
  if(!ConstructPackage()) return false;
  // Link for each
  for(Token* n: m_asts){
    environment* envs[4];
    environment local_env,single_type,on_demand;
    Token* cun = n->SearchByTypeBFS(TokenType::CompilationUnit);
    // Get the package environment first
    environment* p_env = GetCurrentPackage(n);
    if(p_env == nullptr) return false;
    envs[0] = &local_env;
    envs[1] = &single_type;
    envs[2] = p_env;
    envs[3] = &on_demand;
    for(Token& c: n->m_generated_tokens){
      TokenType node_type = c.type();
      // Search for all import declarations, try to import them to local env
      if(node_type == TokenType::SingleTypeImportDeclaration ||
	 node_type == TokenType::TypeImportOnDemandDeclaration){
	// get the envrionment from the pack
	environment* pack_env;
	bool is_on_demand = false;
	// check if the on demand import 
	if(node_type == TokenType::TypeImportOnDemandDeclaration){
	  is_on_demand = true;
	  pack_env = m_packages->GetAll(c.m_generated_tokens[1].m_lex);
	}else{
	  pack_env = m_packages->Search(c.m_generated_tokens[1].m_lex);
	}
	
	if(pack_env == nullptr){
	  RED();
	  std::cerr<<"Type Linker ERROR: cannot find ";
	  std::cerr<<c.m_generated_tokens[1].m_lex;
	  std::cerr<<std::endl;
	  DEFAULT();
	  return false;
	}
	// Merge the environment with local_env
	bool result;
	if(is_on_demand){
	  on_demand.overwrite_merge(*pack_env);
	  result = true;
	}else{
	  result = single_type.merge(*pack_env);
	}
	// since both get all and search creates a newenvironment on stack
	// delete it
	delete pack_env;

	// If error occurs when merging environment
	if(!result){
	  RED();
	  std::cerr<<"Type Linker ERROR: "<<c.m_generated_tokens[1].m_lex;
	  std::cerr<<" cannot";
	  std::cerr<<" be imported."<<std::endl;
	  DEFAULT();
	  return false;
	}
      }
      // Check if the classes, interfaces in this compilation unit
      // clashes with other components in the package envrionment i'm using
      if(!local_env.merge(cun->scope)){
	RED();
	std::cerr<<"Type Linker ERROR: Compilation Unit "<<n->m_lex;
	std::cerr<<" clashes with package environment."<<std::endl;
	DEFAULT();
	return false;
      }

      // check if any package name is a class name
      if(!m_packages->CheckNames(envs)) return false;
      // resolve types out side of env in the ast
      if(!ResolveAST(n,envs)) return false;
      //if(!ResolveType(CUN,&local_env)) return false;

      //if(!ResolveInheritance(n,envs)) return false;
    }
  }  
}

bool TypeLinker::HasEnv(TokenType t){
  switch(t){
  case TokenType::CompilationUnit:
  case TokenType::ClassDeclaration:
  case TokenType::FieldDeclaration:
  case TokenType::MethodDeclaration:
  case TokenType::ConstructorDeclaration:
  case TokenType::FormalParameter:
  case TokenType::InterfaceDeclaration:
  case TokenType::BlockStatement:
  case TokenType::LocalVariableDeclaration:
  case TokenType::ForInit:
    return true;
  default:
    return false;
  }
  return false;
}

bool TypeLinker::HasEnv(Token* root){
  return HasEnv(root->type());
}



bool TypeLinker::DoLinkType(Token* id, environment** envs){
  std::cout<<"Linking...("<<(*id)<<","<<id->m_lex<<")"<<std::endl;
  Token* dec = nullptr;
  // Handles qualified name
  size_t found = id->m_lex.find('.');
  if(found != std::string::npos){
    dec = m_packages->GetQualified(id->m_lex);
  } else {
    // Handles simple names
    dec = GetTypeFromEnv(id->m_lex,envs);
    if(dec == nullptr) return false;
  }
  id->declaration = dec;
  std::cout<<"Linked"<<std::endl; 
  return true;
}

bool TypeLinker::DoInheritClass(Token* sub, Token* super,std::map<Token*,bool>& duplicate, environment** envs){
  // If no more class to inherit from return true.
  if(super==nullptr) {
    return true;
  }
  // check for cycle
  if(duplicate.find(super) != duplicate.end()){
    RED();
    std::cerr<<"ERROR: inheritance is not acyclic"<<std::endl;
    DEFAULT();
    return false;
  }

  // check for final class
  if(super->type() == TokenType::ClassDeclaration){
    Token* final_node = super->m_generated_tokens[0].SearchByTypeBFS(TokenType::T_FINAL);
    if(final_node == nullptr){
      RED();
      std::cerr<<"ERROR: final class cannot be inherited"<<std::endl;
      DEFAULT();
      return false;
    }
  }

  // Inherit or declare the default java.lang.object;
  
  Token *extend, *implement;
  extend = super->SearchOneChild(TokenType::Super);
  implement = super->SearchOneChild(TokenType::Interfaces); 
  
  //  handle class implements interfaces
  if(!DoInheritInterface(super,implement,duplicate,envs)) return false;
  
  // resolve for extend class
  if(extend != nullptr){
    // try to get the name, it's either qualified or simple
    Token* name_node = extend->SearchByTypeBFS(TokenType::QualifiedName);
    if(name_node == nullptr){
      extend->SearchOneChild(TokenType::T_IDENTIFIER);
    }
    // try to get super class
    Token* super_class = GetClassFromEnv(name_node->m_lex, envs);
    
    // if didn't find the declaration, return false
    if(super_class == nullptr){
      RED();
      std::cerr<<"ERROR:"<<name_node->m_lex<<" is not defined or ambiguous."<<std::endl;
      DEFAULT();
      return false;
    }
    // if the name is not binded return false
    if(name_node->declaration == nullptr){
      RED();
      std::cerr<<"ERROR: "<<name_node->m_lex<<" is not binded during type linking"<<std::endl;
      DEFAULT();
      return false;
    }
    
    // prepare for recursive check
    duplicate[super] = true;
    if(!DoInheritClass(super,super_class,duplicate,envs)) return false;
    // merge super to me
    if(!sub->scope.replace_merge(super->scope)) return false;
  } else {
    DoInheritClass(super,nullptr,duplicate,envs);
  }
  
  return true;
}



bool TypeLinker::DoInheritInterface(Token* sub, Token* interfaces,
				    std::map<Token*,bool>& duplicate,
				    environment** envs)
{
  // If no more class to inherit from return true.
  if(interfaces==nullptr) {
    return true;
  }
  // check for cycle
  if(duplicate.find(interfaces) != duplicate.end()){
    RED();
    std::cerr<<"ERROR: inheritance is not acyclic"<<std::endl;
    DEFAULT();
    return false;
  }

  // Inherit or declare the default java.lang.object;

  // Check Interface Duplication
  std::map<std::string, bool> dup;

  // if it's class inheriting from interfaces, search for interface type list
  Token* interface_lists = interfaces;
  if(interfaces->m_type == TokenType::Interfaces){
    interface_lists = interfaces->SearchOneChild(TokenType::InterfaceTypeList);
  }
  
  //  handle class implements interfaces
  for(Token& t: interface_lists->m_generated_tokens){
    if(t.m_type == TokenType::QualifiedName || t.m_type==TokenType::T_IDENTIFIER){
      // Check for duplicate
      if(dup.find(t.m_lex) == dup.end()){
	dup[t.m_lex] = true;
      } else {
	RED();
	std::cerr<<"Inheritance ERROR: "<<t.m_lex<<" appeared twice."<<std::endl;
	DEFAULT();
	return false;
      }
      
      Token* super_class = GetInterfaceFromEnv(t.m_lex, envs);
      
      // if didn't find the declaration, return false
      if(super_class == nullptr){
	RED();
	std::cerr<<"Interface Inheritance ERROR:"<<t.m_lex<<" is not defined or ambiguous."<<std::endl;
	DEFAULT();
	return false;
      }
      
      // if the name is not binded return false
      if(t.declaration == nullptr){
	RED();
	std::cerr<<"Interface Inheritance ERROR: "<<t.m_lex<<" is not binded during type linking"<<std::endl;
	DEFAULT();
	return false;
      }
      
      // prepare for recursive check
      duplicate[&t] = true;
      Token* implement = t.SearchOneChild(TokenType::ExtendsInterfaces); 
      if(!DoInheritInterface(&t,implement,duplicate,envs)) return false;
      // merge super to me
      if(!sub->scope.replace_merge(t.scope)) return false;
    }
  }
  return true;
}

bool TypeLinker::DoInherit(Token* node, environment** envs){
  Token *extend,*implement,*i_extend;
  extend = node->SearchOneChild(TokenType::Super);
  implement = node->SearchOneChild(TokenType::Interfaces);
  i_extend = node->SearchOneChild(TokenType::ExtendsInterfaces);
  
  std::map<Token*,bool> dup;
  dup[node] = true;
  
  // If class implements interface
  if(node->type() == TokenType::ClassDeclaration){  
    if(!DoInheritClass(node,implement,dup,envs)) return false;
    // try to get the class if it extends
    if(extend != nullptr){
      // try to get the name, it's either qualified or simple
      Token* name_node = extend->SearchByTypeBFS(TokenType::QualifiedName);
      if(name_node == nullptr){
	extend->SearchByTypeBFS(TokenType::T_IDENTIFIER);
      }
      // try to get super class
      Token* super_class = GetClassFromEnv(name_node->m_lex, envs);
      
      // if didn't find the declaration, return false
      if(super_class == nullptr){
	RED();
	std::cerr<<"ERROR:"<<name_node->m_lex<<" is not defined or ambiguous."<<std::endl;
	DEFAULT();
	return false;
      }
      // if the name is not binded return false
      if(name_node->declaration == nullptr){
	RED();
	std::cerr<<"ERROR: "<<name_node->m_lex<<" is not binded during type linking"<<std::endl;
	DEFAULT();
	return false;
      }
      // inherit from super class
      if(!DoInheritClass(node,super_class,dup,envs)) return false;
    } // else inherit from java lang
  } else {
    DoInheritInterface(node,i_extend,dup,envs);
  }
  
  return true;
}


bool TypeLinker::ResolveInheritance(Token* root, environment** envs){
  TokenType t = root->type();
  // Copy the environment to stack
  environment next_local_env;
  next_local_env.merge(*(envs[0]));
  environment* new_envs[4];
  new_envs[0] = &next_local_env;
  new_envs[1] = envs[1];
  new_envs[2] = envs[2];
  new_envs[3] = envs[3];
  
  if(HasEnv(t)){
    switch(t){
    case TokenType::CompilationUnit:
      if(!(new_envs[0]->merge(root->scope))) return false;
      break;
    case TokenType::ClassDeclaration:
    case TokenType::InterfaceDeclaration:
      // Resolve inheritance
      if(!DoInherit(root,new_envs)){
	return false;
      }
      
      // get this scope into then env stack
      if(!new_envs[0]->merge(root->scope)){
	return false;
      }
      break;
      // The follwoing nodes has a scope and does declaration
    default:
      return false;
    }
  }
    
  for(Token& n: root->m_generated_tokens){
    if(!ResolveInheritance(&n,new_envs)) return false;
  }
  
  return true;
}
  
  
bool TypeLinker::ResolveAST(Token* root, environment** envs){
  TokenType t = root->type();
  // Copy the environment to stack
  environment next_local_env;
  next_local_env.merge(*(envs[0]));
  environment* new_envs[4];
  bool checked = false;
  new_envs[0] = &next_local_env;
  new_envs[1] = envs[1];
  new_envs[2] = envs[2];
  new_envs[3] = envs[3];
  if(HasEnv(t)){
    switch(t){
    case TokenType::CompilationUnit:
      if(!new_envs[0]->merge(root->scope)) return false;
      break;

      // for these types, if the third child is id, then it's a class name
      // need to be resolved.
	
    case TokenType::ClassDeclaration:
    case TokenType::InterfaceDeclaration:

      if(root->m_generated_tokens.size() > 0 &&
	 root->m_generated_tokens[2].type() == TokenType::T_IDENTIFIER ||
	 root->m_generated_tokens[2].type() == TokenType::QualifiedName){
	CYAN();
	std::cout<<"Link: "<<(*root)<<std::endl;
	DEFAULT();
	if(!DoLinkType(&(root->m_generated_tokens[2]),new_envs)) return false;
	checked = true;
      }

      // for these types, if the second child is id, then it's a class name
      // need to be resolved.
    case TokenType::Super:
    case TokenType::MethodHeader:
    case TokenType::FieldDeclaration:
    case TokenType::ArrayCreationExpression:
    case TokenType::ClassInstanceCreationExpression:
	
      if(root->m_generated_tokens.size() > 0 && (!checked) &&
	 root->m_generated_tokens[1].type() == TokenType::T_IDENTIFIER ||
	 root->m_generated_tokens[1].type() == TokenType::QualifiedName){
	CYAN();
	std::cout<<"Link: "<<(*root)<<std::endl;
	DEFAULT();
	if(!DoLinkType(&(root->m_generated_tokens[1]),new_envs)) return false;
	checked = true;
      }

      // for these types, if the first child is id, then it's a class name
      // need to be resolved.
    case TokenType::ConstructorDeclarator:
    case TokenType::FormalParameter:
    case TokenType::LocalVariableDeclaration:
      if(root->m_generated_tokens.size() > 0 && (!checked) &&
	 root->m_generated_tokens[0].type() == TokenType::T_IDENTIFIER ||
	 root->m_generated_tokens[0].type() == TokenType::QualifiedName){
	CYAN();
	std::cout<<"Link: "<<(*root)<<std::endl;
	DEFAULT();
	if(!DoLinkType(&(root->m_generated_tokens[0]),new_envs)) return false;
	checked = true;
      }
      break;
      //link the list of interfaces or classes
    case TokenType::InterfaceTypeList:
    case TokenType::ClassTypeList:
    case TokenType::ExtendsInterfaces:
      if(!checked){
	for(Token& child: root->m_generated_tokens){
	  CYAN();
	  std::cout<<"Link: "<<(*root)<<std::endl;
	  DEFAULT();
	  if(child.type() == TokenType::T_IDENTIFIER ||
	     child.type() == TokenType::QualifiedName){
	    
	    if(!DoLinkType(&child,new_envs)) return false;
	    checked = true;
	  }
	}
      }
      break;
      
    default:
      return false;
    }
  }
  
  for(Token& n: root->m_generated_tokens){
    if(!ResolveAST(&n,new_envs)) return false;
  }
  return true;
}



/*
 * Ctor and Dtor
 */

TypeLinker::TypeLinker(const std::vector<Token*>& asts):
  default_package_name("THE DEFAULT PACKAGE"),
  m_asts(asts)
{
  std::string root_pack_name = "THIS IS THE ROOT PACK";
  m_packages = new Package(root_pack_name);
}




TypeLinker::~TypeLinker(){
  delete m_packages;
}
