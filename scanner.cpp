#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
using namespace std;

string transition (map<string, map<char, string>> transitions, string state, char c) {
  map<string, map<char, string>>::iterator transition_input_state;

  map<char, string> state_transitions;
  map<char, string>::iterator transition_output_state;
 
  transition_input_state = transitions.find(state);

  if (transition_input_state == transitions.end()) {
    return "Could not find input state.";
  }
  else { 
    state_transitions = transition_input_state->second;
    transition_output_state = state_transitions.find(c);

    if (transition_output_state == state_transitions.end()) {
      return "Could not find transition.";
    }
    else {
      return transition_output_state->second;
    }
  }
}

int main (int argc, char* argv[]) {
  cout << "Reading from file: " << argv[1] << endl;

  vector<pair<string, string>> tokens;

  map<string, map<char, string>> transitions = {
    {"start", {{'/', "forwardSlash"}}}
  };

  string current_state = "start";
  vector<string> seenStates = {};

  string line;
  ifstream javaFile (argv[1]);
  if (javaFile.is_open()) {
    while (getline(javaFile, line)) {
      for (char& c : line) {
	 cout << "Current State: " << current_state << endl;
	 cout << "Transition Character: " << '/' << endl;
	 string transition_state = transition(transitions, current_state, '/');
         cout << "Transition State: " << transition_state << endl;
	 if (transition_state == "Could not find input state." ||
	     transition_state == "Could not find transition.") {
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
