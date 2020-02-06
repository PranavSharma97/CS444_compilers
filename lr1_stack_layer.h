#pragma once

#include <iostream>
#include "token.h"

class LR1StackLayer{
 public:
  bool no_token;
  int m_state;
  Token m_token;
  LR1StackLayer(int state);
  LR1StackLayer(int state,const Token& token);
  friend std::ostream& operator<<(std::ostream& os, const LR1StackLayer& l);
};
