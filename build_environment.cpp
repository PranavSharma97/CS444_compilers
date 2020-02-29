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

int traverse(vector<Token> children, environment scope, bool parentIsClass=false){
  cout << "starting traversal" << endl;
  for(vector<Token>::iterator it=children.begin(); it!=children.end(); it++) {
    cout << "looking for token " << it->m_type << " lex " << it->m_lex << endl;
    if (it->m_type == ClassDeclaration){
      string identifier = it->m_generated_tokens[2].m_lex;
      cout << "found class declaration with identifier " << identifier << endl;
      if (parentIsClass){
        scope.classes[identifier] = &(*it);
      }
      it->scope.classes[identifier] = &(*it);
      traverse(it->m_generated_tokens, it->scope, true);
      cout << "finished traversing class" <<  identifier << endl;
    }
    else if(it->m_type == FieldDeclaration){
      string identifier = it->m_generated_tokens.back().m_lex;
      cout << "found FieldDeclaration with identifier " << identifier << endl;
      if(!it->scope.merge(scope)){
        return 42;
      }
      if (parentIsClass) {
        scope.fields[identifier] = &(*it);
      }
      it->scope.fields[identifier] = &(*it);

      scope = it->scope;
    }
    else if (it->m_type == MethodDeclaration){
      Token *identifierToken = it->m_generated_tokens[0].SearchByTypeDFS(T_IDENTIFIER);
      string identifier = identifierToken->m_lex;
      cout << "found MethodDeclaration with identifier " << identifier << endl;
      if (parentIsClass && !(scope.methods.find(identifier) != scope.methods.end())) {
        if (find(scope.methods[identifier].begin(), scope.methods[identifier].end(), &(*it)) == scope.methods[identifier].end()){
          scope.methods[identifier].push_back(&(*it));
        }
        else{
          return 42;
        }
      }
      if (it->scope.methods.find(identifier) != it->scope.methods.end()) {
        if (find(it->scope.methods[identifier].begin(), it->scope.methods[identifier].end(), &(*it)) == it->scope.methods[identifier].end()){
          it->scope.methods[identifier].push_back(&(*it));
        }
        else{
          return 42;
        }
      }
      traverse(it->m_generated_tokens, it->scope);
      cout << "finished traversing MethodDeclaration with identifier " << identifier << endl;
    }
    else if (it->m_type == FormalParameter){
      string identifier = it->m_generated_tokens.back().m_lex;
      cout << "found FormalParameter with identifier " << identifier << endl;
      if (parentIsClass){
        scope.formalParameters[identifier] = &(*it);
      }
    }
    else if (it->m_type == ConstructorDeclaration){
      Token *identifierToken = it->SearchByTypeDFS(T_IDENTIFIER);
      string identifier = identifierToken->m_lex;
      cout << "found MethodDeclaration with identifier " << identifier << endl;
      if (parentIsClass && !(scope.constructors.find(identifier) != scope.constructors.end())) {
        if (find(scope.constructors[identifier].begin(), scope.constructors[identifier].end(), &(*it)) == scope.constructors[identifier].end()){
          scope.constructors[identifier].push_back(&(*it));
        }
        else{
          return 42;
        }
      }
      if (it->scope.constructors.find(identifier) != it->scope.constructors.end()) {
        if (find(it->scope.constructors[identifier].begin(), it->scope.constructors[identifier].end(), &(*it)) == it->scope.constructors[identifier].end()){
          it->scope.constructors[identifier].push_back(&(*it));
        }
        else{
          return 42;
        }
      }
      traverse(it->m_generated_tokens, it->scope);
      cout << "finished traversing ConstructorDeclaration with identifier " << identifier << endl;
    }
    else if (it->m_type == InterfaceDeclaration){
      string identifier = it->m_generated_tokens[2].m_lex;
      cout << "found InterfaceDeclaration with identifier " << identifier << endl;
      if (parentIsClass){
        scope.interfaces[identifier] = &(*it);
      }
      traverse(it->m_generated_tokens, it->scope);
      cout << "finished traversing InterfaceDeclaration with identifier " << identifier << endl;
    }
    else if (it->m_type == BlockStatement){
      cout << "found BlockStatement" << endl;
      traverse(it->m_generated_tokens, it->scope);
      cout << "finished BlockStatement" << endl;
    }
    else if (it->m_type == LocalVariableDeclaration){
      Token *methodDecl = it->SearchByTypeDFS(MethodDeclarator);
      string identifier = methodDecl->m_lex;
      cout << "found LocalVariableDeclaration ith identifier " << identifier << endl;
      if (!it->scope.merge(scope)){
        return 42;
      }
      if (parentIsClass){
        scope.localVariables[identifier] = &(*it);
      }
      it->scope.localVariables[identifier] = &(*it);
      scope = it->scope;
      traverse(it->m_generated_tokens, scope);
      cout << "finished traversing LocalVariableDeclaration with identifier " << identifier << endl;
    }
    else if (it->m_type == ForStatement){
      cout << "found ForStatement" << endl;
      traverse(it->m_generated_tokens, it->scope);
      cout << "end ForStatement" << endl;
    }
    else {
      traverse(it->m_generated_tokens, scope);
    }
  }
  return 0;
}

int BuildEnvironment(Token *token){
  return traverse(token->m_generated_tokens, token->scope);
}
