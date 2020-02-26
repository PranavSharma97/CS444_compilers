
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

bool TypeLinker::MergeEnvironment(environment* src, environment* dst){
  // Merge classes
  for(std::pair<std::string, ClassDeclarationNode*> kv_pair: dst->classes){
    if(src->classes.find(kv_pair.first) == src->classes.end()){
      src->classes[kv_pair.first] = kv_pair.second;
    }else{
      RED();
      std::cerr<<"TYPE LINKER ERROR: Class: "<<kv_pair.first;
      std::cerr<<" already defined"<<std::endl;
      DEFAULT();
      return false;
    }
  }
  // Merge interfaces
  for(std::pair<std::string, InterfaceDeclarationNode*> kv_pair: dst->interfaces){
    if(src->interfaces.find(kv_pair.first) == src->interfaces.end()){
      src->interfaces[kv_pair.first] = kv_pair.second;
    }else{
      RED();
      std::cerr<<"TYPE LINKER ERROR: Interface: "<<kv_pair.first;
      std::cerr<<" already defined"<<std::endl;
      DEFAULT();
      return false;
    }
  }
  // Merge fields
  for(std::pair<std::string, FieldDeclarationNode*> kv_pair: dst->fields){
    if(src->fields.find(kv_pair.first) == src->fields.end()){
      src->fields[kv_pair.first] = kv_pair.second;
    }else{
      RED();
      std::cerr<<"TYPE LINKER ERROR: field: "<<kv_pair.first;
      std::cerr<<" already declared"<<std::endl;
      DEFAULT();
      return false;
    }
  }
  // Merge methods, current doesn't check the methods with same parameters
  for(std::pair<std::string, FieldDeclarationNode*> kv_pair: dst->fields){
    if(true){
      for(MethodDeclarationNode* n: kv_pair.second){
	src->fields[kv_pair.first].emplace_back(n);
      }
    }else{
      RED();
      std::cerr<<"TYPE LINKER ERROR: method: "<<kv_pair.first;
      std::cerr<<" already declared"<<std::endl;
      DEFAULT();
      return false;
    }
  }
  
  // Merge localVariables
  for(std::pair<std::string, LocalVariableDeclarationNode*> kv_pair: dst->localVariables){
    if(src->localVariables.find(kv_pair.first) == src->localVariables.end()){
      src->localVariables[kv_pair.first] = kv_pair.second;
    }else{
      RED();
      std::cerr<<"TYPE LINKER ERROR: variable: "<<kv_pair.first;
      std::cerr<<" already declared"<<std::endl;
      DEFAULT();
      return false;
    }
  }
  // Merge formal Parameters
  for(std::pair<std::string, FieldDeclarationNode*> kv_pair: dst->formalParameters){
    if(src->formalParameters.find(kv_pair.first) == src->formalParameters.end()){
      src->formalParameters[kv_pair.first] = kv_pair.second;
    }else{
      RED();
      std::cerr<<"TYPE LINKER ERROR: parameter: "<<kv_pair.first;
      std::cerr<<" duplicated."<<std::endl;
      DEFAULT();
      return false;
    }
  }
}

// If ast implements a package, bind it to a package.

bool TypeLinker::ConstructPackage(){
  for(ASTNode* n: m_asts){
    // search for PackageDeclaration nodes and add to package
    ASTNode* p = SearchNodeType(n,TokenType::PackageDeclaration);
    if(p!=nullptr){
      PackageDeclarationNode* pkg_dcl = (PackgeDeclarationNode*) p;
      // add this package to all of the packages
      
      CompilationUnitNode* CUN = (CompilationUnitNode*) n;
      if(!m_packages->Add(pkg_dcl->package_name,CUN->scope)){
	RED();
	std::cerr<<"PACKAGE ERROR: Cannot add environment to package ";
	std::cerr<<pkg_dcl->package_name<<std::endl;
	DEFAULT();
	return false;
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
  }
  return true;
}

/*
 * Ctor and Dtor
 */

TypeLinker::TypeLinker(const std::vector<ASTNode*>& asts):
  m_asts(asts)
{
  m_packages = new m_packages();
}

bool TypeLinker::Link(){
  // Construct packages
  if(!ConstructPackage()) return false;
  // Link for each
  for(ASTNode* n: m_asts){
    environment local_env;
    CompilationUnitNode* CUN = (CompilationUnitNode*) n;
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
      // resolve types
    }
  }
  
}


TypeLinker::~TypeLinker(){
  delete m_packages;
}

