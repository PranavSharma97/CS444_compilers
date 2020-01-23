#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "dfa_states.h"

using namespace std;

int transition (map<int, map<char, int>> transitions, int state, char c) {
  map<int, map<char, int>>::iterator transition_input_state;

  map<char, int> state_transitions;
  map<char, int>::iterator transition_output_state;
 
  transition_input_state = transitions.find(state);

  if (transition_input_state == transitions.end()) {
    cout << "Could not find input state." << endl;
    return -1;
  }
  else { 
    state_transitions = transition_input_state->second;
    transition_output_state = state_transitions.find(c);

    if (transition_output_state == state_transitions.end()) {
       cout << "Could not find transition." << endl;
       return -1;
    }
    else {
      return transition_output_state->second;
    }
  }
}

int main (int argc, char* argv[]) {
  cout << "Reading from file: " << argv[1] << endl;

  vector<pair<string, string>> tokens;

  map<int, map<char, int>> transitions = {
    {START, {{'/', FORWARD_SLASH}}}
  };

  int current_state = START;
  vector<int> seenStates = {};

  string line;
  ifstream javaFile (argv[1]);
  if (javaFile.is_open()) {
    while (getline(javaFile, line)) {
      for (char& c : line) {
	 cout << "Current State: " << current_state << endl;
	 cout << "Transition Character: " << '/' << endl;
	 int transition_state = transition(transitions, current_state, '/');
         cout << "Transition State: " << transition_state << endl;
	 if (transition_state == -1) {
	   cout << "IMPLEMENT FUNCTION TO BACKTRACK" << endl;
	 }
	 else {
	   seenStates.push_back(transition_state);
	 }
	 break; 
      }
      break;
      // cout << line << endl;
    }
    javaFile.close();
  }
  return 0;
}
