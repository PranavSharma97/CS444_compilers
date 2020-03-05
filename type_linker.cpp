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
  //std::cout<<"Local"<<std::endl;
  Token *dec = envs[0]->GetType(name);
  if(dec == nullptr){
    //std::cout<<"DirectImport"<<std::endl;
    dec = envs[1]->GetType(name);
    if(dec == nullptr){
      //std::cout<<"Package"<<std::endl;
      dec = envs[2]->GetType(name);
      if(dec == nullptr){
	//std::cout<<"ImportAll"<<std::endl;
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
    // Print The compilation unit package

    PURPLE();
    std::cout<<"CUN:"<<cun->m_lex<<std::endl;
    DEFAULT();
    
    // search for PackageDeclaration nodes and add to package
    Token* p = n->SearchByTypeBFS(TokenType::PackageDeclaration);
    std::string pname = (p==nullptr)? default_package_name:p->m_generated_tokens[1].m_lex;
    
    CYAN();
    std::cout<<"ConstructPackage: File "<<cun->m_lex<<" in "<<pname<<std::endl;
    DEFAULT();
    
    if(p!=nullptr){
      // If cannot add package to package environment, break.
      /*
      CYAN();
      std::cout<<"ConstrcutPackage: adding "<<&(cun->scope)<<std::endl; 
      DEFAULT();
      */
      if(!m_packages->AddToPackage(p->m_generated_tokens[1].m_lex,&(cun->scope))){
	/*
	RED();
	std::cerr<<"PACKAGE ERROR: Cannot add environment to package ";
	std::cerr<<p->m_generated_tokens[1].m_lex<<std::endl;
	DEFAULT();
	*/	
	return false;
      }
    } else {
      // else add to default package
      /*
      CYAN();
      std::cout<<"ConstrcutPackage: adding "<<&(cun->scope)<<std::endl; 
      DEFAULT();
      */
      if(!m_packages->AddToPackage(default_package_name,&(cun->scope))){
	/*
	RED();
	std::cerr<<"PACKAGE ERROR: Cannot add environment to default package."<<std::endl;
	DEFAULT();
	*/	
	return false;
      }
    }
 
  }
  
  CYAN();
  std::cout<<"PACKAGE CONSTRUCTED!"<<std::endl;
  DEFAULT();
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

bool TypeLinker::ResolvePackage(Token* cun,environment** envs){

  // Get the package environment first
    environment* p_env = GetCurrentPackage(cun);
    if(p_env == nullptr) return false;
    envs[2] = p_env;
    //CYAN();
    //std::cout<<"ENVIRONMENT READY"<<std::endl;
    //DEFAULT();
  
  // Check if java.lang is on demanded
  std::string javalang = "java.lang";
  std::map<std::string,bool> imported;
  for(Token& c: cun->m_generated_tokens){
    TokenType node_type = c.m_type;
    // Search for all import declarations, try to import them to local env
    if(node_type == TokenType::SingleTypeImportDeclaration ||
       node_type == TokenType::TypeImportOnDemandDeclaration){
      CYAN();
      std::cout<<"Package Environment Building"<<std::endl;
      DEFAULT();
      std::string pack_name = c.m_generated_tokens[1].m_lex;
      // get the envrionment from the pack
      environment* pack_env;
      bool is_on_demand = false;
      // check if the on demand import 
      if(node_type == TokenType::TypeImportOnDemandDeclaration){
	is_on_demand = true;
	std::cout<<"ON DEMAND PACKAGE"<<std::endl;
	// On demand import should be imported only once
	if(imported.find(pack_name) == imported.end()){
	  pack_env = m_packages->GetAll(pack_name);
	}else continue;
      }else{
	if(imported.find(pack_name) == imported.end()){
	  pack_env = m_packages->Search(pack_name);
	}else continue;
      }
	
      if(pack_env == nullptr){
	RED();
	std::cerr<<"Type Linker ERROR: cannot find ";
	std::cerr<<pack_name;
	std::cerr<<std::endl;
	DEFAULT();
	return false;
      }

      // Mark the class is imported
      imported[pack_name] = true;
      // Merge the environment with corresponding package
      bool result;
      if(is_on_demand){
	envs[3]->overwrite_merge(*pack_env);
	result = true;
      }else{
	result = envs[1]->merge(*pack_env);
      }
      // since both get all and search creates a newenvironment on stack
      // delete it
      delete pack_env;

      // If error occurs when merging environment
      if(!result){
	RED();
	std::cerr<<"Type Linker ERROR: "<<pack_name;
	std::cerr<<" cannot";
	std::cerr<<" be imported."<<std::endl;
	DEFAULT();
	return false;
      }
    }
    // Check if the classes, interfaces in this compilation unit
    // clashes with other components in the package envrionment i'm using
      
    environment temp_env(*envs[1]);
    Token* clash_token;
    // check if single type clashes with local
    if(!temp_env.merge(cun->scope,&clash_token)){
      // Check for self import
      if(!(cun->scope.check_exist(clash_token))){
	RED();
	std::cerr<<"Type Linker ERROR: Compilation Unit "<<cun->m_lex;
	std::cerr<<" clashes with single type import environment."<<std::endl;
	DEFAULT();
	return false;
      }
    }
  }

  // Import java lang if we've not yet
  if(imported.find(javalang) == imported.end()){
    environment* JL = m_packages->GetAll(javalang);
    if(JL == nullptr){
      RED();
      std::cerr<<"Type Linker ERROR: cannot find java.lang";
      std::cerr<<std::endl;
      DEFAULT();
      return false;
    }
    envs[3]->overwrite_merge(*JL);
    delete JL;
  }
  
  std::cout<<std::endl;
  return true;
}

bool TypeLinker::Link(){
  // Construct packages
  if(!ConstructPackage()) return false;
  int file_count = m_asts.size();
  environment local_envs[file_count];
  environment single_types[file_count];
  environment* pack_envs[file_count];
  environment on_demands[file_count];
  int file_index = 0;
  // Link for each
  for(Token* n: m_asts){
    environment* envs[4];
    envs[0] = &(local_envs[file_index]);
    envs[1] = &(single_types[file_index]);
    envs[3] = &(on_demands[file_index]);
    Token* cun = n->SearchByTypeBFS(TokenType::CompilationUnit);
    
    if(!ResolvePackage(cun,envs)) return false;

    pack_envs[file_index] = envs[2];
    file_index ++;
    
    // check if any package name is a class name
    //if(!m_packages->CheckNames(envs)) return false;
    CYAN();
    std::cout<<"PACKAGE NAMES CHECKED"<<std::endl;
    DEFAULT();
    // resolve types out side of env in the ast
    if(!ResolveAST(cun,envs)) return false;
    CYAN();
    std::cout<<"AST Type Linked"<<std::endl;
    DEFAULT();
  }

  file_index = 0;
  for(Token* n: m_asts){
    //if(!ResolveType(CUN,&local_env)) return false;
    environment* envs[4];
    envs[0] = &local_envs[file_index];
    envs[1] = &single_types[file_index];
    envs[2] = pack_envs[file_index];
    envs[3] = &on_demands[file_index];
    file_index ++;
    
    if(!ResolveInheritance(n,envs)) return false;
    CYAN();
    std::cout<<"INHERITANCE Resolved"<<std::endl;
    DEFAULT();
  }
  
  file_index = 0;
  environment* envs[4];
  envs[0] = &local_envs[file_index];
  envs[1] = &single_types[file_index];
  envs[2] = pack_envs[file_index];
  envs[3] = &on_demands[file_index];
  ResolveNameSpaces(m_asts[0],envs);
  /*for(Token* n: m_asts){
    //if(!ResolveType(CUN,&local_env)) return false;
    environment* envs[4];
    envs[0] = &local_envs[file_index];
    envs[1] = &single_types[file_index];
    envs[2] = pack_envs[file_index];
    envs[3] = &on_demands[file_index];
    file_index ++;

    if(!ResolveNameSpaces(n,envs)) return false;
    CYAN();
    std::cout<<"NameSpaces Resolved"<<std::endl;
    DEFAULT();
  }*/
  
  return true;
}

void TypeLinker::set_object_interface(Token* t){
  java_lang_object_interface = t;
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
  //std::cout<<"Linking...("<<(*id)<<","<<id->m_lex<<")"<<std::endl;
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
  //std::cout<<"Linked"<<std::endl; 
  return true;
}

bool TypeLinker::DoInheritClass(Token* sub, Token* super,std::map<Token*,bool>& duplicate, environment** envs){
  CYAN();
  std::cout<<"DO INHERIT CLASS:("<<*sub<<","<<sub->m_generated_tokens[2].m_lex<<")"<<std::endl;
  if(super!=nullptr){
    std::cout<<"From:("<<*super<<","<<super->m_generated_tokens[2].m_lex<<")"<<std::endl;
  }
  DEFAULT();
  // If no more class to inherit from, try to inherit from java.lang.Object
  if(super == nullptr){
    
    CYAN();
    std::cout<<"NO MORE CLASS INHERITANCE"<<std::endl;
    DEFAULT();
    
    duplicate[sub] = true;
    // Try to inherit from java lang
    Token* java_lang_obj = m_packages->GetQualified("java.lang.Object");
    if(java_lang_obj == nullptr){
      RED();
      std::cerr<<"Inheritance ERROR: cannot find java.lang.Object"<<std::endl;
      DEFAULT();
      return false;
    }
    //If cannot inherit
    if(java_lang_obj!= sub &&!DoInheritClass(sub,java_lang_obj,duplicate,envs)){
      RED();
      std::cerr<<"Inheritance ERROR: cannot inherit java.lang.Object"<<std::endl;
      DEFAULT();
      return false;
    }
    sub->Inherited = true;
    return true;
  }
  // If this class is inherited, return ;
  if(sub->Inherited) {
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
    if(final_node != nullptr){
      RED();
      std::cerr<<"ERROR: final class cannot be inherited"<<std::endl;
      DEFAULT();
      return false;
    }
  }

  // Inherit or declare the default java.lang.object;
  
  Token *extend, *implement,*super_class;
  extend = super->SearchOneChild(TokenType::Super);
  implement = super->SearchOneChild(TokenType::Interfaces);

   // resolve for extend class
  if(extend != nullptr){
    // try to get the name, it's either qualified or simple
    Token* name_node = extend->SearchByTypeBFS(TokenType::QualifiedName);
    if(name_node == nullptr){
      name_node=extend->SearchOneChild(TokenType::T_IDENTIFIER);
    }
    CYAN();
    std::cout<<"SUPER INHERITS FROM:"<<name_node->m_lex<<std::endl;
    DEFAULT();
    // try to get super class
    
    if(name_node->m_type == TokenType::T_IDENTIFIER){
      super_class = GetClassFromEnv(name_node->m_lex, envs);
    }else{
      super_class = m_packages->GetQualified(name_node->m_lex);
    }
    
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
    //duplicate[super] = true;
    //if(!DoInheritClass(super,super_class,duplicate,envs)) return false;
    //CYAN();
    //std::cout<<"successfully inherited from"<<name_node->m_lex<<std::endl;
    //DEFAULT();
    // merge super to me
    //if(!sub->scope.replace_merge(super->scope)) return false;
  } else {
    // prepare for recursive check
    CYAN();
    std::cout<<"SUPER CLASS DOES NOT INHERIT FROM ANYTHING"<<std::endl;
    DEFAULT();
    super_class = nullptr;
  }

  // Construct compile time environment for super class
  
  environment* new_envs[4];
  environment local_env,single_type,on_demand;
  if(super_class != nullptr){
    new_envs[0] = &local_env;
    new_envs[1] = &single_type;
    new_envs[3] = &on_demand;
    Token* cun = super_class->compilation_unit;
    if(!ResolvePackage(cun,new_envs)){
      RED();
      std::cerr<<"Inheritance ERROR: cannot construct package environment for";
      std::cerr<<" super class or interface."<<std::endl;
      return false;
      DEFAULT();
    }
  }else{
    new_envs[2] = envs[2];
  }
    
  duplicate[super] = true;
  if(!DoInheritClass(super,super_class,duplicate,new_envs)) return false;
  CYAN();
  std::cout<<"successfully inherited"<<std::endl;
  DEFAULT();
  // merge super to me
  if(!sub->scope.replace_merge(super->scope)) return false;
  
  //  handle class implements interfaces
  
  std::map<Token*,bool> interface_dup;
  if(implement!=nullptr && !DoInheritInterface(super,implement,interface_dup,envs)) return false; 
  
  sub->Inherited = true;
  CYAN();
  std::cout<<"Finished Inheritance"<<std::endl;
  DEFAULT();
  return true;
}



bool TypeLinker::DoInheritInterface(Token* sub, Token* interfaces,
				    std::map<Token*,bool>& duplicate,
				    environment** envs)
{
  CYAN();
  std::cout<<"DO INHERIT INT:("<<*sub<<","<<sub->m_generated_tokens[2].m_lex<<")"<<std::endl;
  if(interfaces!=nullptr){
    std::cout<<"From:"<<*interfaces<<std::endl;
  }
  DEFAULT();
  
  // If no more class to inherit from return true.
  if(interfaces==nullptr) {
    // Try to inherit from java_lang_object_interface
    
    //Token* java_lang_obj = m_packages->GetQualified("java.lang.Object");
    if(sub == java_lang_object_interface ||
       sub->m_type == TokenType::ClassDeclaration){
      sub->Inherited = true;
      return true;
    }
    // If I'm not java_lang_object_interface, get the interface declaration and replace_merge
    Token* the_interface = java_lang_object_interface->SearchByTypeBFS(TokenType::InterfaceDeclaration);
    if(!sub->scope.replace_merge(the_interface->scope)){
      RED();
      std::cerr<<"Interface Inheritance ERROR: cannot get the abstract version of object methods."<<std::endl;
      DEFAULT();
      return false;
    }
    sub->Inherited = true;
    return true;
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
      /*YELLOW();
      std::cout<<t<<","<<t.m_lex<<std::endl;
      std::cout<<envs[2]<<std::endl;
      for(std::pair<std::string,Token*> kv:envs[2]->interfaces){
	std::cout<<kv.first<<","<<kv.second<<","<<*kv.second<<std::endl;
      }
      DEFAULT();*/
      Token* super_class;
      if(t.m_type == TokenType::T_IDENTIFIER){
	super_class = GetInterfaceFromEnv(t.m_lex, envs);
      }else{
	super_class = m_packages->GetQualified(t.m_lex);
      }
      // if didn't find the declaration, return false
      if(super_class == nullptr){
	RED();
	std::cerr<<"Interface Inheritance ERROR:"<<t.m_lex<<" is not defined or ambiguous."<<std::endl;
	DEFAULT();
	return false;
      } else {
	
	// Check for acyclic inheritance
	if(duplicate.find(super_class) != duplicate.end()){
	  RED();
	  std::cerr<<"Interface Inheritance ERROR: inheritance is not acyclic"<<std::endl;
	  DEFAULT();
	  return false;
	}
	// If super interface is inherited continue
      }
      if(super_class->Inherited) continue;
      
      // if the name is not binded return false
      if(t.declaration == nullptr){
	RED();
	std::cerr<<"Interface Inheritance ERROR: "<<t.m_lex<<" is not binded during type linking"<<std::endl;
	DEFAULT();
	return false;
      }

      // Consturct superclass environment
      environment* new_envs[4];
      environment local_env,single_type,on_demand;
      new_envs[0] = &local_env;
      new_envs[1] = &single_type;
      new_envs[3] = &on_demand;
      Token* cun = super_class->compilation_unit;
      if(!ResolvePackage(cun,new_envs)){
	RED();
	std::cerr<<"Inheritance ERROR: cannot construct package environment for";
	std::cerr<<" super class or interface."<<std::endl;
	return false;
	DEFAULT();
      }
      
      // prepare for recursive check
      duplicate[super_class] = true;
      /*YELLOW();
      for(std::pair<Token*,bool> kv:duplicate){
	std::cout<<kv.first<<" ";
      }
      std::cout<<std::endl;
      DEFAULT();*/
      //duplicate[super_class] = true;
      Token* implement = super_class->SearchOneChild(TokenType::ExtendsInterfaces); 
      if(!DoInheritInterface(super_class,implement,duplicate,new_envs)) return false;
      // merge super to me
      if(!sub->scope.replace_merge(super_class->scope)) return false;
    }
  }
  sub->Inherited = true;
  return true;
}

bool TypeLinker::DoInherit(Token* node, environment** envs){
  Token *extend,*implement,*i_extend;
  extend = node->SearchOneChild(TokenType::Super);
  implement = node->SearchOneChild(TokenType::Interfaces);
  i_extend = node->SearchOneChild(TokenType::ExtendsInterfaces);


  CYAN();
  std::cout<<"DO INHERIT:("<<*node<<","<<node->m_generated_tokens[2].m_lex<<")"<<std::endl;
  DEFAULT();
  
  if(node->Inherited) return true;
  
  std::map<Token*,bool> dup;
  std::map<Token*,bool> interface_dup;
  dup[node] = true;
  PURPLE();
  std::cout<<"DO INHERIT: CHECK INHERITANCE OF CLASS, node = "<<node<<std::endl;
  DEFAULT();

  // Construct compile time environment for super class
  environment* new_envs[4];
  environment local_env,single_type,on_demand;
  new_envs[0] = &local_env;
  new_envs[1] = &single_type;
  new_envs[3] = &on_demand;
  Token* cun;
  
  // If class exntends classes
  if(node->type() == TokenType::ClassDeclaration){
    // try to get the class if it extends
    if(extend != nullptr){
      // try to get the name, it's either qualified or simple
      Token* name_node = extend->SearchByTypeBFS(TokenType::QualifiedName);
      
      if(name_node == nullptr){
	name_node = extend->SearchByTypeBFS(TokenType::T_IDENTIFIER);
      }
      std::cout<<"NAME NODE IS:"<<name_node<<std::endl;
      CYAN();
      std::cout<<"INHERIT FROM:"<<name_node->m_lex<<std::endl;
      DEFAULT();
      // try to get super class
      Token* super_class;
      if(name_node->m_type == TokenType::T_IDENTIFIER){
	super_class = GetClassFromEnv(name_node->m_lex, envs);
      }else{
	super_class = m_packages->GetQualified(name_node->m_lex);
      }
      YELLOW();
      std::cout<<"super_calss is:"<<super_class<<std::endl;
      DEFAULT();
      
      // if didn't find the declaration, return false
      if(super_class == nullptr){
	RED();
	std::cerr<<"ERROR:"<<name_node->m_lex<<" is not defined or ambiguous."<<std::endl;
	DEFAULT();
	return false;
      }
      std::cout<<"super class is not null"<<std::endl;
      // if the name is not binded return false
      if(name_node->declaration == nullptr){
	RED();
	std::cerr<<"ERROR: "<<name_node->m_lex<<" is not binded during type linking"<<std::endl;
	DEFAULT();
	return false;
      }
      std::cout<<" name class declaration is not null"<<std::endl;
      // Get the super class environment
      cun = super_class->compilation_unit;
      YELLOW();
      std::cout<<"SUPER CLASS"<<*super_class<<" its CUN is:"<<cun<<std::endl;
      //<<*cun<<","<<cun->m_lex<<std::endl;
      DEFAULT();
      if(!ResolvePackage(cun,new_envs)){
	RED();
	std::cerr<<"Inheritance ERROR: cannot construct package environment for";
	std::cerr<<" super class or interface."<<std::endl;
	return false;
	DEFAULT();
      }
      
      // inherit from super class
      if(!DoInheritClass(node,super_class,dup,new_envs)) return false;
    } else {
      CYAN();
      std::cout<<node->m_generated_tokens[2].m_lex<<" Try to inherit from java lang object"<<std::endl;
      DEFAULT();
      Token* java_lang_obj = m_packages->GetQualified("java.lang.Object");
      if(java_lang_obj == nullptr){
	RED();
	std::cerr<<"Inheritance ERROR: cannot find java.lang.Object"<<std::endl;
	DEFAULT();
	return false;
      }
      if(java_lang_obj != node && !DoInheritClass(node,java_lang_obj,dup,envs)){
	RED();
	std::cerr<<"Inheritance ERROR: cannot inherit java.lang.Object"<<std::endl;
	DEFAULT();
	return false;
      }
    }
    // Inherit from interface
    if(!DoInheritInterface(node,implement,interface_dup,envs)) return false;
  } else {
    if(!DoInheritInterface(node,i_extend,interface_dup,envs)) return false;
  }
  
  // Inherit from java lang object 
  node->Inherited = true;
  CYAN();
  std::cout<<"DO INHERIT FINISHED"<<std::endl;
  DEFAULT();
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
    CYAN();
    std::cout<<"Resolved a class"<<std::endl;
    DEFAULT();
    
    // get this scope into then env stack
    if(!new_envs[0]->merge(root->scope)){
      return false;
    }
    break;
    // The follwoing nodes has a scope and does declaration
  default:
    break;
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

  new_envs[0] = &next_local_env;
  new_envs[1] = envs[1];
  new_envs[2] = envs[2];
  new_envs[3] = envs[3];
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
      //std::cout<<"Link: "<<(*root)<<","<<root->m_lex<<std::endl;
      DEFAULT();
      if(!DoLinkType(&(root->m_generated_tokens[2]),new_envs)) return false;
    }
    break;
    // for these types, if the second child is id, then it's a class name
    // need to be resolved.
  case TokenType::Super:
  case TokenType::MethodHeader:
  case TokenType::FieldDeclaration:
  case TokenType::ArrayCreationExpression:
  case TokenType::ClassInstanceCreationExpression:
	
    if(root->m_generated_tokens.size() > 0 &&
       root->m_generated_tokens[1].type() == TokenType::T_IDENTIFIER ||
       root->m_generated_tokens[1].type() == TokenType::QualifiedName){
      /*CYAN();
      std::cout<<"Link: "<<(*root)<<","<<root->m_lex<<std::endl;
      DEFAULT();
      */      
      if(!DoLinkType(&(root->m_generated_tokens[1]),new_envs)) return false;
    }
    break;
    // for these types, if the first child is id, then it's a class name
    // need to be resolved.
  case TokenType::ConstructorDeclarator:
  case TokenType::FormalParameter:
  case TokenType::LocalVariableDeclaration:
    if(root->m_generated_tokens.size() > 0 &&
       root->m_generated_tokens[0].type() == TokenType::T_IDENTIFIER ||
       root->m_generated_tokens[0].type() == TokenType::QualifiedName){
      /*
      CYAN();
      std::cout<<"Link: "<<(*root)<<std::endl;
      DEFAULT();
      */      
      if(!DoLinkType(&(root->m_generated_tokens[0]),new_envs)) return false;
    }
    break;
    //link the list of interfaces or classes
  case TokenType::InterfaceTypeList:
  case TokenType::ClassTypeList:
  case TokenType::ExtendsInterfaces:
    for(Token& child: root->m_generated_tokens){
      /*
      CYAN();
      std::cout<<"Link: "<<(*root)<<std::endl;
      DEFAULT();
      */      
      if(child.type() == TokenType::T_IDENTIFIER ||
	 child.type() == TokenType::QualifiedName){
	    
	if(!DoLinkType(&child,new_envs)) return false;
      }
    }
    
    break;
      
  default:
    break;
  }
  
  for(Token& n: root->m_generated_tokens){
    if(!ResolveAST(&n,new_envs)) return false;
  }
  return true;
}

bool TypeLinker::ResolveNameSpaces(Token* root, environment** envs){
  TokenType t = root->type();

  envs[0]->merge(root->scope);
  environment next_local_env(*(envs[0]));

  environment* new_envs[4];

  new_envs[0] = &next_local_env;
  new_envs[1] = envs[1];
  new_envs[2] = envs[2];
  new_envs[3] = envs[3];
 
  std::cout << "TYPE OF NODE IS: " << root->m_display_name << std::endl;

  if (t == FieldAccess || t == QualifiedName || 
     (t == MethodInvocation && root->m_generated_tokens[1].m_type == T_DOT)) {
    Token* firstIdentifier = root->SearchByTypeDFS(T_IDENTIFIER);
    std::cout << "Identifier: " << firstIdentifier->m_lex << std::endl;
    Token* declaration = envs[0]->GetDeclaration(firstIdentifier->m_lex);
    if (declaration){
      root->declaration = declaration;
      CYAN();
      std::cout<<"Linking " << root->m_display_name << " to " << declaration->m_display_name << std::endl;
      DEFAULT();
    }
    else{
      CYAN();
      std::cout << "Identifier: " << root->m_display_name << " must be a type, will resolve later?" << std::endl;
      DEFAULT();
    }
  }

  for(Token& n: root->m_generated_tokens){
    ResolveNameSpaces(&n, new_envs);
  }

  return true;
}

bool TypeLinker::ResolveExpressions(Token* root, environment** envs){
  /*TokenType t = root->type();

  environment next_local_env;
  next_local_env.merge(*(envs[0]));

  if (!next_local_env.mergeNames(root->scope)){
    std:cerr << "Shadowing of local variables not allowed" << std::endl;
    return false;
  };

  environment* new_envs[4];

  new_envs[0] = &next_local_env;
  new_envs[1] = envs[1];
  new_envs[2] = envs[2];
  new_envs[3] = envs[3];*/
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
