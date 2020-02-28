#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "token_types.h"
#include "astNodes.h"

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
bool addToMethods(environment *scope, string identifier, MethodDeclarationNode *it){
  if (scope->methods.find(identifier) != scope->methods.end()){
    vector<MethodDeclarationNode*> methods = scope->methods[identifier];
    if (find(methods.begin(), methods.end(), it) == methods.end()){
      methods.push_back(it);
    }
    else {
      return false;
    }
  }
  else {
    pair<string,vector<MethodDeclarationNode*>> methodsPair;
    methodsPair.first = identifier;
    methodsPair.second.push_back(it);
    scope->methods.insert(methodsPair);
  }
  return true;
}

// Add to constructors and check if in environment constructors already
// if inside constructors already return false
bool addToConstructors(environment *scope, string identifier, ConstructorDeclarationNode* it){
  if (scope->constructors.find(identifier) != scope->constructors.end()) {
    vector<ConstructorDeclarationNode*> constructors = scope->constructors[identifier];  
    if (find(constructors.begin(), constructors.end(), it) == constructors.end()){
      scope->constructors[identifier].push_back(it);
    }
    else {
      return false;
    }
  }
  else {
    pair<string,vector<ConstructorDeclarationNode*>> constructorPair;
    constructorPair.first = identifier;
    constructorPair.second.push_back(it);
    scope->constructors.insert(constructorPair);
  }
  return true;
}


int traverse(vector<ASTNode*> children, environment* scope, bool parentIsClass=false){
  for(vector<ASTNode*>::iterator it=children.begin(); it!=children.end(); it++){
    if ((*it)->type() == ClassDeclaration){
      ClassDeclarationNode *classNode = dynamic_cast<ClassDeclarationNode*>(*it);

      if (parentIsClass){
        scope->classes.insert(pair<string,ClassDeclarationNode*>(classNode->identifier, classNode));
      }

      (*it)->scope.classes.insert(pair<string,ClassDeclarationNode*>(classNode->identifier, classNode));
      // traverse children with new scope
      traverse(classNode->children, &classNode->scope,true);
      cout << "environment of class is" << endl;
      debugPrint(&classNode->scope);
    }
    else if ((*it)->type() == FieldDeclaration){
      FieldDeclarationNode* fieldNode = dynamic_cast<FieldDeclarationNode*>(*it);
      VariableDeclaratorNode* variableNode = dynamic_cast<VariableDeclaratorNode*>(fieldNode->children.back());
      string identifier = variableNode->identifier;

      if (!fieldNode->scope.merge(scope)){
        return 42;
      }

      if (parentIsClass) {
        scope->localVariables.insert(pair<string,FieldDeclarationNode*>(identifier,fieldNode));
      }
 
      fieldNode->scope.localVariables.insert(pair<string,FieldDeclarationNode*>(identifier,fieldNode));

      // set scope pointer to subscope
      *scope = fieldNode->scope;
    }
    else if ((*it)->type() == MethodDeclaration){
      MethodDeclarationNode* methodNode = dynamic_cast<MethodDeclarationNode*>(*it);
      // go to MethodHeaderNode -> MethodDeclaratorNode -> Identifier
      MethodHeaderNode* header = dynamic_cast<MethodHeaderNode*>(methodNode->children[0]);
      vector<ASTNode*> subChildren = header->children;
      string identifier;
      for(vector<ASTNode*>::iterator sub_it=subChildren.begin(); sub_it!=subChildren.end(); sub_it++){
        if ((*sub_it)->type() == MethodDeclarator){
          MethodDeclaratorNode* declarator = dynamic_cast<MethodDeclaratorNode*>(*sub_it);
          identifier = declarator->identifier;
        }
      }
      if (parentIsClass && !addToMethods(scope, identifier, methodNode)){
        return 42;
      }
      if (!addToMethods(&methodNode->scope, identifier, methodNode)){
        return 42;
      }
      // traverse children with new scope
      traverse(methodNode->children, &methodNode->scope);
    }/*
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
      if (parentIsClass && !addToConstructors(scope, identifier, &it)){
        return 42;
      }
      if (!addToConstructors(it->scope, identifier, &it){
        return 42;
      }
      // traverse children with new scope
      traverse(it->children, it->scope);
    }
    else if (it->type() == InterfaceDeclaration){
      dynamic_cast<InterfaceDeclarationNode*>(it);
      if (parentIsClass){
        scope->interfaces.insert((pair<string,InterfaceDeclarationNode*>(it->identifier,*it));
      }
      it->scope->interfaces.insert((pair<string,InterfaceDeclarationNode*>(it->identifier,*it));
      // traverse children with new scope
      traverse(it->children, it->scope);
    }
    else if (it->type() == BlockStatement){
      dynamic_cast<BlockStatementsNode*>(it);
      // traverse children with new scope
      traverse(it->children, it->scope);
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
      it->scope.merge(scope);
      if (parentIsClass){
         scope->localVariables.insert((pair<string,LocalVariableDeclarationNode*>(it->identifier,*it));
      }
      it->scope->localVariables.insert((pair<string,LocalVariableDeclarationNode*>(it->identifier,*it));
      // set scope pointer to subscope
      *scope = it->scope;
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
        it[0]->scope->localVariables.insert((pair<string,LocalVariableDeclarationNode*>(it->identifier,*it));
      }
      traverse(it->children.back().children, it[0]->scope);
    }
    else {
      traverse(it->children, scope);
    }*/
  }
  return 0;
}

ASTNode BuildEnvironment(ASTNode *AST){
  traverse(AST->children, &AST->scope);
}
