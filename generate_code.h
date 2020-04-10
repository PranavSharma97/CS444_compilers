#include <vector>
#include "package.h"
#include "token.h"

class GenerateCode {
  std::vector<Token*> tree_ptrs;
  public:
  int Generate();
  GenerateCode(const std::vector<Token*>& tree_ptrs);
  ~GenerateCode();
};
