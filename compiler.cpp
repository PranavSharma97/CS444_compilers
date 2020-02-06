#include <iostream>
#include <string>
#include <vector>
#include <exception>

// Alphabetical order
#include "extra_token_logic.h"
#include "parse_table.h"
#include "scanner.h"
#include "token.h"

using namespace std;

int main(int argc, char *argv[]) {
  string file(argv[1]);
  cout << file << endl;

  vector<Token> result = extraTokenLogic(scanner(file));
  
  for(Token& t: result){
    cout<<'['<<t<<","<<t.m_lex<<"] ";
  }
  cout<<endl;

  try{
    ParseTable PT = ParseTable();
    bool valid = PT.parse(result);
    if(!valid) return 42;
  } catch (std::exception& e){
    return -1;
  }
  
  return 0;
}
