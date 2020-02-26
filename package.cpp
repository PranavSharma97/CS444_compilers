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


bool Package::AddToPackage(std::vector<std::string> path, environment* src){
  if(path.size() == 1){
    if(m_env == nullptr) m_env = new environment();
    return m_env.merge(src);
  }else{
    Package* new_pack = new Package();
    m_sub_packs[v_path[0]] = new_pack;
    path.erase(path.begin());
    return new_pack.AddToPackage(path,src);
  }
}


bool Package::AddToPackage(std::string path, environment* src){
  std::vector<std::string> v_path;
  string_split(path,'.',v_path);

  return AddToPackage(v_path,src);
}

// import pack.pack.class
// adding things to package
environment* Package::Search(std::vector<std::string> path){
  std::string key = path[0];
  if(path.size() > 1){
    if(m_sub_packs.find(key) != m_sub_packs.end()){
      path.erase(path.begin());
      m_sub_packs[key].find(path);
    }
  }else{
    if(m_env.classes.find(key) != m_env.classes.end()){
      return m_env.classes[key];
    }
  }
  return nullptr;
}

// import pack.pack.class;
environment* Package::Serach(std::string path){
  std::vector<std::string> v_path;
  string_split(path,'.',v_path);

  return Search(v_path);
}


// import *
void Package::MergeAll(environment* dst){
  dst->overwrite_merge(m_env);
  for(std::pair<std::string, Package*> kv_pair: m_sub_packs){
    kv_pair.MergeAll(dst);
  }
}

// import pack.pack.*;
bool Package::GetAll(std::vector<std::string> path, environment* dst){
  // verifys the last is * or I'm a package (I have sub packs)
  std::string key = path;
  if(path[0] == "*"){
    dst->overwrite_merge(m_env);
    for(std::pair<std::string, Package*> kv_pair: m_sub_packs){
      kv_pair.MergeAll(dst);
    }
    return true;
  } else {
    // Check key
    if(m_sub_packs.find(key) != m_sub_packs.end()){
      path.erase(path.begin());
      return m_sub_packs[key].GetAll(path,dst);
    }
  }
}

// import pack.pack.*;
environment* Package::GetAll(std::string path){
  environment* new_env = new environment();
  std::vector<std::string> v_path;
  string_split(path,'.',v_path);

  if(!GetAll(v_path,new_env)){
    delete new_env;
    return nullptr;
  }
  return new_env;
}

/* Ctor Dtor */

Package::Package():
  m_env(nullptr)
{}

Package::Package(std::string path, environment* src):
  m_env(nullptr)
{
  std::vector<std::string> v_path;
  string_split(path,'.',v_path);

  Package* new_pack = new Package();
  m_sub_packs[v_path[0]] = new_pack;
  v_path.erase(v_path.begin());
  new_pack.AddToPackage(path,src);
}

Package::~Package(){
  for(std::pair<std::string, Package*> kv_pair: m_sub_packs){
    delete kv_pair->second;
  }
  if(m_env != nullptr) delete m_env;
}
