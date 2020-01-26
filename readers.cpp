#include "readers.h"
#include <iostream>
#include <fstream>
#include <exception>

std::map<DFAStates, std::map<char, DFAStates>> DFAReader(){
  std::map<DFAStates, std::map<char, DFAStates>> DFAMap;
  // Add Start SPACE Start
  std::map<char, DFAStates> entrance_0;
  entrance_0[' '] = DFAStates::START;
  DFAMap[DFAStates::START] = entrance_0;
  entrance_0.clear();

  // Add DOUBLE_QUOTE SPACE DOUBLE_QUOTE
  entrance_0[' ',DFAStates::DOUBLE_QUOTE];
  DFAMap[DFAStates::DOUBLE_QUOTE] = entrance_0;
  entrance_0.clear();
  
  // Add SINGLE_QUOTE SPACE OPEN_CHAR_LITERAL
  entrance_0[' '] = DFAStates::OPEN_CHAR_LITERAL;
  DFAMap[DFAStates::SINGLE_QUOTE] = entrance_0;
  entrance_0.clear();
  
  // Read in states
  // Assume the dfa.in file is in correct form.
  // i.e. int char int for each line
  std::ifstream dfa_input("dfa.in");

  int cur_state,next_state;
  char in_char;
  for(unsigned int line_count = 0;;line_count++){
    // Break if end of line
    if(dfa_input.eof()){
      dfa_input.close();
      break;
    }

    try{
      dfa_input>>cur_state;
      dfa_input>>in_char;
      dfa_input>>next_state;

      std::map<char,DFAStates> entrance;
      DFAStates current = static_cast<DFAStates>(cur_state);
      DFAStates next = static_cast<DFAStates>(next_state);
      entrance[in_char] = next;
      DFAMap[current] = entrance;
    } catch (const std::exception& e){
      std::cerr<<"DFAReader ERROR:"<<e.what()<<" at line: "<<line_count;
      std::cerr<<std::endl;
    }
  }

  return DFAMap;
}

std::map<DFAStates,TokenType> TokenReader(){
  // Read in accepting state to token type
  // Assuming format is correct, i.e. int int
  int dfa_state,token_type;
  std::ifstream token_input("accept_state.in");
  std::map<DFAStates,TokenType> token_map;
  for(unsigned int line_count = 0;; line_count++){
    if(token_input.eof()){
      token_input.close();
      break;
    }

    try{
      token_input>>dfa_state;
      token_input>>token_type;

      DFAStates accept = static_cast<DFAStates>(dfa_state);
      TokenType token = static_cast<TokenType>(token_type);
      token_map[accept] = token;
    }  catch (const std::exception& e){
      std::cerr<<"TokenReader ERROR:"<<e.what()<<" at line: "<<line_count;
      std::cerr<<std::endl;
    }
  }
  return token_map;
  
}
