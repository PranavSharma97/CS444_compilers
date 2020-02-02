#include <iostream>
#include <string>
#include <vector>

#include "token.h"
#include "scanner.h"

using namespace std;

int main(int argc, char *argv[]) {
  string file(argv[1]);
  cout << file << endl;

  vector<Token> result = scanner(file);
  
  return 0;
}
