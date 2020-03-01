#include <iostream>
#include <string>
#include <vector>
#include <exception>

// Alphabetical order
#include "debugger.h"
#include "extra_token_logic.h"
#include "parse_table.h"
#include "scanner.h"
#include "token.h"
#include "weeder.h"
#include "build_environment.h"

using namespace std;


int main(int argc, char *argv[]) {
  string file(argv[1]);
  cout << file << endl;

  vector<Token> result;

  try{
    result = extraTokenLogic(scanner(file));
  } catch (std::exception& e){
    cout << "Scanner failed" << endl;
    return 42;
  }
  
  for(Token& t: result){
    DEBUG_MSG('['<<t<<","<<t.m_lex<<"] ");
  }

  Token parse_tree;

  try{
    ParseTable PT = ParseTable();
    bool valid = PT.parse(result);
    if(!valid) {
      cout << "Invalid parsing" << endl;
      return 42;
    }
    parse_tree = PT.parse_tree;
  } catch (std::exception& e){
    return 42;
  }

  Token weeded_tree;
  try{
    Weeder W(file,parse_tree);
    bool valid = W.weed();
    if(!valid){
      cout << "Invalid weeding" << endl;
      return 42;
    }
    weeded_tree = W.m_ast_tree;
  } catch (std::exception& e){
    return 42;
  }

  
  /*
  vector<Token> queue;
  int counter = 0;
  queue.emplace_back(weeded_tree);
  counter = 1;
  int layer = 0;
  while(queue.size()>0){
    Token t = queue[0];
    cout<<t<<" | ";
    queue.erase(queue.begin());
    counter --;
    for(Token n:t.m_generated_tokens){
      queue.emplace_back(n);
    }
    
    if(counter == 0) { layer += 1; counter = queue.size(); cout<<endl; }
  }
  */

  Token tree_with_environment = BuildEnvironment(&weeded_tree);
  vector<int> levels{1};
  printEnvironments(levels,&tree_with_environment);
  
  cout << "Parsing successful" << endl;
  return 0;
}
