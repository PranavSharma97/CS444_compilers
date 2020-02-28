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
//#include "astProcedures.cpp"

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
    //ASTNode* astNode = CompilationUnitNode(parse_tree); 
  } catch (std::exception& e){
    return 42;
  }

  try{
    Weeder W(file,parse_tree);
    bool valid = W.weed();
    if(!valid){
      cout << "Invalid weeding" << endl;
      return 42;
    }
  } catch (std::exception& e){
    return 42;
  }
  
  cout << "Parsing successful" << endl;
  return 0;
}
