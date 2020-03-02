#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "token_types.h"
#include "token.h"
#include "environment.h"

using namespace std;

void assertNotDefined(map<string,Token*> scopeList, string identifier){
  map<string,Token*>::iterator it = scopeList.find(identifier);
  if (it != scopeList.end()){
    throw logic_error("ERROR: " + identifier + " identifier already defined");
  }
}

map<string,Token*> addToParent(bool parentIsClass, map<string,Token*> scopeList, string identifier, Token *it){
  if (parentIsClass){
    assertNotDefined(scopeList, identifier);
    scopeList[identifier] = it;
  }
  return scopeList;
}

map<string,Token*> addToSelf(map<string,Token*> scopeList, string identifier, Token *it){
  assertNotDefined(scopeList, identifier);
  scopeList[identifier] = it;
  return scopeList;
}

void addProtectedFlag(Token *token){
  Token *modifier = (*token).m_generated_tokens[0].SearchByTypeDFS(T_PROTECTED);
  if (modifier) {
    token->Protected = true;
  }
}

void removeAbstractFlag(Token *token){
  Token *abstract = (*token).m_generated_tokens[0].SearchByTypeDFS(T_ABSTRACT);
  Token *block = (*token).SearchByTypeDFS(Block);
  if (!abstract && block){
    token->Abstract = false;
  }
}

Token traverse(Token *token, environment *scope, bool parentIsClass=false){
  vector<Token> &children = token->m_generated_tokens;
  for(vector<Token>::iterator it=children.begin(); it!=children.end(); it++) {
    // cout << "looking at token" << it->m_display_name << " parentIsClass: " << parentIsClass << endl;
    if (it->m_type == ClassDeclaration){
      string identifier = it->m_generated_tokens[2].m_lex;
      scope->classes = addToParent(parentIsClass, scope->classes, identifier, &(*it));

      addProtectedFlag(&(*it));

      traverse(&(*it), &it->scope, true);
    }
    else if(it->m_type == FieldDeclaration){
      Token identifierToken = *(it->m_generated_tokens.rbegin()+1);
      string identifier = identifierToken.m_lex;

      addProtectedFlag(&(*it));

      scope->fields = addToParent(parentIsClass, scope->fields, identifier, &(*it));
      it->scope.fields = addToSelf(it->scope.fields, identifier, &(*it));
    }
    else if (it->m_type == MethodDeclaration){
      Token *identifierToken = it->m_generated_tokens[0].SearchByTypeDFS(T_IDENTIFIER);
      string identifier = identifierToken->m_lex;

      addProtectedFlag(&(*it));
      removeAbstractFlag(&(*it));

      if (parentIsClass && !(scope->methods.find(identifier) != scope->methods.end())) {
        if (find(scope->methods[identifier].begin(), scope->methods[identifier].end(), &(*it)) == scope->methods[identifier].end()){
          scope->methods[identifier].push_back(&(*it));
        }
        else{
          throw logic_error("ERROR: " + identifier + " identifier already defined");
        }
      }
      if (it->scope.methods.find(identifier) != it->scope.methods.end()) {
        if (find(it->scope.methods[identifier].begin(), it->scope.methods[identifier].end(), &(*it)) == it->scope.methods[identifier].end()){
          it->scope.methods[identifier].push_back(&(*it));
        }
        else{
          throw logic_error("ERROR: " + identifier + " identifier already defined");;
        }
      }
      traverse(&(*it), &it->scope);
    }
    else if (it->m_type == FormalParameter){
      string identifier = it->m_generated_tokens.back().m_lex;
      it->scope.formalParameters = addToSelf(it->scope.formalParameters, identifier, &(*it));
    }
    else if (it->m_type == ConstructorDeclaration){
      Token *identifierToken = it->SearchByTypeDFS(T_IDENTIFIER);
      string identifier = identifierToken->m_lex;

      addProtectedFlag(&(*it));

      if (parentIsClass && !(scope->constructors.find(identifier) != scope->constructors.end())) {
        if (find(scope->constructors[identifier].begin(), scope->constructors[identifier].end(), &(*it)) == scope->constructors[identifier].end()){
          scope->constructors[identifier].push_back(&(*it));
          cout << scope->constructors.size();
        }
        else{
          throw logic_error("ERROR: " + identifier + " identifier already defined");;
        }
      }
      if (it->scope.constructors.find(identifier) != it->scope.constructors.end()) {
        if (find(it->scope.constructors[identifier].begin(), it->scope.constructors[identifier].end(), &(*it)) == it->scope.constructors[identifier].end()){
          it->scope.constructors[identifier].push_back(&(*it));
        }
        else{
          throw logic_error("ERROR: " + identifier + " identifier already defined");
        }
      }
      traverse(&(*it), &identifierToken->scope);
    }
    else if (it->m_type == InterfaceDeclaration){
      string identifier = it->m_generated_tokens[2].m_lex;
      scope->interfaces = addToParent(parentIsClass, scope->interfaces, identifier, &(*it));
      addProtectedFlag(&(*it));
      traverse(&(*it), &it->scope, true);
    }
    else if (it->m_type == AbstractMethodDeclaration){
      Token *identifierToken = it->m_generated_tokens[0].SearchByTypeDFS(T_IDENTIFIER);
      string identifier = identifierToken->m_lex;
      scope->methods[identifier].push_back(&(*it));
      it->scope.methods[identifier].push_back(&(*it));
    }
    else if (it->m_type == BlockStatement){
      traverse(&(*it), &it->scope);
    }
    else if (it->m_type == LocalVariableDeclaration){
      Token *identifierNode = it->SearchByTypeDFS(VariableDeclarator);
      string identifier = identifierNode->m_generated_tokens[0].m_lex;

      if (!it->scope.merge(*scope)){
        throw logic_error("ERROR: " + identifier + " identifier already defined");
      }
      scope->localVariables = addToParent(parentIsClass, scope->localVariables, identifier, &(*it));
      it->scope.localVariables = addToSelf(it->scope.localVariables, identifier, &(*it));

      // uncomment if you want local variables to contain previous local variables
      // *scope = it->scope;

      traverse(&(*it), scope, parentIsClass);
    }
    else if (it->m_type == ForStatement){
      traverse(&(*it), &it->scope);
    }
    else if (it->m_type == CompilationUnit){
      traverse(&(*it), &it->scope, true);
    }
    else {
      traverse(&(*it), scope, parentIsClass);
    }
  }
  return *token;
}

void BuildEnvironment(Token *token){
  traverse(token, &token->scope);
}

void printHelper(string name, map<string,Token*> scopeList){
  cout << name;
  for(map<string,Token*>::iterator subit=scopeList.begin(); subit!=scopeList.end(); subit++){
    cout << subit->first << " type: " << *(subit->second);
    if (subit->second->Protected){
      cout << "(protected)";
    }
    cout << ", ";
  }
  cout << endl;
}

void printHelper2(string name, map<string,vector<Token*>> scopeList){
  cout << name;
  for(map<string,vector<Token*>>::iterator it=scopeList.begin(); it!=scopeList.end(); it++){
    cout << it->first;
    for (vector<Token*>::iterator subit=it->second.begin(); subit!=it->second.end(); subit++){
      if ((*subit)->Protected){
        cout << (*subit)->m_lex << "(protected)";
      }
      if (name == "methods: " && (*subit)->Abstract){
        cout << "(abstract)";
      }
      cout << ", ";
    }
  }
  cout << endl;
}

void printScope(Token token){
  cout << " Type: " << token.m_display_name << " has the following scope:" << endl;
  printHelper("classes: ", token.scope.classes);
  printHelper("interfaces: ", token.scope.interfaces);
  printHelper("fields: ", token.scope.fields);
  printHelper2("methods: ", token.scope.methods);
  printHelper("localVariables: ", token.scope.localVariables);
  printHelper("formalParameters: ", token.scope.formalParameters);
  printHelper2("constructors: ", token.scope.constructors);
}

void printEnvironments(vector<int> levels, Token *token){
  int curLevel = 0;
  for(vector<Token>::iterator it = token->m_generated_tokens.begin(); it!=token->m_generated_tokens.end(); it++) {
    curLevel += 1;
    int anyScope = it->scope.constructors.size() + it->scope.classes.size() +it->scope.interfaces.size();
    anyScope += it->scope.fields.size() + it->scope.methods.size() + it->scope.localVariables.size();
    anyScope += it->scope.formalParameters.size();
    if (anyScope >  0) {
      cout << endl << "Level: ";
      for(vector<int>::iterator level=levels.begin(); level!=levels.end(); level++){
        cout << *level << ".";
      }
      printScope(*it);
    }
    levels.push_back(curLevel);
    printEnvironments(levels, &(*it));
    levels.pop_back();
  }
}
