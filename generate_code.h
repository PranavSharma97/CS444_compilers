#include <vector>
#include "package.h"
#include "token.h"

class GenerateCode {
  std::vector<Token*> tree_ptrs;
  void generateToken(std::ofstream &file, Token *t);
  void pushArguments(std::ofstream &file, Token *argumentList);
  public:
  int Generate();
  GenerateCode(const std::vector<Token*>& tree_ptrs);
  ~GenerateCode();
};
