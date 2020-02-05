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

  // Add Start TAB/newline Start
  DFAMap[DFAStates::START]['\t'] = DFAStates::START;
  DFAMap[DFAStates::START]['\n'] = DFAStates::START;
  DFAMap[DFAStates::START]['\v'] = DFAStates::START;
  
  // Add DOUBLE_QUOTE SPACE DOUBLE_QUOTE
  entrance_0[' '] = DFAStates::DOUBLE_QUOTE;
  DFAMap[DFAStates::DOUBLE_QUOTE] = entrance_0;
  entrance_0.clear();
  
  // Add SINGLE_QUOTE SPACE OPEN_CHAR_LITERAL
  entrance_0[' '] = DFAStates::OPEN_CHAR_LITERAL;
  DFAMap[DFAStates::SINGLE_QUOTE] = entrance_0;
  entrance_0.clear();

  // Add Start to identifiers
  // Anything Starts with $,_,letters is legal
  for(char c = '$'; c <= 'z'; c++){
    if((c>='A' && c<='Z')||(c>='a' && c<= 'z')||(c=='$')||(c=='_')){
      DFAMap[DFAStates::START][c] = DFAStates::POSSIBLY_IDENTIFIER;
      // Add possibly_identifier to possibly_identifier
      if(c == '$'){
	entrance_0['$'] = DFAStates::POSSIBLY_IDENTIFIER;
	DFAMap[DFAStates::POSSIBLY_IDENTIFIER] = entrance_0;
	entrance_0.clear();
      }
      DFAMap[DFAStates::POSSIBLY_IDENTIFIER][c] = DFAStates::POSSIBLY_IDENTIFIER;
    }
    
  }

  // Added numbers to possibly identifiers
  for(char c = '0'; c<='9'; c++){
    DFAMap[DFAStates::POSSIBLY_IDENTIFIER][c] = DFAStates::POSSIBLY_IDENTIFIER;
  }
  
  
  // Read in states
  // Assume the dfa.in file is in correct form.
  // i.e. int char int for each line
  std::ifstream dfa_input("data/scanner/dfa.in");

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
      DFAStates current = static_cast<DFAStates>(cur_state);
      DFAStates next = static_cast<DFAStates>(next_state);

      // Check if key exists
      if(DFAMap.find(current) == DFAMap.end()){
	std::map<char,DFAStates> entrance;
	entrance[in_char] = next;
	DFAMap[current] = entrance;
      }else{
	DFAMap[current][in_char] = next;
      }
    
    } catch (const std::exception& e){
      dfa_input.close();
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
  std::ifstream token_input("data/scanner/accept_state.in");
  std::map<DFAStates,TokenType> token_map;

  for(unsigned int line_count = 0;; line_count++){
    // Break when end of line
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
      token_input.close();
      std::cerr<<"TokenReader ERROR:"<<e.what()<<" at line: "<<line_count;
      std::cerr<<std::endl;
    }
  }
  return token_map;
}
