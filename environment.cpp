#include "environment.h"
#include "token.h"
#include "color_print.h"
#include <utility>
#include <iostream>
#include <sstream>

/*
 * Copy CTOR
 */

environment::environment(const environment& other)/*:
  classes(other.classes.size()),
  interfaces(other.interfaces.size()),
  fields(other.fields.size()),
  methods(other.methods.size()),
  localVariables(other.localVariables.size()),
  formalParameters(other.formalParameters.size()),
  constructors(other.constructors.size())*/
{
  for(std::pair<std::string,Token*> kv_pair: other.classes){
    classes[kv_pair.first] = kv_pair.second;
  }

  for(std::pair<std::string,Token*> kv_pair: other.interfaces){
    interfaces[kv_pair.first] = kv_pair.second; //other.interfaces[kv_pair.first];
  }

  for(std::pair<std::string,Token*> kv_pair: other.fields){
    fields[kv_pair.first] = kv_pair.second;//other.fields[kv_pair.first];
  }

  for(std::pair<std::string,std::vector<Token*>> kv_pair: other.methods){
    std::vector<Token*> new_vec;
    methods[kv_pair.first] = new_vec;
    for(Token* t: kv_pair.second){
      methods[kv_pair.first].emplace_back(t);
    }
  }

  for(std::pair<std::string,Token*> kv_pair: other.localVariables){
    localVariables[kv_pair.first] = kv_pair.second;//other.localVariables[kv_pair.first];
  }
  
  for(std::pair<std::string,Token*> kv_pair: other.formalParameters){
    formalParameters[kv_pair.first] = kv_pair.second;//other.formalParameters[kv_pair.first];
  }

  for(std::pair<std::string,std::vector<Token*>> kv_pair: other.constructors){
    std::vector<Token*> new_vec;
    constructors[kv_pair.first] = new_vec;
    for(Token* t: kv_pair.second){
      constructors[kv_pair.first].emplace_back(t);
    }
  }
  
  
  
}

Token* GetNodeByType(std::vector<Token*>& nodes, TokenType type){
  for(Token* node: nodes){
    if(node->type() == type) return node;
  }
  return nullptr;
}

bool environment::valid_method(std::pair<std::string,std::vector<Token* >>& kv){
  if(methods.find(kv.first) == methods.end()){
    return true;
  } else {
    
    // check for method with different parameters
    return true;
  }
  return false;
}

bool environment::valid_ctor(std::pair<std::string,std::vector<Token* >>& kv){
  if(constructors.find(kv.first) == constructors.end()){
    return true;
  } else {
    // check for ctors with different parameters
    return true;
  }
  return false;
}

bool environment::replace_merge(environment& src){
  return true;
}

// Check for self import
bool environment::check_exist(Token* t){
  for(std::pair<std::string, Token*> kv: classes){
    if(kv.second == t) return true;
  }
  for(std::pair<std::string, Token*> kv: interfaces){
    if(kv.second == t) return true;
  }
  return false;
}
bool environment::merge(environment src,Token** clash_token){
  // Merge classes
  int counter = 0;
  /*YELLOW();
  std::cout<<"MERGING ... " <<std::endl;
  DEFAULT();
  std::cout<<"STEP 1"<<std::endl;
  */
  for(std::pair<std::string, Token*> kv_pair: src.classes){
    // std::cout<<"STEP 2:"<<kv_pair.first<<std::endl;
    if(classes.find(kv_pair.first) == classes.end() &&
       interfaces.find(kv_pair.first) == interfaces.end()){
       
      //std::cout<<"CLASS ENV: Added ["<<kv_pair.first<<","<<kv_pair.second;
      //std::cout<<","<<*(kv_pair.second)<<"] "<<counter++<<std::endl;
      classes[kv_pair.first] = kv_pair.second;
    }else{
      // Get the clashed token for caller to check for self import
      if(clash_token!=nullptr){
	if(classes.find(kv_pair.first) != classes.end()){
	  *clash_token = src.classes[kv_pair.first];
	}else{
	  *clash_token = src.interfaces[kv_pair.first];
	}
      }
      //RED();
      //std::cerr<<"TYPE LINKER ERROR: Class: "<<kv_pair.first;
      //std::cerr<<" already defined"<<std::endl;
      //DEFAULT();
      return false;
    }
  }
  // Merge interfaces
  for(std::pair<std::string, Token*> kv_pair: src.interfaces){
    if(classes.find(kv_pair.first) == classes.end() &&
       interfaces.find(kv_pair.first) == interfaces.end()){
      
      //std::cout<<"INTERFACE ENV: Added ["<<kv_pair.first<<","<<(*(kv_pair.second));
      //std::cout<<","<<kv_pair.second->m_lex<<"]"<<std::endl;
      interfaces[kv_pair.first] = kv_pair.second;
    }else{
      if(clash_token!=nullptr){
	if(classes.find(kv_pair.first) != classes.end()){
	  *clash_token = src.classes[kv_pair.first];
	}else{
	  *clash_token = src.interfaces[kv_pair.first];
	}
      }
      /*
      RED();
      std::cerr<<"TYPE LINKER ERROR: Interface: "<<kv_pair.first;
      std::cerr<<" already defined"<<std::endl;
      DEFAULT();
      */
      return false;
    }
  }
  
  // Merge methods, current doesn't check the methods with same parameters
  for(std::pair<std::string, std::vector<Token*>> kv_pair: src.methods){
    if(valid_method(kv_pair)){
      for(Token* n: kv_pair.second){
	
	//std::cout<<"METHOD ENV: Added ["<<kv_pair.first<<","<<(*n);
	//std::cout<<","<<n->m_lex<<"]"<<std::endl;
	methods[kv_pair.first].emplace_back(n);
      }
    }else{
      if(clash_token!=nullptr){
	if(classes.find(kv_pair.first) != classes.end()){
	  *clash_token = src.classes[kv_pair.first];
	}else{
	  *clash_token = src.interfaces[kv_pair.first];
	}
      }
      RED();
      std::cerr<<"TYPE LINKER ERROR: method: "<<kv_pair.first;
      std::cerr<<" already declared"<<std::endl;
      DEFAULT();
      return false;
    }
  }

  // Merge Ctors
  for(std::pair<std::string, std::vector<Token*>> kv_pair: src.constructors){
    if(valid_ctor(kv_pair)){
      for(Token* n: kv_pair.second){
	
	//std::cout<<"CTOR ENV: Added ["<<kv_pair.first<<","<<(*n);
	//std::cout<<","<<n->m_lex<<"]"<<std::endl;
	constructors[kv_pair.first].emplace_back(n);
      }
    }else{
      RED();
      std::cerr<<"TYPE LINKER ERROR: method: "<<kv_pair.first;
      std::cerr<<" already declared"<<std::endl;
      DEFAULT();
      return false;
    }
  }
  
  // Merge formal Parameters
  for(std::pair<std::string, Token*> kv_pair: src.formalParameters){
    if(formalParameters.find(kv_pair.first) == formalParameters.end() &&
       localVariables.find(kv_pair.first) == localVariables.end()){
      formalParameters[kv_pair.first] = kv_pair.second;
    }else{
      RED();
      std::cerr<<"TYPE LINKER ERROR: parameter: "<<kv_pair.first;
      std::cerr<<" duplicated."<<std::endl;
      DEFAULT();
      return false;
    }
  }

  // Merge fields
  for(std::pair<std::string, Token*> kv_pair: src.fields){
    if(fields.find(kv_pair.first) == fields.end()){
      fields[kv_pair.first] = kv_pair.second;
    }else{
      RED();
      std::cerr<<"TYPE LINKER ERROR: field: "<<kv_pair.first;
      std::cerr<<" already declared"<<std::endl;
      DEFAULT();
      return false;
    }
  }

  // Merge localVariables
  for(std::pair<std::string, Token*> kv_pair: src.localVariables){
    if(formalParameters.find(kv_pair.first) == formalParameters.end() &&
       localVariables.find(kv_pair.first) == localVariables.end()){
      localVariables[kv_pair.first] = kv_pair.second;
    }else{
      RED();
      std::cerr<<"TYPE LINKER ERROR: variable: "<<kv_pair.first;
      std::cerr<<" already declared"<<std::endl;
      DEFAULT();
      return false;
    }
  }
  return true;
}

void environment::overwrite_merge(environment& src){
  // Merge classes
  for(std::pair<std::string, Token*> kv_pair: src.classes){
    if(classes.find(kv_pair.first) == classes.end() &&
       interfaces.find(kv_pair.first) == interfaces.end()){
      classes[kv_pair.first] = kv_pair.second;
    }else{
      classes[kv_pair.first] = nullptr;
    }
  }
  // Merge interfaces
  for(std::pair<std::string, Token*> kv_pair: src.interfaces){
    if(classes.find(kv_pair.first) == classes.end() &&
       interfaces.find(kv_pair.first) == interfaces.end()){
      interfaces[kv_pair.first] = kv_pair.second;
    }else{
      interfaces[kv_pair.first] = nullptr;
    }
  }
  
  // Merge methods, current doesn't check the methods with same parameters
  for(std::pair<std::string, std::vector<Token*>> kv_pair: src.methods){
    if(valid_method(kv_pair)){
      for(Token* n: kv_pair.second){
	methods[kv_pair.first].emplace_back(n);
      }
    }else{
      std::vector<Token*> new_vec;
      methods[kv_pair.first] = new_vec;
    }
  }

  // Merge Ctors
  for(std::pair<std::string, std::vector<Token*>> kv_pair: src.constructors){
    if(valid_ctor(kv_pair)){
      for(Token* n: kv_pair.second){
	constructors[kv_pair.first].emplace_back(n);
      }
    }else{ 
      std::vector<Token*> new_vec;
      constructors[kv_pair.first] = new_vec;
     
    }
  }
  
  // Merge formal Parameters
  for(std::pair<std::string, Token*> kv_pair: src.formalParameters){
    if(formalParameters.find(kv_pair.first) == formalParameters.end() &&
       localVariables.find(kv_pair.first) == localVariables.end()){
      formalParameters[kv_pair.first] = kv_pair.second;
    }else{
      formalParameters[kv_pair.first] = nullptr;
    }
  }

  // Merge fields
  for(std::pair<std::string, Token*> kv_pair: src.fields){
    if(fields.find(kv_pair.first) == fields.end()){
      fields[kv_pair.first] = kv_pair.second;
    }else{
      fields[kv_pair.first] = nullptr;
    }
  }

  // Merge localVariables
  for(std::pair<std::string, Token*> kv_pair: src.localVariables){
    if(formalParameters.find(kv_pair.first) == formalParameters.end() &&
       localVariables.find(kv_pair.first) == localVariables.end()){
      localVariables[kv_pair.first] = kv_pair.second;
    }else{
      localVariables[kv_pair.first] = nullptr;
    }
  }
}

Token* environment::GetType(std::string & name){
  
  if(classes.find(name) == classes.end()){
    if(interfaces.find(name) == interfaces.end()){
      return nullptr;
    }
    return ((Token*) interfaces[name]);
  }
  return ((Token*) classes[name]);
}

Token* environment::GetClass(std::string & name){
  if(classes.find(name) == classes.end()) return nullptr;
  return classes[name];
}

Token* environment::GetInterface(std::string& name){
  if(interfaces.find(name) == interfaces.end()) return nullptr;
  return interfaces[name];
}

void environment::clear(){
  classes.clear();
  interfaces.clear();
  fields.clear();
  methods.clear();
  localVariables.clear();
  formalParameters.clear();
  constructors.clear();
}

void environment::postProcessMethodMap() {
  for(std::pair<std::string, std::vector<Token*>> kv_pair: methods){
    for(Token* n: kv_pair.second){
      string signature = "";
      Token* parameterList = n->SearchByTypeBFS(FormalParameterList);
      if (parameterList == nullptr) {
        methodsWithSignatures[kv_pair.first][signature].push_back(n);
        continue;
      }
      
      for(Token parameter: parameterList->m_generated_tokens) {
        Token* arrayType = parameter.SearchByTypeDFS(ArrayType);
        if(arrayType) {
          Token* id = arrayType->SearchByTypeDFS(T_IDENTIFIER);
          if(id) {
            const void * address = static_cast<const void*>(id->declaration);
            std::stringstream ss;
            ss << address;
            signature += ss.str();
            signature += "[]";
          } else if(arrayType->SearchByTypeDFS(QualifiedName)) {
            Token* qualifiedName = arrayType->SearchByTypeDFS(QualifiedName);
            const void * address = static_cast<const void*>(qualifiedName->declaration);
            std::stringstream ss;
            ss << address;
            signature += ss.str();
            signature += "[]";
          }
          else {
            signature += arrayType->m_generated_tokens[0].m_lex;
            signature += "[]";
          }
        } else {
          Token* id = parameter.SearchByTypeDFS(T_IDENTIFIER);
          if(id) {
            const void * address = static_cast<const void*>(id->declaration);
            std::stringstream ss;
            ss << address;
            signature += ss.str();
          } else if(parameter.SearchByTypeDFS(QualifiedName)) {
            Token* qualifiedName = parameter.SearchByTypeDFS(QualifiedName);
            const void * address = static_cast<const void*>(qualifiedName->declaration);
            std::stringstream ss;
            ss << address;
            signature += ss.str();
          }
          else {
            signature += parameter.m_generated_tokens[0].m_lex;
          }
        }
      }
      
      methodsWithSignatures[kv_pair.first][signature].push_back(n);
    }
  }
}

void environment::postProcessConstructorMap() {
  for(std::pair<std::string, std::vector<Token*>> kv_pair: constructors){
    for(Token* n: kv_pair.second){
      string signature = "";
      Token* parameterList = n->SearchByTypeBFS(FormalParameterList);
      if (parameterList == nullptr) {
        constructorsWithSignatures[kv_pair.first][signature].push_back(n);
        continue;
      }
      
      for(Token parameter: parameterList->m_generated_tokens) {
        Token* arrayType = parameter.SearchByTypeDFS(ArrayType);
        if(arrayType) {
          Token* id = arrayType->SearchByTypeDFS(T_IDENTIFIER);
          if(id) {
            const void * address = static_cast<const void*>(id->declaration);
            std::stringstream ss;
            ss << address;
            signature += ss.str();
            signature += "[]";
          } else if(arrayType->SearchByTypeDFS(QualifiedName)) {
            Token* qualifiedName = arrayType->SearchByTypeDFS(QualifiedName);
            const void * address = static_cast<const void*>(qualifiedName->declaration);
            std::stringstream ss;
            ss << address;
            signature += ss.str();
            signature += "[]";
          }
          else {
            signature += arrayType->m_generated_tokens[0].m_lex;
            signature += "[]";
          }
        } else {
          Token* id = parameter.SearchByTypeDFS(T_IDENTIFIER);
          if(id) {
            const void * address = static_cast<const void*>(id->declaration);
            std::stringstream ss;
            ss << address;
            signature += ss.str();
          } else if(parameter.SearchByTypeDFS(QualifiedName)) {
            Token* qualifiedName = parameter.SearchByTypeDFS(QualifiedName);
            const void * address = static_cast<const void*>(qualifiedName->declaration);
            std::stringstream ss;
            ss << address;
            signature += ss.str();
          }
          else {
            signature += parameter.m_generated_tokens[0].m_lex;
          }
        }
      }
      
      constructorsWithSignatures[kv_pair.first][signature].push_back(n);
    }
  }
}

bool environment::checkMethods() {
  for(std::map<std::string,std::map<std::string,std::vector<Token*>>> kv_pair: methodsWithSignatures){
    for(std::pair<std::string, std::vector<Token*>> signatureToDeclarations: kv_pair.second) {
      if (signatureToDeclarations.second.size() > 1) {
        return false;
      }
    }

    return true;
}

bool environment::checkConstructors() {
  for(std::map<std::string,std::map<std::string,std::vector<Token*>>> kv_pair: constructorsWithSignatures){
    for(std::pair<std::string, std::vector<Token*>> signatureToDeclarations: kv_pair.second) {
      if (signatureToDeclarations.second.size() > 1) {
        return false;
      }
    }
    
    return true;
}

bool environment::valid_method(std::map<std::string,std::map<std::string,std::vector<Token*>>>& srcMethod){
  // If method is not in declare set
  if(methodsWithSignaturesDeclared.find(srcMethod.first) == methodsWithSignaturesDeclared.end()){
    // Check if method is in inherited set
    if(methodsWithSignaturesInherited.find(srcMethod.first) != methodsWithSignaturesInherited.end()) {
      // Loop over all source signatures (Should be only 1 signature, since no class/interface can declare 2 methods with same signature
      for(std::pair<std::string, std::vector<Token*>> srcSignature: srcMethod.second) { 
        // Check if signatures match
        if(methodsWithSignaturesInherited[srcMethod.first].find(srcSignature.first) != methodsWithSignaturesInherited[srcMethod.first].end()) {
          // If src method is abstract, but inherited method is not  
          if(srcSignature.second[0].abstract && !methodsWithSignaturesInherited[srcMethod.first][srcSignature.first][0].abstract) {
            // Check for return types (belongs to replace set)
            // This means method is replacing superclass method
            // So:
            // Check for return types
            // Check for both static or non static
            // Check if src method is public, current is public
            // Check src method is not final
            
            if(srcSignature.second[0]->searchByTypeDFS(Modifiers

            // Check for return types
            Token* srcArrayType = srcSignature.second[0]->searchByTypeDFS(ArrayType);
            Token* currentArrayType = methodsWithSignaturesInherited[srcMethod.first][srcSignature.first][0]->searchByTypeDFS(ArrayType);
            if(srcArrayType || currentArrayType) {
              if(currentArrayType == nullptr) {
                return false;
              }
              if(srcArrayType == nullptr) {
                return false;
              }
              Token* srcQualifiedName = srcArrayType->searchByTypeDFS(QualifiedName);
              Token* currentQualifiedName = currentArrayType->searchByTypeDFS(QualifiedName);
              if(srcQualifiedName || currentQualifiedName) {
                if(!currentQualifiedName) { return false; }
                if(!srcQualifiedName) { return false; }
                if(srcQualifiedName->declaration != currentQualifiedName->declaration) {
                  return false;
                }
              }
              else {
                if(srcArrayType->m_generated_tokens[0].m_lex != currentArrayType->m_generated_tokens[0].m_lex) {
                  return false;
                }
              }
            }
            else {
              Token* srcQualifiedName = srcSignature.second[0]->searchByTypeDFS(QualifiedName);
              Token* currentQualifiedName = methodsWithSignaturesInherited[srcMethod.first][srcSignature.first][0]->searchByTypeDFS(QualifiedName);
              if(srcQualifiedName || currentQualifiedName) {
                if(!currentQualifiedName) { return false; }
                if(!srcQualifiedName) { return false; }
                if(srcQualifiedName->declaration != currentQualifiedName->declaration) {
                  return false;
                }
              }
              else {
                if(srcArrayType->m_generated_tokens[0].m_lex != currentArrayType->m_generated_tokens[0].m_lex) {
                  return false;
                }
              }
            }

          }
          // If both src and inherited methods are abstract
          else if(srcSignature.second[0].abstract && methodsWithSignaturesInherited[srcMethod.first][srcSignature.first][0].abstract) {
            // Check for return types
            Token* srcArrayType = srcSignature.second[0]->searchByTypeDFS(ArrayType);
            Token* currentArrayType = methodsWithSignaturesInherited[srcMethod.first][srcSignature.first][0]->searchByTypeDFS(ArrayType);
            if(srcArrayType || currentArrayType) {
              if(currentArrayType == nullptr) {
                return false;
              }
              if(srcArrayType == nullptr) {
                return false;
              }
              Token* srcQualifiedName = srcArrayType->searchByTypeDFS(QualifiedName);
              Token* currentQualifiedName = currentArrayType->searchByTypeDFS(QualifiedName);
              if(srcQualifiedName || currentQualifiedName) {
                if(!currentQualifiedName) { return false; }
                if(!srcQualifiedName) { return false; }
                if(srcQualifiedName->declaration != currentQualifiedName->declaration) {
                  return false;
                }
              }
              else {
                if(srcArrayType->m_generated_tokens[0].m_lex != currentArrayType->m_generated_tokens[0].m_lex) {
                  return false;
                }
              }
            }
            else {
              Token* srcQualifiedName = srcSignature.second[0]->searchByTypeDFS(QualifiedName);
              Token* currentQualifiedName = methodsWithSignaturesInherited[srcMethod.first][srcSignature.first][0]->searchByTypeDFS(QualifiedName);
              if(srcQualifiedName || currentQualifiedName) {
                if(!currentQualifiedName) { return false; }
                if(!srcQualifiedName) { return false; }
                if(srcQualifiedName->declaration != currentQualifiedName->declaration) {
                  return false;
                }
              }
              else {
                if(srcArrayType->m_generated_tokens[0].m_lex != currentArrayType->m_generated_tokens[0].m_lex) {
                  return false;
                }
              }    
            }
          }
        }
      }

      // Default case, if signatures don't match but names match for inherited methods
      // add to inherit set
      return true
    }
    // add to inherit set
    return true;
  } else {
    // This means method is replacing superclass method
    // So:
    // Check for return types
    // Check for both static or non static
    // Check if src method is public, current is public
    // Check src method is not final
  }

  if(methodsWithSignatures.find(srcMethod.first) != methodsWithSignatures.end()){
    for(std::pair<std::string, std::vector<Token*>> srcSignature: srcMethod.second) {
      if(methodsWithSignatures[srcMethod.first].find(srcSignature.first) != methodsWithSignatures[srcMethod.first].end()) {
        if(srcSignature.second[0].abstract && srcSignature.second[0].m_type == AbstractMethodHeader && !methodsWithSignatures[srcMethod.first][srcSignature.first][0].abstract && methodsWithSignatures[srcMethod.first][srcSignature.first][0].m_type == MethodDeclaration) {
        // Do nothing
          if(inheritedClassMethod == true) {
            // check return types (for replacing method)
          }
        }
        else if(srcSignature.second[0].abstract && srcSignature.second[0].m_type == AbstractMethodHeader && methodsWithSignatures[srcMethod.first][srcSignature.first][0].m_type == AbstractMethodHeader) {
        // Check return types

        }
        else if(srcSignature.second[0].abstract && srcSignature.second[0].m_type == AbstractMethodHeader && methodsWithSignatures[srcMethod.first][srcSignature.first][0].m_type == MethodDeclaration && flag == on) {
          // check return types (for inherited abstract methods)
        }
        else {
          // check both static or non static
          // check src method is not final
          // check return types
          // check if src method is public, current is public
        }
      }
    }
  } else { 
    // If method was methodDeclaration and abstract, then flag = on, so check for return types for inherited AbstractMethodDeclaration
    // Put method in inherited methods
    return true;
  }
  return false;
}

bool environment::replace_merge(environment& src){
   // Merge methods, current doesn't check the methods with same parameters
  for(std::map<std::string,std::map<std::string,std::vector<Token*>>> kv_pair: src.methodsWithSignatures){
    if(valid_method(kv_pair)){
      for(Token* n: kv_pair.second){
        methods[kv_pair.first].emplace_back(n);
      }
    }else{
      // Return error
      std::vector<Token*> new_vec;
      methods[kv_pair.first] = new_vec;
    }
  }
  
  // Merge fields
  for(std::pair<std::string, Token*> kv_pair: src.fields){
    if(fields.find(kv_pair.first) == fields.end()){
      fields[kv_pair.first] = kv_pair.second;
  }

  return true;
}

