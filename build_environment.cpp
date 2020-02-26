#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "token_types.h"

using namespace std;

void traverse(list<ASTNode> children, environment scope){
  for(vector<ASTNode>::iterator it=children.begin(); it!=children.end(); it++){
    if (it->type() == ClassDeclaration){
      dynamic_cast<ClassDeclarationNode*>(it);
      scope.classes.insert(pair<string,ClassDeclarationNode>(it->identifier,it));
      traverse(it->children, it->environment);
    }
    else if (it->type() == FieldDeclaration){
      dynamic_cast<FieldDeclarationNode*>(it);
      string identifier = it->children.back().identifier;
      scope.localVariables.insert(pair<string,FieldDeclarationNode>(identifier,it));
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
      if (scope.methods.find(identifier) == scope.methods.end()){
        scope.methods.insert(pair<string,vector<MethodDeclarationNode>>(identifier,[it]));
      }
      else{
        scope.methods[identifier].push_back(it);
      }
      traverse(it->children, it->environment);
    }
    else if (it->type() == FormalParameter){
      dynamic_cast<FormalParameterNode*>(it);
      string identifier = it->children.back().identifier;
      scope.formalParameters.insert(pair<string,FormalParameterNode>(identifier,it));
    }
    else if (it->type() == ConstructorDeclaration){
      dynamic_cast<ConstructorDeclaration*>(it);
      string identifier;
      for(vector<ASTNode>::iterator sub_it=subChildren.begin(); sub_it!=subChildren.end(); sub_it++){
        if (sub_it->type() == ConstructorDeclarator){
          dynamic_cast<ConstructorDeclarator*>(sub_it);
          identifier = sub_it->identifier;
        }
      }
      if (scope.constructors.find(identifier) == scope.constructors.end()){
        scope.constructors.insert(pair<string,vector<ConstructorDeclarationNode>>(identifier,[it]));
      }
      else{
        scope.constructors[identifier].push_back(it);
      }
      traverse(it->children, it->environment);
    }
    else if (it->type() == InterfaceDeclaration){
      dynamic_cast<InterfaceDeclarationNode*>(it);
      scope.interfaces.insert((pair<string,InterfaceDeclarationNode>(it->identifier,it));
      traverse(it->children, it->environment);    
    }
    else if (it->type() == BlockStatement){
      dynamic_cast<BlockStatementsNode*>(it);
      traverse(it->children, it->environment);
    }
    else if (it->type() == LocalVariableDeclaration){
      dynamic_cast<LocalVariableDeclarationNode*>(it);
      // VariableDeclarators -> VariableDeclaratorId -> identifier
      string identifier;
      vector<ASTNode> subChildren = it->children[0];
      for(vector<ASTNode>::iterator sub_it=subChildren.begin(); sub_it!=subChildren.end(); sub_it++){
        if (sub_it->type() == MethodDeclarator){
          dynamic_cast<MethodDeclarator*>(sub_it);
          identifier = sub_it->identifier;
        }
      }
      traverse(it->children, scope);
    }
    else {
      traverse(it->children, scope);
    }
  }
}

CompilationUnitNode BuildEnvironment(CompilationUnitNode AST){
  traverse(AST->children, AST->environment);
}
