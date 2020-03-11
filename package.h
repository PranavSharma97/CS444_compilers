#pragma once
#include <string>
#include "environment.h"

class Package{
  bool AddToPackage(std::vector<std::string>& path, environment* src);
  environment* Search(std::vector<std::string>& path);
  Token* GetQualified(std::vector<std::string>& path);
  bool GetAll(std::vector<std::string>& path,environment* dst);
  void MergeAll(environment* dst);
  environment* GetPack(std::vector<std::string>& path);
  bool CheckNames(environment** envs, const std::string& parent_name,Package* root);
 public:
  
  std::map<std::string,Package*> m_sub_packs;
  environment* m_env;
  std::string package_name;
  
  Package();
  Package(const std::string & pack_name);
  Package(const std::string& path, environment* src);
  ~Package();
  bool AddToPackage(const std::string& path, environment* src);
  // search for one class, return it in an environment.
  // return value is newed on heap.
  environment* Search(const std::string& path);
  // Searches for all,creates new environment.
  environment* GetAll(const std::string& path);
  // Check if any of the package name clashes with a class or interface
  // in the given environment.
  bool CheckNames(environment** envs);

  Token* GetQualified(const std::string & name);
  environment* GetPack(const std::string& pack_name);
};
