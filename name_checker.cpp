#include "name_checker.h"
#include <iostream>
#include "helper_functions.h"
#include "type_linker.h"
#include "color_print.h"

/*
class NameChecker{
  
 public:
  Package* m_packages;
  environment* local_envs;
  environment* single_types;
  environment** pack_envs;
  environment* on_demand;

  Token* m_asts;
  
  NameChecker() = default;
  ~NameChecker();
};
*/

NameChecker::NameChecker(){
  local_envs = single_types = on_demand = nullptr;
  pack_envs = nullptr;
  m_packages = nullptr;
}


NameChecker::~NameChecker(){
  // Do Not delete m_packages, this should
  // be deleted by TypeLinker
  // It's a shallow copy
  if(local_envs != nullptr){
    delete[] local_envs;
    delete[] single_types;
    delete[] pack_envs;
    delete[] on_demand;
  }
}

bool NameChecker::CheckNames(){
  // Resolve NameSpaces
  // Resolve Fields
  // Resolve Expressions


  
  // Everything Below is copied from type_linker.cpp
  // their corresponding methods and implementations are copied
  // here as well
  // but not declared in the .h file yet.
  int file_count = m_asts.size();
  int file_index = 0;
  environment local_envs[file_count];
  environment single_types[file_count];
  environment* pack_envs[file_count];
  environment on_demands[file_count];

  for(Token* n: m_asts){
    if (file_count - file_index - 16 <= 0) break; 
    environment* envs[4];
    envs[0] = &local_envs[file_index];
    envs[1] = &single_types[file_index];
    envs[2] = pack_envs[file_index];
    envs[3] = &on_demands[file_index];
    file_index ++;

    if(!ResolveNameSpaces(m_asts[file_index],envs)) return false;
    CYAN();
    std::cout<<"NameSpaces Resolved"<<std::endl;
    DEFAULT(); 
  }
  
  file_index = 0;
  for(Token* n: m_asts){
    if (file_count - file_index - 16 <= 0) break;
    environment* envs[4];
    envs[0] = &local_envs[file_index];
    envs[1] = &single_types[file_index];
    envs[2] = pack_envs[file_index];
    envs[3] = &on_demands[file_index];
    file_index ++;

    if(!ResolveFieldDeclarations(m_asts[file_index],envs)) return false;
    CYAN();
    DEFAULT();
  }

  file_index = 0;
  for(Token* n: m_asts){
    if (file_count - file_index - 16 <= 0) break;
    environment* envs[4];
    envs[0] = &local_envs[file_index];
    envs[1] = &single_types[file_index];
    envs[2] = pack_envs[file_index];
    envs[3] = &on_demands[file_index];
    file_index ++;

    if(!ResolveExpressions(m_asts[file_index],envs,false)) return false;
    CYAN();
    std::cout<<"Expressions Resolved"<<std::endl;
    DEFAULT();
  }
  return true;
}

bool NameChecker::ResolveNameSpaces(Token* root, environment** envs){
  TokenType t = root->type();  
  if (t != ClassDeclaration && t != InterfaceDeclaration){
    if (!envs[0]->merge(root->scope)){
      RED();
      std::cerr << "Error: Shadowing of local variables not allowed" << std::endl;
      DEFAULT();
      return false;
    }
  }

  /********************** COMMENTS *******************************
  std::cout << "TOKEN TYPE: " << root->m_display_name << std::endl;

  std::cout << "constructors: ";
  for(std::pair<std::string, std::vector<Token*>> kv_pair: envs[0]->constructors){
    std::cout << kv_pair.first;
  }
  std::cout << std::endl;
  std::cout << "methods: ";
  for(std::pair<std::string, std::vector<Token*>> kv_pair: envs[0]->methods){
    std::cout << kv_pair.first;
  }
  std::cout << std::endl;
  std::cout << "fields: ";
  for(std::pair<std::string, Token*> kv_pair: envs[0]->fields){
    std::cout << kv_pair.first;
  }
  std::cout << std::endl;
  std::cout << "localVariables: ";
  for(std::pair<std::string, Token*> kv_pair: envs[0]->localVariables){
    std::cout << kv_pair.first;
  }
  std::cout << std::endl;
  std::cout << "formalParameters: ";
  for(std::pair<std::string, Token*> kv_pair: envs[0]->formalParameters){
    std::cout << kv_pair.first;
  }
  std::cout << std::endl;

  ***************************** COMMENTS END ********************/
  environment next_local_env(*(envs[0]));

  environment* new_envs[4];

  new_envs[0] = &next_local_env;
  new_envs[1] = envs[1];
  new_envs[2] = envs[2];
  new_envs[3] = envs[3];
  
  // std::cout << "TOKEN TYPE: " << root->m_display_name << std::endl;

  if (t == FieldAccess || t == QualifiedName || 
     (t == MethodInvocation && root->m_generated_tokens[1].m_type == T_DOT)) {
    Token* firstIdentifier = root->SearchByTypeDFS(T_IDENTIFIER);

    Token* declaration = envs[0]->GetDeclaration(firstIdentifier->m_lex);
    if (declaration){
      root->declaration = declaration;
      firstIdentifier->declaration = declaration;
      CYAN();
      std::cout<<"Linking " << firstIdentifier->m_lex << " to " << declaration->m_display_name << std::endl;
      DEFAULT();
    }
    else{
      CYAN();
      std::cout << "Could not link \"" << firstIdentifier->m_lex << "\" possibly a type" << std::endl;
      DEFAULT();
    }
  }

  bool result = true;
  for(Token& n: root->m_generated_tokens){
    if (t == LocalVariableDeclarationStatement || t == FormalParameterList || t == FormalParameter || t == MethodDeclarator ||
        t == MethodHeader || t == ConstructorDeclarator){
      if (!ResolveNameSpaces(&n, envs)) return false;
    } else {
      if (!ResolveNameSpaces(&n, new_envs)) return false;
    }
  }

  return true;
}

bool NameChecker::ResolveFieldDeclarations(Token* root, environment** envs){
  TokenType t = root->type();
  if (t != ClassDeclaration && t!= InterfaceDeclaration){
    envs[0]->force_merge(root->scope);
  }
  environment next_local_env(*(envs[0]));
  environment* new_envs[4];

  new_envs[0] = &next_local_env;
  new_envs[1] = envs[1];
  new_envs[2] = envs[2];
  new_envs[3] = envs[3];

  if (t == FieldDeclaration){
    // check there is no reference to self in FieldDeclaration
    Token *identifierToken = root->SearchByTypeDFS(T_IDENTIFIER);
    std::string identifier = identifierToken->m_lex;

    Token *rightHandToken = root->SearchByTypeDFS(VariableDeclarator);
    Token mostRightToken;
    if (rightHandToken) mostRightToken = rightHandToken->m_generated_tokens.back();
    Token *rightHandIdentifier = nullptr;

    if (rightHandToken) rightHandIdentifier = mostRightToken.SearchByTypeDFS(T_IDENTIFIER);
    if (rightHandIdentifier && rightHandIdentifier->m_lex == identifier){
      RED();
      std::cout << "Error cannot use field name \"" << identifier <<  "\" in self declaration" << std::endl;
      DEFAULT();
      return false;
    }
    if (!ResolveExpressions(root, new_envs, false)) return false;
  }
  for(std::vector<Token>::iterator it=root->m_generated_tokens.begin(); it!=root->m_generated_tokens.end(); it++){
    if (!ResolveFieldDeclarations(&(*it), new_envs)) return false;
  }
  return true;
}

bool NameChecker::ResolveExpressions(Token* root, environment** envs, bool methodOrConstructor){
  TokenType t = root->type();

  envs[0]->force_merge(root->scope);

  environment next_local_env(*(envs[0]));
  environment* new_envs[4];

  new_envs[0] = &next_local_env;
  new_envs[1] = envs[1];
  new_envs[2] = envs[2];
  new_envs[3] = envs[3];

  /******************** COMMENTS ******************************
  std::cout << "TOKEN TYPE: " << root->m_display_name << std::endl;
  
  std::cout << "constructors: ";
  for(std::pair<std::string, std::vector<Token*>> kv_pair: new_envs[0]->constructors){
    std::cout << kv_pair.first;
  }
  std::cout << std::endl;
  std::cout << "methods: ";
  for(std::pair<std::string, std::vector<Token*>> kv_pair: new_envs[0]->methods){
    std::cout << kv_pair.first;
  }
  std::cout << std::endl;
  std::cout << "fields: ";
  for(std::pair<std::string, Token*> kv_pair: new_envs[0]->fields){
    std::cout << kv_pair.first;
  }
  std::cout << std::endl;
  std::cout << "localVariables: ";
  for(std::pair<std::string, Token*> kv_pair: new_envs[0]->localVariables){
    std::cout << kv_pair.first;
  }
  std::cout << std::endl;
  std::cout << "formalParameters: ";
  for(std::pair<std::string, Token*> kv_pair: new_envs[0]->formalParameters){
    std::cout << kv_pair.first;
  }
  std::cout << std::endl;

  ************************ COMMENTS END *******************/

    
  if (t == T_IDENTIFIER && !root->declaration){
    Token* declaration = nullptr;
    if (methodOrConstructor){
      std::vector<Token*> declarations = new_envs[0]->GetInvocationDeclaration(root->m_lex);
      if (declarations.size() == 0) {
        RED();
        std::cerr << "Error: cannot find method or constructor identifier: " << root->m_lex << std::endl;
        DEFAULT();
        return false;
      }
      else if (declarations.size() == 1) declaration = declarations[0];
      else {
        CYAN();
        std::cout << "method \"" << root->m_lex << "\" is overloaded, will have to do after type checking" << std::endl;
        DEFAULT();
      }
      methodOrConstructor = false;
    }
    else {
      declaration = new_envs[0]->GetDeclaration(root->m_lex);
      if (!declaration) {
        RED();
        std::cerr << "Error: cannot find variable: " << root->m_lex << std::endl;
        DEFAULT();
        return false;
      }
    }
    if (declaration){
      root->declaration = declaration;
      CYAN();
      std::cout << "Linked " << root->m_lex << " to " << declaration->m_display_name << std::endl;
      DEFAULT();
    }
  } else if (t!=QualifiedName) {
    for(std::vector<Token>::iterator it=root->m_generated_tokens.begin(); it!=root->m_generated_tokens.end(); it++){
      if (it->type() == T_DOT){
        break;
      }
      if (t == ExplicitConstructorInvocation || t == MethodInvocation || t == ClassInstanceCreationExpression || t == MethodDeclarator){
        methodOrConstructor = true;
      }
      if (t == FormalParameter || t == ArgumentList || (it-1)->type() == T_LEFT_ROUND_BRACKET){
        methodOrConstructor = false;
      }
      
      // constructor declarator blocks need to have access to all variables, fields, methods inside its parent class
      if (t == ConstructorDeclaration){
        new_envs[0]->merge(root->scope);
      }

      if (t == LocalVariableDeclarationStatement || t == FormalParameterList || t == FormalParameter || t == MethodDeclarator ||
          t == MethodHeader || t == ConstructorDeclarator){
        if (!ResolveExpressions(&(*it), envs, methodOrConstructor)) return false;
      }
      // imports, ArrayType, CastExpression are all types that will be resolved later
      if (t != SingleTypeImportDeclaration && t != TypeImportOnDemandDeclaration && t != PackageDeclaration &&
          t != ArrayType && t != CastExpression) {
        if (!ResolveExpressions(&(*it), new_envs, methodOrConstructor)) return false;
      }
    }
  }
  return true;
}

