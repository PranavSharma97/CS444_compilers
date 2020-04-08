#include <iostream>
#include <string>
#include <vector>
#include <exception>
#include <utility>

// Alphabetical order
#include "debugger.h"
#include "extra_token_logic.h"
#include "parse_table.h"
#include "scanner.h"
#include "token.h"
#include "weeder.h"
#include "build_environment.h"
#include "type_linker.h"
#include "color_print.h"
#include "name_checker.h"
#include "type_checker.h"

using namespace std;

void CheckToken(Token* t){
  if(t->m_type == T_IDENTIFIER && t->declaration) {
    std::cerr << "IDENTIFIER: " << t->m_lex << " is connected to " << t->declaration->m_display_name << std::endl;
  }
  else if (t->m_type == T_IDENTIFIER){
    std::cerr << "IDENTIFIER: " << t->m_lex << " is not connected" << std::endl;
  }
  else if(t->m_generated_tokens.size() > 0){
    for(Token& child: t->m_generated_tokens){
      CheckToken(&child);
    }
  }
  return;
}

int main(int argc, char *argv[]) {

  vector<Token> parse_trees;
  vector<Token*> tree_ptrs;
  Token object_interface;
  for(int i = 0;i<argc;i++){
    // compile the interface version of java lang object
    string file;
    if(i == 0){
      file = "data/java/ObjectINTERFACE.java";
    } else {
      string s(argv[i]);
      file = s;
    }
    cout << "Compiling:"<<file << endl;

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
    
    try{
      Weeder W(file,parse_tree);
      bool valid = W.weed();
      if(!valid){
	cout << "Invalid weeding" << endl;
	return 42;
      }
      // 0 is the parse interface version of java object
      //if(i!=0){
      parse_trees.emplace_back(W.m_ast_tree);
	/*} else {
	// Record the object version of interface 
	object_interface = W.m_ast_tree;
	}*/
      
    } catch (std::exception& e){
      return 42;
    }
  }
    
  for(int i = 0; i<parse_trees.size();i++){
    BuildEnvironment(&parse_trees[i]);
    tree_ptrs.emplace_back((&parse_trees[i]));
    vector<int> levels{0};
    printEnvironments(levels,&parse_trees[i]);
    //printEnvironments(levels,&parse_trees[i-1],1);
  }
  //BuildEnvironment(&object_interface);
  
  
  
  // convert parse_trees to token pointers
  /* for(int i = 0;i<parse_trees.size();i++){
    tree_ptrs.emplace_back(&parse_trees[i]);
  }*/
  // Bind Compilation Unit
  for(Token* t: tree_ptrs){
    t->BindCompilationUnit();
  }

  /*
  for(Token* t: tree_ptrs){
    if(!t->BuildDeclaredSet()) {
      return 42;
    }
  }
  */

  //object_interface.BindCompilationUnit();

  
  /*
  for(Token* t: tree_ptrs){
    t->SearchByTypeBFS(TokenType::TOKEN_FAILURE);
    }*/
  NameChecker NCheck;
  NCheck.m_asts = tree_ptrs;
  TypeLinker TPLink(tree_ptrs);
  TPLink.set_object_interface(tree_ptrs[0],&NCheck);
  // TPLink.set_object_interface(&object_interface);
  if(!TPLink.Link()) return 42;
  if(!NCheck.CheckNames()) return 42;

//  for(Token* t: tree_ptrs){
//    if (!NCheck.LinkStringLiterals(t)) return 42;
//  }

  

/*
  try {
    for(Token* t: tree_ptrs){
      checkTypes(t);
    }
  }
  catch (std::exception& e){
    cerr<<e.what()<<endl;
    return 42;
  }
*/
  /*
  for(Token* ti: tree_ptrs){
    vector<Token*> queue;
    int counter = 0;
    queue.emplace_back(ti);
    counter = 1;
    int layer = 0;
    while(queue.size()>0){
      Token* t = queue[0];
      if(t->declaration != nullptr){
	cerr<<"("<<*t<<","<<t->m_lex<<"->"<<*(t->declaration)<<") | ";
      } else {
	cerr<<"("<<*t<<","<<t->m_lex<<"-> No) | ";
      }
      queue.erase(queue.begin());
      counter --;
      for(Token& n:t->m_generated_tokens){
	queue.emplace_back(&n);
      }
    
      if(counter == 0) { layer += 1; counter = queue.size(); cerr<<endl; }
    } 
    }*/
  
  cout << "Parsing successful" << endl;
  return 0;
}
