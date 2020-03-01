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

Token traverse(Token *token, environment *scope, bool parentIsClass=false){
  vector<Token> &children = token->m_generated_tokens;
  for(vector<Token>::iterator it=children.begin(); it!=children.end(); it++) {
    if (it->m_type == ClassDeclaration){
      string identifier = it->m_generated_tokens[2].m_lex;
      scope->classes = addToParent(parentIsClass, scope->classes, identifier, &(*it));
      it->scope.classes = addToSelf(it->scope.classes, identifier, &(*it));

      traverse(&(*it), &it->scope, true);
    }
    else if(it->m_type == FieldDeclaration){
      string identifier = it->m_generated_tokens.back().m_lex;

      scope->fields = addToParent(parentIsClass, scope->fields, identifier, &(*it));
      it->scope.fields = addToSelf(it->scope.fields, identifier, &(*it));
    }
    else if (it->m_type == MethodDeclaration){
      Token *identifierToken = it->m_generated_tokens[0].SearchByTypeDFS(T_IDENTIFIER);
      string identifier = identifierToken->m_lex;
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
      traverse(&(*it), &it->scope);
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
    else {
      traverse(&(*it), scope, parentIsClass);
    }
  }
  return *token;
}

Token BuildEnvironment(Token *token){
  return traverse(token, &token->scope);
}

void printEnvironments(int level, Token token){
  for(vector<Token>::iterator it = token.m_generated_tokens.begin(); it!=token.m_generated_tokens.end(); it++) {
    int anyScope = it->scope.constructors.size() + it->scope.classes.size() +it->scope.interfaces.size();
    anyScope += it->scope.fields.size() + it->scope.methods.size() + it->scope.localVariables.size();
    anyScope += it->scope.formalParameters.size();
    if (anyScope >  0) {
      cout << endl;
      cout << "Level: " << level << " Type: " << it->m_display_name << " has the following scope:" << endl;
      cout << "classes: ";
      for(map<string,Token*>::iterator subit=it->scope.classes.begin(); subit!=it->scope.classes.end(); subit++){
        cout << subit->first;
      }
      cout << endl << "interfaces: ";
      for(map<string,Token*>::iterator subit=it->scope.interfaces.begin(); subit!=it->scope.interfaces.end(); subit++){
        cout << subit->first;
      }
      cout << endl << "fields: ";
      for(map<string,Token*>::iterator subit=it->scope.fields.begin(); subit!=it->scope.fields.end(); subit++){
        cout << subit->first;
      }
      cout << endl << "methods: ";
      for(map<string,vector<Token*>>::iterator subit=it->scope.methods.begin(); subit!=it->scope.methods.end(); subit++){
        cout << subit->first;
      }
      cout << endl << "localVariables: ";
      for(map<string,Token*>::iterator subit=it->scope.localVariables.begin(); subit!=it->scope.localVariables.end(); subit++){
        cout << subit->first;
      }
      cout << endl << "formalParameters: ";
      for(map<string,Token*>::iterator subit=it->scope.formalParameters.begin(); subit!=it->scope.formalParameters.end(); subit++){
        cout << subit->first;
      }
      cout << endl << "constructors: ";
      for(map<string,vector<Token*>>::iterator subit=it->scope.constructors.begin(); subit!=it->scope.constructors.end(); subit++){
        cout << subit->first;
      }
      cout << endl;
    }
    printEnvironments(level+1, *it);
  }
}
