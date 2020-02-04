#include "parse_table.h"
#include "token.h"

#include <iostream>
#include <vector>

using namespace std;

int main(){
  ParseTable pt = ParseTable();
  // Try to print it all
  string empty_string = "";
  // print all terminal rules
  for(vector<TokenType> &vs: pt.m_reduce_rules){
    for(TokenType &ts: vs){
      Token t(ts,empty_string);
      cout<<t<<" ";
    }
    cout<<endl;
  }

  // Print all entries
  int index = 0;
  cout<<pt.m_parse_table.size()<<endl;
  for(map<TokenType,std::pair<char,int>>& m:pt.m_parse_table){
    for(auto i: m){
      Token t(i.first,empty_string);
      cout<<index<<" "<<t<<" "<<i.second.first<<" "<<i.second.second<<endl;
    }
    index++;
  }
  return 0;
}
