#include <algorithm>
#include <ctype.h>
#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "readers.h"
#include "dfa_states.h"
#include "scanner.h"
#include "debugger.h"

using namespace std;

// Given all transitions, current state, and character return the next state
// If no transition exists returns error
DFAStates transition (map<DFAStates, map<char, DFAStates>> transitions,DFAStates state, char c) {
  map<DFAStates, map<char, DFAStates>>::iterator transition_input_state;

  map<char, DFAStates> state_transitions;
  map<char, DFAStates>::iterator transition_output_state;
 
  transition_input_state = transitions.find(state);

  if (transition_input_state != transitions.end()) {
    state_transitions = transition_input_state->second;
    transition_output_state = state_transitions.find(c);

    if (transition_output_state != state_transitions.end()) {
       return transition_output_state->second;
    }
    else {
      DEBUG_MSG("Could not find transition from " << state << " with " << c);
      return FAILURE;
    }
  }
  else {
    DEBUG_MSG("Could not find input state.");
    return FAILURE;
  }
}

map<string, TokenType> readSpecialWords() {
    map<string, TokenType> specialWordsMap;
    ifstream specialWordsFile;
    specialWordsFile.open("data/scanner/special_words_to_kinds");
    string word;
    int kind;
    while(!specialWordsFile.eof()) {
        specialWordsFile>>word;
        specialWordsFile>>kind;
        specialWordsMap[word] = static_cast<TokenType>(kind);
    }

    return specialWordsMap;
}

TokenType extraScanningLogic(string lexeme) {
    map<string, TokenType> specialWordsMap = readSpecialWords();
    if (specialWordsMap.find(lexeme) != specialWordsMap.end()) {
        return specialWordsMap[lexeme];
    } else {
        return TokenType::T_IDENTIFIER;
    }
}

// Given lastState and acceptingStates return last seen accepting state
// If no acceptingStates exist in lastState return error
TokenType getTokenKind(string lexeme, DFAStates lastState, map<DFAStates, TokenType> acceptingStates) {
  // TODO: not exactly sure if  extraScanningLogic returns state, may need to change
  TokenType type;
  if (lastState == DFAStates::POSSIBLY_IDENTIFIER) {
    type = extraScanningLogic(lexeme);
  }
  else {
    map<DFAStates, TokenType>::iterator it = acceptingStates.find(lastState);
     
    if (it != acceptingStates.end()) {
      type = acceptingStates[lastState];
    }
    else {
      return TOKEN_FAILURE;
    }
  }
  return type;
}

vector<Token> scanner (string filename) {
  DEBUG_MSG("Reading from file: " + filename);
  //vector<pair<TokenType, string>> tokens;
  vector<Token> tokens;
  
  map<DFAStates, map<char, DFAStates>> transitions = DFAReader();

  map<DFAStates, TokenType> acceptingStates = TokenReader();
  DFAStates currentState = START;
  DFAStates lastState = START;
  string lexeme = "";
  
  string line;
  ifstream javaFile (filename);
  if (javaFile.is_open()) {
    while (getline(javaFile, line)) {
      line += " ";
      for (char& c : line) {
	 if (lexeme == "" && isspace(c)) { continue; }

	 if (currentState == SLASH_STAR) {
	   lexeme += c;
	   if (lexeme.substr(lexeme.length() - 2) == "*/") {
             currentState = START;
	     lexeme = "";
	   }
	 }
	 else if (currentState == DOUBLE_SLASH) {
	   currentState = START;
           lastState = START;
	   lexeme = "";
	   break;
	 }
	 else {
	   DEBUG_MSG("");
	   lastState = currentState;
	   DFAStates transitionState = transition(transitions, currentState, c);

	   DEBUG_MSG("Current State: " << currentState);
     DEBUG_MSG("Transition Character: " << c);
     DEBUG_MSG("Transition State: " << transitionState);

	   if (transitionState != FAILURE) {
             currentState = transitionState;
	     DEBUG_MSG("Adding " << c << " to lexeme " << lexeme);
	     lexeme += c;
	   }
	   else {
	     TokenType tokenKind = getTokenKind(lexeme, lastState, acceptingStates);

	     if (tokenKind != TOKEN_FAILURE) {
	       DEBUG_MSG("Pushing back state: " << tokenKind << " for lexeme " << lexeme);
	       //tokens.push_back(make_pair(tokenKind, lexeme));
	       tokens.push_back(Token(tokenKind,lexeme));
				
	       if (isspace(c)) {
	         currentState = START;
	         lexeme = "";
	       }
	       else {
	         currentState = transition(transitions, START, c);
	         lexeme = c;
	       }
	     }
	     else {
	       DEBUG_MSG("ERROR: could not parse Java file");
	       throw logic_error("ERROR: could not parse Java file");
	     }
	   }
	 }
      }
    }

    if (currentState == SLASH_STAR) {
      DEBUG_MSG("ERROR: no accepting state reached at end of Java file");
      throw logic_error("ERROR: no accepting state reached at end of Java file");
    }

    for(Token& t: tokens){
      DEBUG_MSG('['<<t<<","<<t.m_lex<<"] ");
    }
    
    javaFile.close();
  } else {
    throw logic_error("ERROR: cannot find " + filename);
  }
  return tokens;
};
