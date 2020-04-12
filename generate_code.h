#include <vector>
#include "package.h"
#include "token.h"

class GenerateCode {
  std::vector<Token*> tree_ptrs;
  // After generateToken is finished, what code is generated
  // It's result must be in ebx.
  void generateToken(std::ofstream &file, Token *t);
  void pushArguments(std::ofstream &file, Token *argumentList);
  public:
  int Generate();
  GenerateCode(const std::vector<Token*>& tree_ptrs);
  ~GenerateCode();
};
