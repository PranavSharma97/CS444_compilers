#include <iostream>
#include "type_linker.h"

/*
 * Local Helper Functions
 */

ASTNode* GetClassFromEnv(std::string& name,environments ** envs){
  ASTNode *dec = envs[0]->GetClass(name);
  if(dec == nullptr){
    dec = envs[1]->GetClass(name);
    if(dec == nullptr){
      dec = envs[2]->GetClass(name);
      if(dec == nullptr){
	dec = envs[3]->GetClass(name);
	if(dec == nullptr){
	  RED();
	  std::cerr<<"ERROR: "<<(name)<<" is ambiguous or undefined."<<std::endl;
	  DEFAULT();
	}
      }
    }
  }
  return ASTNode;
}

ASTNode* GetInterfaceFromEnv(std::string& name,environments ** envs){
  ASTNode *dec = envs[0]->GetInterface(name);
  if(dec == nullptr){
    dec = envs[1]->GetInterface(name);
    if(dec == nullptr){
      dec = envs[2]->GetInterface(name);
      if(dec == nullptr){
	dec = envs[3]->GetInterface(name);
	if(dec == nullptr){
	  RED();
	  std::cerr<<"ERROR: "<<(name)<<" is ambiguous or undefined."<<std::endl;
	  DEFAULT();
	}
      }
    }
  }
  return ASTNode;
}

bool CheckDupInterface(std::vector<ASTNode*>& src,std::vector<ASTNode*>& dst){
  std::map<std::string,bool> dup;
  for(ASTNode* n: src){
    if(n->type() == TokenType::Name){
      // if it's not in it, record it and put into dst
      if(dup.find(n->name) == dup.end()){
	dst.emplace_back(n);
	dup[n->name] = true;
      }else{
	RED();
	std::cerr<<"ERROR: Interface "<<n->name<<" is duplicated in inheritance";
	std::cerr<<std::endl;
	DEFAULT();
	return true;
      }
    }
  }
  return true;
}

/*
class TypeLinker{


  std::vector<ASTNode*> m_asts;
  
  void ConstructPackage();
 public:
  TypeLinker(const std::vector<ASTNode*>& asts);
  
  
};
*/


ASTNode* TypeLinker::SearchNodeType(ASTNode* root, TokenType type){
  if(root->type() == type){
    return root;
  }else{
    ASTNode* next = nullptr;
    for(ASTNode* n: root){
      next = SearchNodeType(root);
      if(next != nullptr) return next;
    }
  }
  return nullptr;
}

// If ast implements a package, bind it to a package.

bool TypeLinker::ConstructPackage(){
  for(ASTNode* n: m_asts){
    // search for PackageDeclaration nodes and add to package
    ASTNode* p = SearchNodeType(n,TokenType::PackageDeclaration);
    CompilationUnitNode* CUN = (CompilationUnitNode*) n;
    if(p!=nullptr){
      PackageDeclarationNode* pkg_dcl = (PackgeDeclarationNode*) p;
      // add this package to all of the packages
      
      if(!m_packages->Add(pkg_dcl->package_name,CUN->scope)){
	RED();
	std::cerr<<"PACKAGE ERROR: Cannot add environment to package ";
	std::cerr<<pkg_dcl->package_name<<std::endl;
	DEFAULT();
	return false;
      }
    } else {
      // else add to default package
      if(!m_packages->Add(default_package_name,CUN->scope)){
	RED();
	std::cerr<<"PACKAGE ERROR: Cannot add environment to default package."<<std::endl;
	DEFAULT();
	return false;
      }
    }
      /*
      if(m_packages.find(pkg_dcl->package_name) != m_packages.end()){
	// get the package node
	CompilationUnitNode* CUN = (CompilationUnitNode*) n;
	environment* pkg = m_packages[pkg_dcl->package_name];
	// add all the classes of this ast to the package
	if(!MergeEnvironment(CUN->scope,pkg)){
	  return false;
	}
      }else{
	CompilationUnitNode* CUN = (CompilationUnitNode*) n;
	// create a new package
	environment* pkg = new environment();
	if(!MergeEnvironment(CUN->scope,pkg)){
	  return false;
	}
      }
      */
 
  }
  return true;
}

environment* TypeLinker::GetCurrentPackage(CompilationUnitNode* CUN){
  environment* pack_env = nullptr;
  char pack_count = 0;
  std::string pack_name;
  for(ASTNode* c: CUN->children){
    if(c->type() == TokenType::PackageDeclaration){
      pack_count ++;
      if(duplicate_package){
	RED();
	std::cerr<<"ERROR: a file cannot be in two packages.";
	std::cerr<<endl;
	DEFAULT();
	break;
      }
      pack_name = c->package_name;
    }
  }

  // IF no package is declared, use the default package name
  if(pack_count == 0){
    pack_name = default_package_name;
  }
  // If duplicated packages.
  if(pack_count > 1) return nullptr;

  // Otherwise try to get package environment.
  pack_env = m_packages->GetPack(c->package_name);
  if(pack_env == nullptr){
    RED();
    std::cerr<<"ERROR: Packge "<<c->package_name<<" is not constructed.";
    std::cerr<<endl;
    DEFAULT();
  }
  
  return pack_env;
}

bool TypeLinker::Link(){
  // Construct packages
  if(!ConstructPackage()) return false;
  // Link for each
  for(ASTNode* n: m_asts){
    environment* envs[4];
    environment local_env,single_type,on_demand;
    CompilationUnitNode* CUN = (CompilationUnitNode*) n;
    // Get the package environment first
    environment* p_env = GetCurrentPackage(CUN);
    if(p_env == nullptr) return false;
    envs[0] = &local_env;
    envs[1] = &single_type;
    envs[2] = p_env;
    envs[3] = &on_demand;
    for(ASTNode* c: CUN->children){
      TokenType node_type = c->type();
      // Search for all import declarations, try to import them to local env
      if(node_type == TokenType::ImportDeclaration){
	ImportDeclarationNode* impt_dcl = (ImportDeclarationNode*) c;
	// get the envrionment from the pack
	environment* pack_env;
	bool on_demand = false;
	if(impt_dcl->name[impt_dcl->name.size()-1] == '*'){
	  on_demand = true;
	  pack_env = m_packages->GetAll(impt_dcl->name);
	}else{
	  pack_env = m_packages->Search(impt_dcl->name);
	}
	if(pack_env == nullptr){
	  RED();
	  std::cerr<<"Type Linker ERROR: cannot find "<<impt_dcl->name;
	  std::cerr<<std::endl;
	  DEFAULT();
	  return false;
	}
	// Merge the environment with local_env
	bool result;
	if(on_demand){
	  result = on_demand.overwrite_merge(pack_env);
	}else{
	  result = single_type.merge(pack_env);
	}
	// since both get all and search creates a newenvironment on stack
	// delete it
	delete pack_env;

	// If error occurs when merging environment
	if(!result){
	  RED();
	  std::cerr<<"Type Linker ERROR: "<<impt_dcl->name<<" cannot";
	  std::cerr<<" be imported."<<std::endl;
	  DEFAULT();
	  return false;
	}
      }
      // Check if the classes, interfaces in this compilation unit
      // clashes with other components in the package envrionment i'm using
      if(!local_env.merge(CUN->scope)){
	  RED();
	  std::cerr<<"Type Linker ERROR: Compilation Unit "<<CUN->name;
	  std::cerr<<" clashes with package environment."<<std::endl;
	  DEFAULT();
	  return false;
      }

      // check if any package name is a class name
      if(!m_packages.CheckNames(envs)) return false;
      // resolve types out side of env in the ast
      if(!ResolveAST(CUN,envs)) return false;
      //if(!ResolveType(CUN,&local_env)) return false;
    }
  }  
}

bool TypeLinker::HasEnv(TokenType t){
  switch(t){
  case TokenType::CompilationUnit:
  case TokenType::ClassDeclaration:
  case TokenType::FieldDeclaration:
  case TokenType::MethodDeclaration:
  case TokenType::ConsturctorDeclaration:
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

bool TypeLinker::HasEnv(ASTNode* root){
  return HaasEnv(root->type());
}



ASTNode* Typelinker::GetTypeFromEnv(std::string& name, environments ** envs){
  ASTNode *dec = envs[0]->GetType(name);
  if(dec == nullptr){
    dec = envs[1]->GetType(name);
    if(dec == nullptr){
      dec = envs[2]->GetType(name);
      if(dec == nullptr){
	dec = envs[3]->GetType(name);
	if(dec == nullptr){
	  RED();
	  std::cerr<<"ERROR: "<<(name)<<" is ambiguous or undefined."<<std::endl;
	  DEFAULT();
	}
      }
    }
  }
  return ASTNode;
}


ASTNode* TypeLinker::GetByType(std::vector<ASTNode*>& nodes, TokenType type){
  for(ASTNode* node:nodes){
    if(nodes->type() == type) return nodes;
  }
  return nullptr;
}


bool TypeLinker::DoLinkType(IdentifierNode* id, environment** envs){
  ASTNode* dec = nullptr;
  // Handles qualified name
  size_t found = id->identifier.find('.');
  if(found != std::string::npos){
    dec = m_packages->GetQualified(id->identifier);
  } else {
    // Handles simple names
    dec = GetTypeFromEnv(id->identifier,envs);
    if(dec == nullptr) return false;
  }
  id->class_declare = dec;
  return true;
}

bool TypeLinker::ResolveInheritance(ASTNode* sub,ASTNode* super,std::map<ASTNode*,bool>& duplicate, environment** envs){
  // check for cycle
  if(duplicate.find(super) != duplicate.end()){
    RED();
    std::cerr<<"ERROR: inheritance is not acyclic"<<std::endl;
    DEFAULT();
    return false;
  }

  // check for final class
  if(super.type() == TokenType::ClassDeclaration){
    ASTNode* final_node = GetByType(super->children,TokenType::T_FINAL);
    if(final_node == nullptr){
      RED();
      std::cerr<<"ERROR: final class cannot be inherited"<<std::endl;
      DEFAUT();
      return false;
    }
  }

  // Inherit or declare the default java.lang.object;
  
  ASTNode *extend, *implement;
  extend = GetByType(super->children,TokenType::NameNode);
  implement = GetByType(super->children,TokenType::T_INTERFACE); 

  //  handle class implements interfaces
  if(super->type() == TokenType::ClassDeclaration && implement != nullptr){
    // get all interfaces name nodes 
    std::vector<ASTNode*> all_interface;
    if(!CheckDupInterface(super->children,all_interface)) return false;
    for(ASTNode* name: all_interface){
      NameNode* name_node = (NameNode*) name;
      ASTNode* super_interface = GetInterfaceFromEnv(name_node->name,envs);

      // if didn't find declaration, return false.
      if(super_interface == nullptr){
	RED();
	std::cerr<<"ERROR:"<<name_node->name<<" is not defined or ambiguous."<<std::endl;
	DEFAULT();
	return false;
      }

      // if the name is binded
      if(name_node->declartion != nullptr){
	
	ClassDeclarationNode* cls = (ClassDeclarationNode*) super;
	InterfaceDeclarationNode* sup_cls = (InterfaceDeclarationNode*) super_interface;
	if(!cls->scope->replace_merge(sup_cls->scope)) return false;
      }else {
	// prepare for recursive check
	std::map<ASTNode*,bool> dup;
	dup[super] = true;
	if(!ResolveInheritance(super,super_class,dup,envs)) return false;
	// bind it with name 
	name_node->declaration = super_class;
	return true;
      }
    }
  }

 
  // resolve for extend class
  if(super->type() == TokenType::ClassDeclaration && extend != nullptr){
    NameNode* name_node = (NameNode*) extend;
    // try to get super class
    ASTNode* super_class = GetClassFromEnv(name_node->name, envs);
    
    // if didn't find the declaration, return false
    if(super_class == nullptr){
      RED();
      std::cerr<<"ERROR:"<<name_node->name<<" is not defined or ambiguous."<<std::endl;
      DEFAULT();
      return false;
    }

    // if the name is binded
    if(name_node->declartion != nullptr){
      
      ClassDeclarationNode* cls = (ClassDeclarationNode*) super;
      ClassDeclarationNode* sup_cls = (ClassDeclarationNode*) super_class;
      if(!cls->scope->replace_merge(sup_cls->scope)) return false;
    }else {
      // prepare for recursive check
      std::map<ASTNode*,bool> dup;
      dup[super] = true;
      if(!ResolveInheritance(super,super_class,dup,envs)) return false;
      // bind it with name 
      name_node->declaration = super_class;
      return true;
    }
  }

  
  // if interfaces extends interfaces
  if(super->type() == TokenType::InterfaceDeclarations &&
     i_extend != nullptr){
    // get all interfaces name nodes 
    std::vector<ASTNode*> all_interface;
    if(!CheckDupInterface(super->children,all_interface)) return false;
    for(ASTNode* name: all_interface){
      NameNode* name_node = (NameNode*) name;
      ASTNode* super_interface = GetInterfaceFromEnv(name_node->name,envs);

      // if didn't find declaration, return false.
      if(super_interface == nullptr){
	RED();
	std::cerr<<"ERROR:"<<name_node->name<<" is not defined or ambiguous."<<std::endl;
	DEFAULT();
	return false;
      }

      // if the name is binded
      if(name_node->declartion != nullptr){
	
	InterfaceDeclarationNode* cls = (InterfaceDeclarationNode*) super;
	InterfaceDeclarationNode* sup_cls = (InterfaceDeclarationNode*) super_interface;
	if(!cls->scope->replace_merge(sup_cls->scope)) return false;
      }else {
	// prepare for recursive check
	std::map<ASTNode*,bool> dup;
	dup[super] = true;
	if(!ResolveInheritance(super,super_class,dup,envs)) return false;
	// bind it with name 
	name_node->declaration = super_class;
	return true;
      }
    }
  }

  return true;
}


bool TypeLinker::ResolveInheritance(ASTNode* node, environment** envs){
  ASTNode *extend,*implement,*i_extend;
  extend = GetByType(node->children, TokenType::Name);
  implement = GetByType(node->children, TokenType::T_INTERFACE);
  i_extend = GetByType(node->children,TokenType::ExtendInterfaces);

  
  // Inherite or declare the default java.lang.object;
  
  // Resolve the extend interfaces first
  if(node->type() == TokenType::ClassDeclaration && implement != nullptr){
    // get all interfaces name nodes 
    std::vector<ASTNode*> all_interface;
    if(!CheckDupInterface(node->children,all_interface)) return false;
    for(ASTNode* name: all_interface){
      NameNode* name_node = (NameNode*) name;
      ASTNode* super_interface = GetInterfaceFromEnv(name_node->name,envs);

      // if didn't find declaration, return false.
      if(super_interface == nullptr){
	RED();
	std::cerr<<"ERROR:"<<name_node->name<<" is not defined or ambiguous."<<std::endl;
	DEFAULT();
	return false;
      }

      // if the name is binded
      if(name_node->declartion != nullptr){
	
	ClassDeclarationNode* cls = (ClassDeclarationNode*) node;
	InterfaceDeclarationNode* sup_cls = (InterfaceDeclarationNode*) super_interface;
	if(!cls->scope->replace_merge(sup_cls->scope)) return false;
      }else {
	// prepare for recursive check
	std::map<ASTNode*,bool> dup;
	dup[node] = true;
	if(!ResolveInheritance(node,super_class,dup,envs)) return false;
	// bind it with name 
	name_node->declaration = super_class;
	return true;
      }
    }
  }

  
  // resolve for extend class
  if(node->type() == TokenType::ClassDeclaration && extend != nullptr){
    NameNode* name_node = (NameNode*) extend;
    // try to get super class
    ASTNode* super_class = GetClassFromEnv(name_node->name, envs);
    
    // if didn't find the declaration, return false
    if(super_class == nullptr){
      RED();
      std::cerr<<"ERROR:"<<name_node->name<<" is not defined or ambiguous."<<std::endl;
      DEFAULT();
      return false;
    }

    // if the name is binded
    if(name_node->declartion != nullptr){
      
      ClassDeclarationNode* cls = (ClassDeclarationNode*) node;
      ClassDeclarationNode* sup_cls = (ClassDeclarationNode*) super_class;
      if(!cls->scope->replace_merge(sup_cls->scope)) return false;
    }else {
      // prepare for recursive check
      std::map<ASTNode*,bool> dup;
      dup[node] = true;
      if(!ResolveInheritance(node,super_class,dup,envs)) return false;
      // bind it with name 
      name_node->declaration = super_class;
      return true;
    }
  }
 
  // if interfaces extends interfaces
  if(node->type() == TokenType::InterfaceDeclarations &&
     i_extend != nullptr){
    // get all interfaces name nodes 
    std::vector<ASTNode*> all_interface;
    if(!CheckDupInterface(node->children,all_interface)) return false;
    for(ASTNode* name: all_interface){
      NameNode* name_node = (NameNode*) name;
      ASTNode* super_interface = GetInterfaceFromEnv(name_node->name,envs);

      // if didn't find declaration, return false.
      if(super_interface == nullptr){
	RED();
	std::cerr<<"ERROR:"<<name_node->name<<" is not defined or ambiguous."<<std::endl;
	DEFAULT();
	return false;
      }

      // if the name is binded
      if(name_node->declartion != nullptr){
	
	InterfaceDeclarationNode* cls = (InterfaceDeclarationNode*) node;
	InterfaceDeclarationNode* sup_cls = (InterfaceDeclarationNode*) super_interface;
	if(!cls->scope->replace_merge(sup_cls->scope)) return false;
      }else {
	// prepare for recursive check
	std::map<ASTNode*,bool> dup;
	dup[node] = true;
	if(!ResolveInheritance(node,super_class,dup,envs)) return false;
	// bind it with name 
	name_node->declaration = super_class;
	return true;
      }
    }
  }

  return true;
}

bool TypeLinker::ResolveAST(ASTNode* root, environment** envs){
  TokenType t = root->type();
  if(HasEnv(t)){
    switch(t){
    case TokenType::CompilationUnit:
      if(!envs[0]->merge(((CompilationUnitNode*)root)->scope)) return false;
      break;
    case TokenType::ClassDeclaration:
      // Resolve inheritance
      if(!ResolveInheritance(root,envs)){
	return false;
      }
      
      // get this scope into then env stack
      if(!envs[0]->merge(((ClassDeclarationNode*)root)->scope)){
	return false;
      }
      break;
      // The follwoing nodes has a scope and does declaration
    case TokenType::ConsturctorDeclaration:
      break;
    case TokenType::InterfaceDeclaration:
    case TokenType::BlockStatement:
      
    case TokenType::FieldDeclaration:
    case TokenType::MethodDeclaration:
    case TokenType::LocalVariableDeclaration:
    case TokenType::FormalParameter:
    case TokenType::ForInit:
      // for these types, if the first child is id, then it's a class name
      // need to be resolved.
      if(root->children.size() > 0 &&
	 root->children[0]->type() == TokenType::T_IDENTIFIER){
	IdentifierNode* ID = (IdentifierNode*) root->children[0];
	if(!DoLinkType(ID,envs)) return false;
      }
      
      // add the environment defined here into the environment
      {
	environment* this_env;
	
	switch(t){
	case TokenType::ClassDeclaration:
	  this_env = ((ClassDeclarationNode*) root)->scope;
	  break;
	case TokenType::FieldDeclaration:
	  this_env = ((ClassDeclarationNode*) root)->scope;
	  break;
	case TokenType::MethodDeclaration:
	  this_env = ((ClassDeclarationNode*) root)->scope;
	  break;
	case TokenType::ConsturctorDeclaration:
	  this_env = ((ClassDeclarationNode*) root)->scope;
	  break;
	case TokenType::FormalParameter:
	  this_env = ((ClassDeclarationNode*) root)->scope;
	  break;
	case TokenType::InterfaceDeclaration:
	  this_env = ((ClassDeclarationNode*) root)->scope;
	  break;
	case TokenType::BlockStatement:
	  this_env = ((ClassDeclarationNode*) root)->scope;
	  break;
	case TokenType::LocalVariableDeclaration:
	  this_env = ((ClassDeclarationNode*) root)->scope;
	  break;
	case TokenType::ForInit:
	  this_env = ((ClassDeclarationNode*) root)->scope;
	  break;
	}
	
	// get this scope into then env stack
	if(!envs[0]->merge(this_env)){
	  return false;
	}
      }
      
      break;
    case TokenType::ClassInstanceCreationExpressionNode:
      if(root->children.size() > 0 &&
	 root->children[1]->type() == TokenType::T_IDENTIFIER){
	IdentifierNode* ID = (IdentifierNode*) root->children[0];
	if(!DoLinkType(ID,envs)) return false;
      }
      break;
      
    default:
      return false;
    }
  }
  // Copy the environment to stack
  environment next_local_env;
  next_local_env.merge(envs[0]);
  environment* new_envs[4];
  new_envs[0] = &next_local_env;
  new_envs[1] = envs[1];
  new_envs[2] = envs[2];
  new_envs[3] = envs[3];
  for(ASTNode* n: root->children){
    if(!ResolveAST(n,new_envs)) return false;
  }
  return true;
}



/*
 * Ctor and Dtor
 */

TypeLinker::TypeLinker(const std::vector<ASTNode*>& asts):
  default_package_name("THE DEFAULT PACKAGE"),
  m_asts(asts)
{
  std::string root_pack_name = "THIS IS THE ROOT PACK";
  m_packages = new m_packages(root_pack_name);
}




TypeLinker::~TypeLinker(){
  delete m_packages;
}

