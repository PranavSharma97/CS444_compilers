#include <algorithm>
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
	 cout << "Current State: " << currentState << endl;
	 cout << "Transition Character: " << c << endl;
	 int transitionState = transition(transitions, currentState, c);
         cout << "Transition State: " << transitionState << endl;
	 if (transitionState == -1) {
	   int tokenState = lastAcceptingState(seenStates, acceptingStates);
	   cout << "Last accepting state: " << tokenState << endl;
	   if (tokenState != -1) {
	     cout << "Pushing back state: " << tokenState << endl;
	     tokenStates.push_back(tokenState);
	     seenStates = {};
	     acceptingStates = {};
	   }
	   // If no accepting state exists return error
	   else {
	     cout << "ERROR: could not parse Java file" << endl;
	     return -1;
	   }
	 }
	 else {
	   seenStates.push_back(transitionState);
	   currentState = transitionState;
	 }
	 // break; 
      }
      break;
    }
    javaFile.close();
  }
  return 0;
}
