#pragma once
#include <map>

#include "dfa_states.h"
#include "token_types.h"

std::map<DFAStates, std::map<char, DFAStates>> DFAReader();

std::map<DFAStates,TokenType> TokenReader();

