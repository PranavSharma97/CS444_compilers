#include <algorithm>
#include <ctype.h>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "readers.h"
#include "dfa_states.h"

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

// TODO: Extra scanning logic (keywords)
TokenType extraScanningLogic(string lexeme) {
  TokenType type;
  // TODO: Add function
  return type;
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

int outputTokens(vector<pair<TokenType, string>> tokens) {
  int tokenSize = tokens.size();

  cout << endl << "Token states: ";
  for (int i=0; i<tokenSize; i++) {
    cout << get<0>(tokens[i]) << " " << get<1>(tokens[i]) << " ";
  }
  cout << endl;

  return 0;
}

int main (int argc, char* argv[]) {
  cout << "Reading from file: " << argv[1] << endl;

  vector<pair<TokenType, string>> tokens;

  map<DFAStates, map<char, DFAStates>> transitions = DFAReader();

  map<DFAStates, TokenType> acceptingStates = TokenReader();
  /* TODO: CREATE DFA
  map<int, map<char, int>> transitions = {
    {START, {{'i', KEYWORD}, {'=', EQUAL}, {'1', INTEGER}, {';', SEMICOLON}, {'/', SLASH}, {' ', START}}},
    {KEYWORD, {{'n', KEYWORD}, {'t', KEYWORD}}},
    {SLASH, {{'*', STAR_SLASH}, {'/', DOUBLE_SLASH}}}
  };

  // TODO: read in all accepting states and token kinds
  map<int, string> acceptingStates = {{START, "START"}, {KEYWORD, "KEYWORD"}, {EQUAL, "OPERATOR"}, {INTEGER, "INTEGER"}};
	*/

  DFAStates currentState = START;
  DFAStates lastState = START;
  string lexeme = "";

  string line;
  ifstream javaFile (argv[1]);
  if (javaFile.is_open()) {
    while (getline(javaFile, line)) {
      for (char& c : line) {
	 if (currentState == SLASH_STAR) {
	   lexeme += c;
	   if (lexeme.substr(lexeme.length() - 2) == "*/") {
             currentState = DOUBLE_SLASH;
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

	   if (transitionState != -1) {
             currentState = transitionState;
	     cout << "Adding " << c << " to lexeme " << lexeme <<endl;
	     lexeme += c;
	   }
	   else {
	     TokenType tokenKind = getTokenKind(lexeme, lastState, acceptingStates);

	     if (tokenKind != TOKEN_FAILURE) {
	       cout << "Pushing back state: " << tokenKind << " for lexeme " << lexeme <<  endl;
	       tokens.push_back(make_pair(tokenKind, lexeme));

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
	       return -1;
	     }
	   }
	 }
      }
    }

    if (currentState == SLASH_STAR) {
      cout << "ERROR: no accepting state reached at end of Java file" << endl;
      return -1;
    }

    outputTokens(tokens);

    javaFile.close();
  }
  return 0;
}
