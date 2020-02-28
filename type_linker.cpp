
#include "type_linker.h"

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
    environment local_env;
    CompilationUnitNode* CUN = (CompilationUnitNode*) n;
    // Get the package environment first
    environment* pack_env = GetCurrentPackage(CUN);
    if(pack_env == nullptr || !local_env.merge(pack_env)) return false;
    
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
	  result = local_env.overwrite_merge(pack_env);
	}else{
	  result = local_env.merge(pack_env);
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
      if(!m_packages.CheckNames(&local_env)) return false;
      // resolve types out side of env in the ast
      if(!ResolveAST(CUN,&local_env)) return false;
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

bool DoLinkClass(IdentifierNode* id, environment* env){
  ASTNode* dec = nullptr;
  // Handles qualified name
  size_t found = id->identifier.find('.');
  if(found != std::string::npos){
    ASTNode* dec = m_packages->GetQualified(id->identifier);
  } else if(env->classes.find(id->identifier) != env->classes.end()){
    ASTNode* dec = env->classes[id->identifier];
  } else {
    // We didn't find it in the current environment
    RED();
    std::cerr<<"ERROR: "<<(ID->identifier)<<" undefined."<<std::endl;
    DEFAULT();
    return false;
  }

  
    // Null pointer means ambiguous. may be a collision in
    // import package.*
  if(dec == nullptr){
    RED();
    std::cerr<<"ERROR: "<<(id->identifier)<<" is ambiguous or undefined."<<std::endl;
    DEFAULT();
    return false;
  }
  id->class_declare = env->classes[id->identifier];
  
  return true;
}

bool TypeLinker::ResolveAST(ASTNode* root, environment* env){
  TokenType t = root->type();
  if(!HasEnv(t)){
    for(ASTNode* n: root->children){
      if(!ResolveAST(n,env)) return false;
    }
    return true;
  } else {
    switch(t){
    case TokenType::CompilationUnit:{
      if(!env->merge(((CompilationUnitNode*)root)->scope)){
	return false;
      }
      break;
    case TokenType::ClassDeclaration:
      // Copy the class environment into the env
      
	// get this scope into then env stack
      if(!env->merge(((ClassDeclarationNode*)root)->scope)){
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
	if(!DoLinkClass(ID,env)) return false;
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
	if(!env->merge(this_env)){
	  return false;
	}
      }
      
      break;
    case TokenType::ClassInstanceCreationExpressionNode:
      if(root->children.size() > 0 &&
	 root->children[1]->type() == TokenType::T_IDENTIFIER){
	IdentifierNode* ID = (IdentifierNode*) root->children[0];
	if(!DoLinkClass(ID,env)) return false;
      }
      break;
    
  default:
    return false;
  }
  }
}

bool TypeLinker::ResolveType(ASTNode* root, envrionment* env){
  /*
  TokenType t_type = root->type();
  switch(t_type){
  case TokenType::MethodInvocation:{
    MethodInvocationNode* MIN = (MethodInvocationNode*) root;
    
  }
  }
  */
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

