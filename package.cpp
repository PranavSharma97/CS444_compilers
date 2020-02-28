#include "package.h"
#include "helper_functions.h"

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
  if(path.size() == 1){
    if(m_env == nullptr) m_env = new environment();
    return m_env->merge(src);
  }else{
    Package* new_pack = new Package(v_path[0]);
    m_sub_packs[v_path[0]] = new_pack;
    path.erase(path.begin());
    return new_pack.AddToPackage(path,src);
  }
}


bool Package::AddToPackage(std::string& path, environment* src){
  std::vector<std::string> v_path;
  string_split(path,'.',v_path);

  return AddToPackage(v_path,src);
}

// import pack.pack.class
// adding things to package
environment* Package::Search(std::vector<std::string>& path){
  std::string key = path[0];
  environment* pack = nullptr;
  if(path.size() > 1){
    if(m_sub_packs.find(key) != m_sub_packs.end()){
      path.erase(path.begin());
      pack = m_sub_packs[key].Search(path);
    }
  }else{
    if(m_env->classes.find(key) != m_env->classes.end()){
      pack = new environment();
      pack->classes[key] = m_env->classes[key];
    }else if(m_env->interfaces.find(key) != m_env->interfaces.end()){
      pack = new environment();
      pack->interfaces[key] = m_env->interfaces[key];
    }
  }
  return pack;
}

// import pack.pack.class;
environment* Package::Serach(std::string& path){
  std::vector<std::string> v_path;
  string_split(path,'.',v_path);

  return Search(v_path);
}


// import *
void Package::MergeAll(environment* dst){
  dst->overwrite_merge(m_env);

  for(std::pair<std::string, Package*> &kv_pair: m_sub_packs){
    kv_pair.MergeAll(dst);
  }
}

// import pack.pack.*;
bool Package::GetAll(std::vector<std::string>& path, environment* dst){
  // verifys the last is * or I'm a package (I have sub packs)
  std::string key = path;
  if(path[0] == "*"){
    dst->overwrite_merge(m_env);
    //for(std::pair<std::string, Package*> &kv_pair: m_sub_packs){
      //kv_pair.MergeAll(dst);
      
    //}
    return true;
  } else {
    // Check key
    if(m_sub_packs.find(key) != m_sub_packs.end()){
      path.erase(path.begin());
      return m_sub_packs[key].GetAll(path,dst);
    }
    return false;
  }
}

// import pack.pack.*;
environment* Package::GetAll(std::string& path){
  environment* new_env = new environment();
  std::vector<std::string> v_path;
  string_split(path,'.',v_path);

  if(!GetAll(v_path,new_env)){
    delete new_env;
    return nullptr;
  }
  return new_env;
}

bool Package::CheckNames(environment* env){
  if( env->classes.find(package_name) == env.classes.end() &&
      env->interfaces.find(package_name) == env.classes.end()){
    for(std::pair<std::string,Package*> &kv_pair: m_sub_packs){
      if(!kv_pair.second->CheckNames(env)) return false;
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

ASTNode* Package::GetQualified(std::vector<std::string>& path){
  std::string key = path[0];
  if(path.size() > 1){
    if(m_sub_packs.find(key) != m_sub_packs.end()){
      path.erase(path.begin());
      return m_sub_packs[key].GetQualified(path);
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

ASTNode* Package::GetQualified(std::string& name){
  std::vector<std::string> v_path;
  string_split(path,'.',v_path);

  return GetQualified(v_path);
  
}

environment* Package::GetPack(std::vector<std::string>* path){
  std::string key = path[0];
  if(path.size() > 1){
    if(m_sub_packs.find(key) != m_sub_packs.end()){
      path.erase(path.begin());
      return m_sub_packs[key].GetPack(path);
    }
  }else{
    if(m_sub_packs.find(key) != m_sub_packs.end()){
      return m_sub_packs[key];
    }
  }
  return nullptr;
}

environment* Package::GetPack(std::string& pack_name){
  std::vector<std::string> v_path;
  string_split(path,'.',v_path);

  return Getpack(v_path);
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

Package::Package(std::string& path, environment* src):
  m_env(nullptr),
  package_name("")
{
  std::vector<std::string> v_path;
  string_split(path,'.',v_path);

  
  Package* new_pack = new Package(v_path[0]);
  m_sub_packs[v_path[0]] = new_pack;
  v_path.erase(v_path.begin());
  new_pack.AddToPackage(path,src);
}

Package::~Package(){
  for(std::pair<std::string, Package*> &kv_pair: m_sub_packs){
    delete kv_pair->second;
  }
  if(m_env != nullptr) delete m_env;
}

