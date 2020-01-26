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
      cout << "Could not find transition." << endl;
      return -1;
    }
  }
  else {
    cout << "Could not find input state." << endl;
    return -1;
  }
}

// Given all seenStates and acceptingStates return last seen accepting state
// If no acceptingStates exist in seenStates return error
int lastAcceptingState(vector<int> seenStates, vector<int> acceptingStates) {
  while (seenStates.size() > 0) {
    int lastState = seenStates.back();
    vector<int>::iterator it;

    it = find(acceptingStates.begin(), acceptingStates.end(), lastState);
     
    if (it != acceptingStates.end()) {
      return * it;
    }
    else {
      seenStates.pop_back();
    }
  }
  return -1;
}

int main (int argc, char* argv[]) {
  cout << "Reading from file: " << argv[1] << endl;

  // TODO: Find a way to connect accepting states to token kinds
  vector<pair<string, string>> tokens;

  // List of accepting states
  vector<int> tokenStates;

  map<int, map<char, int>> transitions = {
    {START, {{'/', FORWARD_SLASH}}},
    {FORWARD_SLASH, {{'/', COMMENT}}}
  };

  vector<int> acceptingStates = {START, COMMENT};

  int currentState = START;
  vector<int> seenStates = {};

  string line;
  ifstream javaFile (argv[1]);
  if (javaFile.is_open()) {
    while (getline(javaFile, line)) {
      for (char& c : line) {
	 int transitionState = transition(transitions, currentState, c);
	 cout << "Current State: " << currentState << endl;
         cout << "Transition Character: " << c << endl;
         cout << "Transition State: " << transitionState << endl;

	 if (transitionState != -1) {
	   seenStates.push_back(transitionState);
           currentState = transitionState;
	 }
	 else {
	   int acceptState = lastAcceptingState(seenStates, acceptingStates);

	   if (acceptState != -1) {
	     cout << "Pushing back state: " << acceptState << endl;
	     tokenStates.push_back(acceptState);

	     if (isspace(c)) {
	       currentState = START;
               seenStates = {};
	     }
	     else {
	       currentState = transition(transitions, START, c);	     
	       seenStates = {currentState};
	     }
	   }
	   else {
	     cout << "ERROR: could not parse Java file" << endl;
	     return -1;
	   }
	 }
	 // break; 
      }
      break;
    }
    javaFile.close();
  }
  return 0;
}
