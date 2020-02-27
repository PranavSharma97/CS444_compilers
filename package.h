#pragma once

#include "environment.h"

class Package{
  bool AddToPackage(std::vector<std::string> path, environment* src);
  environment* Search(std::vector<std::string> path);
  bool GetAll(std::vector<std::string> path,environment* dst);
  void MergeAll(environment* dst);
 public:
  
  std::map<std::string,Package*> m_sub_packs;
  environment* m_env;
  std::string package_name;
  
  Package();
  Package(const std::string & pack_name);
  Package(std::string path, environment* src);
  ~Package();
  bool AddToPackage(std::string path, environment* src);
  // search for one class 
  environment* Search(std::string path);
  // Searches for all,creates new environment.
  environment* GetAll(std::string path);
  // Check if any of the package name clashes with a class or interface
  // in the given environment.
  bool CheckNames(environment* env);
};
