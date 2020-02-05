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
      cout << "Could not find transition from " << state << " with " << c << endl;
      return FAILURE;
    }
  }
  else {
    cout << "Could not find input state." << endl;
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
  cout << "Reading from file: " << filename << endl;

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
	   cout << endl;
	   lastState = currentState;
	   DFAStates transitionState = transition(transitions, currentState, c);

	   cout << "Current State: " << currentState << endl;
           cout << "Transition Character: " << c << endl;
           cout << "Transition State: " << transitionState << endl;

	   if (transitionState != FAILURE) {
             currentState = transitionState;
	     cout << "Adding " << c << " to lexeme " << lexeme <<endl;
	     lexeme += c;
	   }
	   else {
	     TokenType tokenKind = getTokenKind(lexeme, lastState, acceptingStates);

	     if (tokenKind != TOKEN_FAILURE) {
	       cout << "Pushing back state: " << tokenKind << " for lexeme " << lexeme <<  endl;
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
	       cout << "ERROR: could not parse Java file" << endl;
	       throw logic_error("ERROR: could not parse Java file");
	     }
	   }
	 }
      }
    }

    if (currentState == SLASH_STAR) {
      cout << "ERROR: no accepting state reached at end of Java file" << endl;
      throw logic_error("ERROR: no accepting state reached at end of Java file");
    }

    for(Token& t: tokens){
      cout<<'['<<t<<","<<t.m_lex<<"] ";
    }
    cout<<endl;
    
    javaFile.close();
  }
  return tokens;
};
