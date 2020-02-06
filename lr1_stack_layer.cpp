#include "lr1_stack_layer.h"

/*
 * LR1StackLayer
 */

LR1StackLayer::LR1StackLayer(int state):
  no_token(true),
  m_state(state)
{}


LR1StackLayer::LR1StackLayer(int state,const Token& token):
  no_token(false),
  m_state(state),
  m_token(token)
{}

std::ostream& operator<<(std::ostream& os, const LR1StackLayer& l){
  os<<"(";
  if(!l.no_token){
    os<<l.m_token;
  }
  os<<","<<l.m_state<<")";
  return os;
}
