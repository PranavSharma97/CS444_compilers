#include "package.h"
#include "helper_functions.h"
#include "color_print.h"
#include "token.h"

#include <iostream>
/*
class Package{
 public:
  std::map<std::string,Package*> m_sub_packs;
  environment* m_env;
  Package();
  Package(std::string path, environment* src);
  bool AddToPackage(std::string path, environment* src);
  // search for one class 
  environment* Search(std::string path);
  // Searches for all,creates new environment.
  environment* GetAll(std::string path);
};
*/


bool Package::AddToPackage(std::vector<std::string>& path, environment* src){

  std::string key = path[0];
  // Check if prefix can be resolved to class
  if(m_env != nullptr && m_env->GetType(key)!=nullptr){
    RED();
    std::cerr<<"Package Name ERROR: prefix"<<key<<" resolved to type"<<std::endl;
    DEFAULT();
    return false;
  }
  path.erase(path.begin());
  PURPLE();
  std::cout<<package_name<<" Add "<<key<<std::endl;
  PURPLE();
  if(m_sub_packs.find(key) == m_sub_packs.end()){
    // create sub package
    Package* new_pack = new Package(key);
    m_sub_packs[key] = new_pack;
    if(path.size()>0) return new_pack->AddToPackage(path,src);
    new_pack->package_name = key;
    new_pack->m_env = new environment();
    
    new_pack->m_env->merge(*src);
    return true;
  }else{
    //if this is the package
    if(path.size()==0) {
      if(m_sub_packs[key]->m_env==nullptr) {
	
	m_sub_packs[key]->m_env = new environment();
	m_sub_packs[key]->m_env->merge(*src);
	return true;
      }
      return m_sub_packs[key]->m_env->merge(*src);
    }
    return m_sub_packs[key]->AddToPackage(path,src);
  }
}


bool Package::AddToPackage(const std::string& path, environment* src){
  std::vector<std::string> v_path;
  string_split(path,'.',v_path);

  return AddToPackage(v_path,src);
}

// import pack.pack.class
// adding things to package
environment* Package::Search(std::vector<std::string>& path){
  std::string key = path[0];
  /*
  PURPLE();
  std::cout<<"SEARCH "<<key<<std::endl;
  DEFAULT();
  */
  environment* pack = nullptr;
  if(path.size() > 1){
    if(m_sub_packs.find(key) != m_sub_packs.end()){
      path.erase(path.begin());
      pack = m_sub_packs[key]->Search(path);
    }
  }else{
    CYAN();
    std::cout<<"KEY:"<<key<<", env:"<<m_env<<std::endl;
    DEFAULT();
    Token* t= m_env->GetType(key);
    if(t == nullptr){
      RED();
      std::cerr<<"Package Search ERROR: cannot find "<<key<<" in "<<package_name;
      std::cerr<<std::endl;
      DEFAULT();
      return nullptr;
    }
    pack = new environment();
    if(t->m_type == TokenType::ClassDeclaration){
      pack->classes[key] = t;
    }else{
      pack->interfaces[key] = t;
    }
  }
  return pack;
}

// import pack.pack.class;
environment* Package::Search(const std::string& path){
  std::vector<std::string> v_path;
  string_split(path,'.',v_path);

  return Search(v_path);
}


// import *
void Package::MergeAll(environment* dst){
  dst->overwrite_merge(*m_env);

  for(std::pair<std::string, Package*> kv_pair: m_sub_packs){
    kv_pair.second->MergeAll(dst);
  }
}

// import pack.pack.*;
bool Package::GetAll(std::vector<std::string>& path, environment* dst){

  std::string key = path[0];
  path.erase(path.begin());
  PURPLE();
  std::cout<<"ON DEMAND: pack = "<<package_name<<" key = "<<key<<std::endl;
  DEFAULT();
  if(path.size()==0){
    std::cout<<"CHECKED"<<std::endl;
    if(m_sub_packs.find(key)!= m_sub_packs.end()){
      environment* src_env = m_sub_packs[key]->m_env;
      if(src_env == nullptr) return true;
      dst->overwrite_merge(*src_env);
      return true;
    }
    return false;
  } else {
    // Check key
    if(m_sub_packs.find(key) != m_sub_packs.end()){
      std::cout<<"RECURSE"<<std::endl;
      return m_sub_packs[key]->GetAll(path,dst);
    }
    return false;
  }
}

// import pack.pack.*;
environment* Package::GetAll(const std::string& path){
  environment* new_env = new environment();
  std::vector<std::string> v_path;
  string_split(path,'.',v_path);
  PURPLE();
  std::cout<<"On Demand Import:"<<path<<std::endl;
  for(std::string& s:v_path){
    std::cout<<s<<" ";
  }
  std::cout<<std::endl;
  DEFAULT();
  if(!GetAll(v_path,new_env)){
    delete new_env;
    return nullptr;
  }
  return new_env;
}

bool Package::CheckNames(environment** envs){
  if( envs[0]->classes.find(package_name) == envs[0]->classes.end() &&
      envs[0]->interfaces.find(package_name) == envs[0]->interfaces.end() &&
      envs[1]->classes.find(package_name) == envs[1]->classes.end() &&
      envs[1]->interfaces.find(package_name) == envs[1]->interfaces.end()&&
      envs[3]->classes.find(package_name) == envs[3]->classes.end() &&
      envs[3]->interfaces.find(package_name) == envs[3]->interfaces.end()){
    for(std::pair<std::string,Package*> kv_pair: m_sub_packs){
      if(!kv_pair.second->CheckNames(envs)) return false;
    }
    return true;
  }else {
    RED();
    std::cerr<<"ERROR: Package "<<package_name<<" clashes with a class";
    std::cerr<<" or an interface name"<<std::endl;
    DEFAULT();
    return false;
  }
}

Token* Package::GetQualified(std::vector<std::string>& path){
  std::string key = path[0];
  if(path.size() > 1){
    if(m_sub_packs.find(key) != m_sub_packs.end()){
      path.erase(path.begin());
      return m_sub_packs[key]->GetQualified(path);
    }
  }else{
    if(m_env->classes.find(key) != m_env->classes.end()){
      return m_env->classes[key];
    }else if(m_env->interfaces.find(key) != m_env->interfaces.end()){
      return m_env->interfaces[key];
    }
  }
  return nullptr;
}

Token* Package::GetQualified(const std::string& name){
  std::vector<std::string> v_path;
  string_split(name,'.',v_path);

  return GetQualified(v_path);
  
}

environment* Package::GetPack(std::vector<std::string>& path){
  if(path.size() > 0){
    std::string key = path[0];
    //std::cout<<"GET PACK:"<<key<<std::endl; 
    std::vector<std::string> new_path;
    for(int i = 1; i<path.size();i++) { new_path.emplace_back(path[i]); }

    if(m_sub_packs.find(key) != m_sub_packs.end()){
      return m_sub_packs[key]->GetPack(new_path);
    }else {
      return nullptr;
    }
  } else {
    /*
    for(std::pair<std::string,Token*> kv_pair:m_env->classes){
    std::cout<<kv_pair.first<<","<<((kv_pair.second)->m_display_name)<<std::endl;
    }*/
    return m_env;
  }
}

environment* Package::GetPack(const std::string& pack_name){
  std::vector<std::string> v_path;
  string_split(pack_name,'.',v_path);

  return GetPack(v_path);
}

/* Ctor Dtor */

Package::Package():
  m_env(nullptr),
  package_name("")
{}

Package::Package(const std::string& pack_name):
  m_env(nullptr),
  package_name(pack_name)
{}

Package::Package(const std::string& path, environment* src):
  m_env(nullptr),
  package_name("")
{
  std::vector<std::string> v_path;
  string_split(path,'.',v_path);

  
  Package* new_pack = new Package(v_path[0]);
  m_sub_packs[v_path[0]] = new_pack;
  v_path.erase(v_path.begin());
  new_pack->AddToPackage(path,src);
}

Package::~Package(){
  for(std::pair<std::string, Package*> kv_pair: m_sub_packs){
    delete kv_pair.second;
  }
  if(m_env != nullptr) delete m_env;
}

