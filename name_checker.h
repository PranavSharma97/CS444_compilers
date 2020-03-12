#pragma once
#include "token.h"
#include "package.h"
#include "environment.h"
#include <vector>

class NameChecker{
  
 public:
  Package* m_packages;
  environment* local_envs;
  environment* single_types;
  environment** pack_envs;
  environment* on_demand;

  std::vector<Token *> m_asts;
  
  NameChecker();
  ~NameChecker();
  bool CheckNames();
};
