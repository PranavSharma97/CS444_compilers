#include <algorithm>
#include <ctype.h>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "dfa_states.h"

using namespace std;

// Given all transitions, current state, and character return the next state
// If no transition exists returns error
int transition (map<int, map<char, int>> transitions, int state, char c) {
  map<int, map<char, int>>::iterator transition_input_state;

  map<char, int> state_transitions;
  map<char, int>::iterator transition_output_state;
 
  transition_input_state = transitions.find(state);

  if (transition_input_state != transitions.end()) {
    state_transitions = transition_input_state->second;
    transition_output_state = state_transitions.find(c);

    if (transition_output_state != state_transitions.end()) {
       return transition_output_state->second;
    }
    else {
      cout << "Could not find transition from " << state << " with " << c << endl;
      return -1;
    }
  }
  else {
    cout << "Could not find input state." << endl;
    return -1;
  }
}

int extraScanningLogic(string lexeme) {
  // TODO: Extra scanning logic (keywords)
  int state;
  if (lexeme == "int") {
    state = INTEGER;
  }
  return state;
}

// Given all seenStates and acceptingStates return last seen accepting state
// If no acceptingStates exist in seenStates return error
string getTokenKind(string lexeme, vector<int> seenStates, map<int, string> acceptingStates) {
  // TODO: not exactly sure if  extraScanningLogic returns state, may need to change
  seenStates.push_back(extraScanningLogic(lexeme));

  while (seenStates.size() > 0) {
    int lastState = seenStates.back();

    map<int, string>::iterator it = acceptingStates.find(lastState);
     
    if (it != acceptingStates.end()) {
      return acceptingStates[lastState];
    }
    else {
      seenStates.pop_back();
    }
  }
  return "";
}

int main (int argc, char* argv[]) {
  cout << "Reading from file: " << argv[1] << endl;

  // TODO: Find a way to connect accepting states to token kinds
  vector<pair<string, string>> tokens;

  map<int, map<char, int>> transitions = {
    {START, {{'i', KEYWORD}, {'=', EQUAL}, {'1', INTEGER}, {';', SEMICOLON}, {'/', SLASH}, {' ', START}}},
    {KEYWORD, {{'n', KEYWORD}, {'t', KEYWORD}}},
    {SLASH, {{'*', STAR_SLASH}, {'/', DOUBLE_SLASH}}}
  };

  map<int, string> acceptingStates = {{START, "START"}, {KEYWORD, "KEYWORD"}, {EQUAL, "OPERATOR"}, {INTEGER, "INTEGER"}};

  int currentState = START;
  vector<int> seenStates = {};
  string lexeme = "";

  string line;
  ifstream javaFile (argv[1]);
  if (javaFile.is_open()) {
    while (getline(javaFile, line)) {
      for (char& c : line) {
	 if (currentState == STAR_SLASH) {
	   lexeme += c;
	   if (lexeme.substr(lexeme.length() - 2) == "*/") {
	     currentState = COMMENT;
	   }
	 }
	 else if (currentState == DOUBLE_SLASH) {
	   currentState = START;
           seenStates = {};
	   lexeme = "";
	   break;
	 }
	 else {
	   cout << endl;
	   int transitionState = transition(transitions, currentState, c);

	   cout << "Current State: " << currentState << endl;
           cout << "Transition Character: " << c << endl;
           cout << "Transition State: " << transitionState << endl;

	   if (transitionState != -1) {
	     seenStates.push_back(transitionState);
             currentState = transitionState;
	     cout << "Adding " << c << " to lexeme " << lexeme <<endl;
	     lexeme += c;
	   }
	   else {
	     string tokenKind = getTokenKind(lexeme, seenStates, acceptingStates);

	     if (tokenKind != "") {
	       cout << "Pushing back state: " << tokenKind << " for lexeme " << lexeme <<  endl;
	       tokens.push_back(make_pair(tokenKind, lexeme));

	       if (isspace(c)) {
	         currentState = START;
                 seenStates = {};
	         lexeme = "";
	       }
	       else {
	         currentState = transition(transitions, START, c);
	         seenStates = {currentState};
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

    if (currentState == STAR_SLASH) {
      cout << "ERROR: no accepting state reached at end of Java file" << endl;
      return -1;
    }

    // TODO: Output all tokens
    int tokenSize = tokens.size();

    cout << endl << "Token states: ";
    for (int i=0; i<tokenSize; i++) {
      cout << get<0>(tokens[i]) << " " << get<1>(tokens[i]) << " ";
    }
    cout << endl;

    javaFile.close();
  }
  return 0;
}
