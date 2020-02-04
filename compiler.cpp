#include <iostream>
#include <string>
#include <vector>

#include "token.h"
#include "scanner.h"
#include "extra_token_logic.h"

using namespace std;

int main(int argc, char *argv[]) {
  string file(argv[1]);
  cout << file << endl;

  vector<Token> result = extraTokenLogic(scanner(file));
  
  for(Token& t: result){
    cout<<'['<<t<<","<<t.m_lex<<"] ";
  }
  cout<<endl;

  return 0;
}
