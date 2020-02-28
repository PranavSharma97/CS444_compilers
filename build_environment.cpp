#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "token_types.h"

using namespace std;


void debugPrint(environment *scope){
  cout << "Keys in classes: ";
  for (auto const& pair: scope->classes) {
    cout << pair.first << " ";
  }
  cout << endl;
  cout << "Keys in interfaces: ";
  for (auto const& pair: scope->interfaces) {
    cout << pair.first << " ";
  }
  cout << endl;
  cout << "Keys in fields: ";
  for (auto const& pair: scope->fields) {
    cout << pair.first << " ";
  }
  cout << endl;
  cout << "Keys in methods: ";
  for (auto const& pair: scope->methods) {
    cout << pair.first << " ";
  }
  cout << endl;
  cout << "Keys in localVariables: ";
  for (auto const& pair: scope->localVariables) {
    cout << pair.first << " ";
  }
  cout << endl;
  cout << "Keys in formalParameters: ";
  for (auto const& pair: scope->formalParameters) {
    cout << pair.first << " ";
  }
  cout << endl;
  cout << "Keys in Constructors: ";
  for (auto const& pair: scope->constructors) {
    cout << pair.first << " ";
  }
  cout << endl;
}

// Add to methods and check if in environment methods already
// if inside methods already return false
bool addToMethods(environment *scope, string identifier, ASTNode* it){
  if (scope->methods.find(identifier) != scope->methods.end() &&
      scope->methods[identifier].find(it) == scope->methods[identifier].end()){
    scope->methods[identifier].push_back(it);
  }
  else if (scope->methods.find(identifier) != scope->methods.end()){
    return false;
  }
  else{
    scope->methods.insert(pair<string,vector<MethodDeclarationNode>>(identifier,[it]));
  }
  return true;
}

// Add to constructors and check if in environment constructors already
// if inside constructors already return false
bool addToConstructors(environment *scope, string identifier, ASTNode* it){
  if (scope->constructors.find(identifier) != scope->constructors.end() &&
      scope->constructors[identifier].find(it) == scope->constructors[identifier].end()){
    scope->constructors[identifier].push_back(it);
  }
  else if (scope->constructors.find(identifier) != scope->constructors.end()){
    return false;
  }
  else {
    scope->constructors.insert(pair<string,vector<ConstructorDeclarationNode>>(identifier,[it]);
  }
  return true;
}


void traverse(list<ASTNode*> children, environment* scope, parentIsClass=false){
  for(vector<ASTNode*>::iterator it=children.begin(); it!=children.end(); it++){
    if (it->type() == ClassDeclaration){
      dynamic_cast<ClassDeclarationNode*>(it);

      if (parentIsClass){
        scope->classes.insert(pair<string,ClassDeclarationNode*>(it->identifier,*it));
      }

      it->environment.classes.insert(pair<string,ClassDeclarationNode*>(it->identifier,*it));
      // traverse children with new scope
      traverse(it->children, it->environment,true);
      cout << "environment of class is" << endl;
      debugPrint(&it->environment)
    }
    else if (it->type() == FieldDeclaration){
      dynamic_cast<FieldDeclarationNode*>(it);
      string identifier = it->children.back().identifier;

      if (!it->environment.merge(scope)){
        return 42;
      }

      if (parentIsClass) {
        scope->localVariables.insert(pair<string,FieldDeclarationNode*>(identifier,*it));
      }
 
      it->environment.localVariables.insert(pair<string,FieldDeclarationNode*>(identifier,*it));

      // set scope pointer to subscope
      *scope = it->environment;
    }
    else if (it->type() == MethodDeclaration){
      dynamic_cast<MethodDeclarationNode*>(it);
      // go to MethodHeaderNode -> MethodDeclaratorNode -> Identifier
      vector<ASTNode> subChildren = it->children[0];
      string identifier;
      for(vector<ASTNode>::iterator sub_it=subChildren.begin(); sub_it!=subChildren.end(); sub_it++){
        if (sub_it->type() == MethodDeclarator){
          dynamic_cast<MethodDeclarator*>(sub_it);
          identifier = sub_it->identifier;
        }
      }
      if (parentIsClass && !addToMethods(scope, identifier, *it)){
        return 42;
      }
      if (!addToMethods(it->environment, identifier, *it)){
        return 42;
      }
      // traverse children with new scope
      traverse(it->children, it->environment);
    }
    else if (it->type() == FormalParameter){
      dynamic_cast<FormalParameterNode*>(it);
      string identifier = it->children.back().identifier;
      if (parentIsClass){
        scope->formalParameters.insert(pair<string,FormalParameterNode*>(identifier,*it));
      }
    }
    else if (it->type() == ConstructorDeclaration){
      dynamic_cast<ConstructorDeclaration*>(it);
      string identifier;
      // go to ConstructorDeclarator -> Identifier
      for(vector<ASTNode>::iterator sub_it=subChildren.begin(); sub_it!=subChildren.end(); sub_it++){
        if (sub_it->type() == ConstructorDeclarator){
          dynamic_cast<ConstructorDeclarator*>(sub_it);
          identifier = sub_it->identifier;
        }
      }
      if (parentIsClass && !addToConstructors(scope, identifier, *it)){
        return 42;
      }
      if (!addToConstructors(it->environment, identifier, *it){
        return 42;
      }
      // traverse children with new scope
      traverse(it->children, it->environment);
    }
    else if (it->type() == InterfaceDeclaration){
      dynamic_cast<InterfaceDeclarationNode*>(it);
      if (parentIsClass){
        scope->interfaces.insert((pair<string,InterfaceDeclarationNode*>(it->identifier,*it));
      }
      it->environment->interfaces.insert((pair<string,InterfaceDeclarationNode*>(it->identifier,*it));
      // traverse children with new scope
      traverse(it->children, it->environment);
    }
    else if (it->type() == BlockStatement){
      dynamic_cast<BlockStatementsNode*>(it);
      // traverse children with new scope
      traverse(it->children, it->environment);
    }
    else if (it->type() == LocalVariableDeclaration){
      dynamic_cast<LocalVariableDeclarationNode*>(it);
      // VariableDeclarators -> VariableDeclaratorId -> identifier
      string identifier;
      vector<ASTNode> subChildren = it->children;
      for(vector<ASTNode>::iterator sub_it=subChildren.begin(); sub_it!=subChildren.end(); sub_it++){
        if (sub_it->type() == VariableDeclarator){
          dynamic_cast<MethodDeclarator*>(sub_it);
          identifier = sub_it->identifier;
        }
      }
      // local variable declaration will have its own scope
      it->environment.merge(scope);
      if (parentIsClass){
         scope->localVariables.insert((pair<string,LocalVariableDeclarationNode*>(it->identifier,*it));
      }
      it->environment->localVariables.insert((pair<string,LocalVariableDeclarationNode*>(it->identifier,*it));
      // set scope pointer to subscope
      *scope = it->environment;
      traverse(it->children, scope);
    }
    else if (it->type() == ForStatement){
      dynamic_cast<ForStatementNode*>(it);
      // LocalVariableDeclaration
      if (it->children[0].type() == LocalVariableDeclaration){
        string identifier;
        vector<ASTNode> subChildren = it->children[0];
        for(vector<ASTNode>::iterator sub_it=subChildren.begin(); sub_it!=subChildren.end(); sub_it++){
          if (sub_it->type() == VariableDeclarator){
            dynamic_cast<VariableDeclaratorNode*>(sub_it);
            identifier = sub_it->identifier;
          }
        }
        it[0]->environment->localVariables.insert((pair<string,LocalVariableDeclarationNode*>(it->identifier,*it));
      }
      traverse(it->children.back().children, it[0]->environment);
    }
    else {
      traverse(it->children, scope);
    }
  }
}

CompilationUnitNode BuildEnvironment(CompilationUnitNode AST){
  traverse(AST->children, AST->environment);
}
