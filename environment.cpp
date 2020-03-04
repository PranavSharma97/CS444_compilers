#include "environment.h"
#include "token.h"
#include "color_print.h"
#include <utility>
#include <iostream>
#include <sstream>
#include <string>

/*
 * Copy CTOR
 */
using namespace std;

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

/*
bool environment::valid_method(std::pair<std::string,std::vector<Token* >>& kv){
  if(methods.find(kv.first) == methods.end()){
    return true;
  } else {
    
    // check for method with different parameters
    return true;
  }
  return false;
}
*/
bool environment::valid_ctor(std::pair<std::string,std::vector<Token* >>& kv){
  if(constructors.find(kv.first) == constructors.end()){
    return true;
  } else {
    // check for ctors with different parameters
    return true;
  }
  return false;
}

/*
bool environment::replace_merge(environment& src){
  return true;
}
*/

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
    //if(valid_method(kv_pair)){
    if(true){
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
    //if(valid_method(kv_pair)){
    if(true){
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

void printList(std::map<std::string, Token*> list){
  for(std::pair<std::string, Token*> kv_pair: list){
    std::cout << kv_pair.first << kv_pair.second << ", ";
  }
  std::cout << std::endl;
}

Token* environment::GetDeclaration(std::string& name){
  Token *declaration = nullptr; 
  
  std::cout << "Looking for: " << name << std::endl;
  std::cout << "localVariables: ";
  printList(localVariables);
  std::cout << "fields: ";
  printList(fields);
  std::cout << "formalParameters: ";
  printList(formalParameters);
  
  if(localVariables.find(name) != localVariables.end()) declaration = localVariables[name];
  else if(fields.find(name) != fields.end()) declaration = fields[name];
  else if(formalParameters.find(name) != formalParameters.end()) declaration = fields[name];

  if (fields.find(name) != fields.end()){
    std::cout << "found name" << fields[name] << std::endl;
    declaration = localVariables[name];
  }
  if (fields[name] == nullptr){
    std::cout << "fields[name] is a nullptr" << std::endl;
  }
  return declaration;
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
  CYAN();;
  cout<<"\nPost processing method map\n";
  for(std::pair<std::string, std::vector<Token*>> kv_pair: methods){
    for(Token* n: kv_pair.second){
      std::string signature = "";
      Token* parameterList = n->SearchByTypeBFS(FormalParameterList);
      if (parameterList == nullptr) {
        cout<<"Parameter list is empty\n";
        DEFAULT();
        methodsWithSignatures[kv_pair.first][signature].push_back(n);
        continue;
      }
      
      CYAN();
      cout<<"Parameter list is non empty\n";
      DEFAULT();
      for(Token parameter: parameterList->m_generated_tokens) {
        if(parameter.m_type != FormalParameter) continue; 
        Token* arrayType = parameter.SearchByTypeDFS(ArrayType);
        if(arrayType) {
          cout<<"Type is arrayType\n";
          Token* id = arrayType->SearchByTypeDFS(T_IDENTIFIER);
          if(id) {
            cout<<"Identifer type array\n";
            const void * address = static_cast<const void*>(id->declaration);
            std::stringstream ss;
            ss << address;
            signature += ss.str();
            signature += "[]";
          } else if(arrayType->SearchByTypeDFS(QualifiedName)) {
            cout<<"Qualified Name type array\n";
            Token* qualifiedName = arrayType->SearchByTypeDFS(QualifiedName);
            const void * address = static_cast<const void*>(qualifiedName->declaration);
            std::stringstream ss;
            ss << address;
            signature += ss.str();
            signature += "[]";
          }
          else {
            cout<<"Primitive type array\n";
            signature += arrayType->m_generated_tokens[0].m_lex;
            signature += "[]";
          }
        } else {
          cout<<"Type is not arrayType\n";
          Token* id = parameter.m_generated_tokens[0].SearchByTypeDFS(T_IDENTIFIER);
          if(id) {
            cout<<"Identifier type\n";
            const void * address = static_cast<const void*>(id->declaration);
            std::stringstream ss;
            ss << address;
            signature += ss.str();
          } else if(parameter.m_generated_tokens[0].SearchByTypeDFS(QualifiedName)) {
            cout<<"Qualified Name type\n";
            Token* qualifiedName = parameter.SearchByTypeDFS(QualifiedName);
            const void * address = static_cast<const void*>(qualifiedName->declaration);
            std::stringstream ss;
            ss << address;
            signature += ss.str();
          }
          else {
            cout<<"Primitive type\n";
            cout<<"Need to access child of parameter:\n\n";
            if (parameter.m_generated_tokens.size() == 0) cout<<"OOPS parameter is empty\n";
            signature += parameter.m_generated_tokens[0].m_lex;
          }
        }
      }
     
      cout<<"Actual Signature is: \n";
      cout<<signature<<"\n";
      methodsWithSignatures[kv_pair.first][signature].push_back(n);
    }
  }
}

void environment::postProcessConstructorMap() {
  for(std::pair<std::string, std::vector<Token*>> kv_pair: constructors){
    for(Token* n: kv_pair.second){
      std::string signature = "";
      Token* parameterList = n->SearchByTypeBFS(FormalParameterList);
      if (parameterList == nullptr) {
        constructorsWithSignatures[kv_pair.first][signature].push_back(n);
        continue;
      }
      
      for(Token parameter: parameterList->m_generated_tokens) {
        if(parameter.m_type != FormalParameter) continue; 
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
  //cerr<<"Size of declared set is....."<<methodsWithSignaturesDeclared.size()<<endl;
  for(std::pair<std::string,std::map<std::string,std::vector<Token*>>> kv_pair: methodsWithSignaturesDeclared){
    for(std::pair<std::string, std::vector<Token*>> signatureToDeclarations: kv_pair.second) {
      if (signatureToDeclarations.second.size() > 1) {
        return false;
      }
      if(signatureToDeclarations.second[0]->Abstract == true) {
        if(signatureToDeclarations.second[0]->compilation_unit->SearchByTypeDFS(InterfaceDeclaration) == nullptr) {
          if(signatureToDeclarations.second[0]->compilation_unit->SearchByTypeDFS(ClassDeclaration)->m_generated_tokens[0].SearchByTypeBFS(T_ABSTRACT) == nullptr) {
            cerr<<"Abstract declared method, Class not abstract"<<endl;
            return false;
          }
        }
      }
    }
  }

  return true;
}

bool environment::checkConstructors() {
  for(std::pair<std::string,std::map<std::string,std::vector<Token*>>> kv_pair: constructorsWithSignatures){
    for(std::pair<std::string, std::vector<Token*>> signatureToDeclarations: kv_pair.second) {
      if (signatureToDeclarations.second.size() > 1) {
        return false;
      }
    }
  }
    return true;
}

bool environment::check_return_types(Token* src, Token* current) {  
  // Check for VOID return type
  if(src->SearchByTypeDFS(VOID) != nullptr) {
    if(current->SearchByTypeDFS(VOID) == nullptr) {
      std::cerr<<"Different return types: src void, current not"<<std::endl;
      return false;
    }
  } else if(current->SearchByTypeDFS(VOID) != nullptr) {
      std::cerr<<"Different return types: src non void, current VOID"<<std::endl;
      return false;
  }
  
  // Now check if return type is ArrayType
  Token* srcArrayType = src->m_generated_tokens[0].m_generated_tokens[1].SearchByTypeDFS(ArrayType);
  Token* currentArrayType = current->m_generated_tokens[0].m_generated_tokens[1].SearchByTypeDFS(ArrayType);
  // If any one of src or current is ArrayType
  if(srcArrayType || currentArrayType) {
    // If one of them is but the other is not, ERROR
    if(currentArrayType == nullptr) {
      std::cerr<<"Different return types: src not arrayType"<<std::endl;
      return false;
    }
    if(srcArrayType == nullptr) {
      std::cerr<<"Different return types: current not arrayType"<<std::endl;
      return false;
    }

    // Now check type of Arrays are the same
    // Can be either QualifiedName, SimpleName, or primitive type arrays
    Token* srcQualifiedName = srcArrayType->SearchByTypeDFS(QualifiedName);
    Token* currentQualifiedName = currentArrayType->SearchByTypeDFS(QualifiedName);
    
    // If both are QualifiedName arrays
    if(srcQualifiedName != nullptr && currentQualifiedName != nullptr) {
      if(srcQualifiedName->declaration != currentQualifiedName->declaration) {
        std::cerr<<"Different return types: DECLARATION MISMATCH for ArrayType"<<std::endl;
        return false;
      }
    }
    // Src is qualified, check for simple current
    else if(srcQualifiedName != nullptr) {
      Token* currentSimpleName = current->SearchByTypeDFS(T_IDENTIFIER);
      if(currentSimpleName == nullptr) {
        std::cerr<<"Different Array return types: current neither simple nor qualified"<<std::endl;
        return false;
      }
      if(currentSimpleName->declaration != srcQualifiedName->declaration) {
        std::cerr<<"Different Array return types: current SIMPLE, src qualified"<<std::endl;
        return false;
      }
    }
    // Current is qualified, check for simple src
    else if(currentQualifiedName != nullptr) {
      Token* srcSimpleName = src->SearchByTypeDFS(T_IDENTIFIER);
      if(srcSimpleName == nullptr) {
        std::cerr<<"Different Array return types: SRC neither simple nor qualified"<<std::endl;
        return false;
      }
      if(srcSimpleName->declaration != currentQualifiedName->declaration) {
        std::cerr<<"Different Array return types: src SIMPLE, current qualified"<<std::endl;
        return false;
      }
    }
    // Both are not qualified, so either Primitive type or SimpleName
    else {
      if((srcArrayType->m_generated_tokens[0].m_lex != currentArrayType->m_generated_tokens[0].m_lex) || (srcArrayType->m_generated_tokens[0].declaration != currentArrayType->m_generated_tokens[0].declaration)) {
        std::cerr<<"Different Array return types: Arraytype identifier"<<std::endl;
        return false;
      }
    }
  }
  // Type is not Array Type
  else {
    Token* srcQualifiedName = src->m_generated_tokens[0].m_generated_tokens[1].SearchByTypeDFS(QualifiedName);
    Token* currentQualifiedName = current->m_generated_tokens[0].m_generated_tokens[1].SearchByTypeDFS(QualifiedName);
    std::cout<<"Type of src: "<<src->m_generated_tokens[0].m_generated_tokens[1].m_type<<std::endl;
    std::cout<<"Type of current: "<<current->m_generated_tokens[0].m_generated_tokens[1].m_type<<std::endl;
    // If both are qualified name, check declaration
    if(srcQualifiedName != nullptr && currentQualifiedName != nullptr) {
      if(srcQualifiedName->declaration != currentQualifiedName->declaration) {
        std::cerr<<"Different return types: DECLARATION MISMATCH"<<std::endl;
        return false;
      }
    }
    // src is qualified, check for simple name of current
    else if(srcQualifiedName != nullptr) {
      Token* currentSimpleName = current->SearchByTypeDFS(T_IDENTIFIER);
      if(currentSimpleName == nullptr) {
        std::cerr<<"Different return types: current neither simple nor qualified"<<std::endl;
        return false;
      }
      if(currentSimpleName->declaration != srcQualifiedName->declaration) {
        //std::cerr<<"LEXEME OF SIMPLE: "<<srcSimpleName->m_lex<<std::endl;
        //std::cerr<<"LEXEME OF QUALIFIED: "<<currentQualifiedName->m_lex<<std::endl;
        std::cerr<<"Different return types: current SIMPLE, src qualified"<<std::endl;
        return false;
      }
    }
    // current is qualified name, check for simple name for src
    else if(currentQualifiedName != nullptr) {
      Token* srcSimpleName = src->SearchByTypeDFS(T_IDENTIFIER);
      if(srcSimpleName == nullptr) {
        std::cerr<<"Different return types: SRC neither simple nor qualified"<<std::endl;
        return false;
      }
      if(srcSimpleName->declaration != currentQualifiedName->declaration) {
        std::cerr<<"LEXEME OF SIMPLE: "<<srcSimpleName->m_lex<<std::endl;
        std::cerr<<"LEXEME OF QUALIFIED: "<<currentQualifiedName->m_lex<<std::endl;
        std::cerr<<"Different return types: src SIMPLE, current qualified"<<std::endl;
        return false;
      }
    }
    // Both are not qualified, so either PRIMITIVE type, or simple names
    else {
      Token srcId = src->m_generated_tokens[0].m_generated_tokens[1];
      Token currentId = current->m_generated_tokens[0].m_generated_tokens[1];
      if(srcId.m_type != currentId.m_type) {
        std::cerr<<"Different return types: (Identifier or primitive)"<<std::endl;
        return false;
      }

      if(srcId.m_type == T_IDENTIFIER) {
        if(srcId.m_lex != currentId.m_lex || srcId.declaration != currentId.declaration) {
          std::cerr<<"Method names ARE: "<<src->m_generated_tokens[0].m_generated_tokens[2].m_generated_tokens[0].m_lex<<" "<<current->m_generated_tokens[0].m_generated_tokens[2].m_generated_tokens[0].m_lex<<std::endl;
          std::cerr<<"IDENTIFIERS ARE: "<<srcId.m_lex<<" "<<currentId.m_lex<<std::endl;
          std::cerr<<"Declaration pointers ARE: "<<srcId.declaration<<" "<<currentId.declaration<<std::endl;
          std::cerr<<"Different return types: IDENTIFIERS different"<<std::endl;
          return false;
        }      
      }
    }
  }

  return true;
}

bool environment::valid_method(std::pair<std::string,std::map<std::string,std::vector<Token*>>>& srcMethod){
  
  for(std::pair<std::string, std::vector<Token*>> currentConstructor: constructors) {
    if(currentConstructor.second[0]->compilation_unit->SearchByTypeDFS(ClassDeclaration)) {
      //cerr<<"Class is: "<<currentConstructor.second[0]->compilation_unit->SearchByTypeDFS(ClassDeclaration)->m_generated_tokens[2].m_lex<<endl;
      //cerr<<"Src method is: "<<srcMethod.first<<endl;
      //cerr<<"Methods declared size is: "<<methodsWithSignaturesDeclared.size()<<endl;
    }
  }
  
  if(methodsWithSignaturesDeclared.size() >= 0) {
    // cerr<<"Methods declared size is: "<<methodsWithSignatures.size()<<endl;
     for(std::pair<std::string,std::map<std::string,std::vector<Token*>>> kv_pair: methodsWithSignaturesDeclared){
        //cerr<<"Name of method is: "<<kv_pair.first<<endl;
    }
  }
  // If method is not in declare set
  if(methodsWithSignaturesDeclared.find(srcMethod.first) == methodsWithSignaturesDeclared.end()){
    cout<<"Method is not in declared set\n";
    // Check if method is in inherited set
    if(methodsWithSignaturesInherited.find(srcMethod.first) != methodsWithSignaturesInherited.end()) {
      cout<<"Method is in inherited set\n";
      // Loop over all source signatures (Should be only 1 signature, since no class/interface can declare 2 methods with same signature
      for(std::pair<std::string, std::vector<Token*>> srcSignature: srcMethod.second) { 
        // Check if signatures match
        if(methodsWithSignaturesInherited[srcMethod.first].find(srcSignature.first) != methodsWithSignaturesInherited[srcMethod.first].end()) {
          // If src method is abstract, but inherited method is not
          cout<<"Found method with same signature....\n";
          if(srcSignature.second[0]->Abstract && !methodsWithSignaturesInherited[srcMethod.first][srcSignature.first][0]->Abstract) {
            // Check for return types (belongs to replace set)
            // This means method is replacing superclass method
            // So:
            // Check for return types
            // Check for both static or non static
            // Check if src method is public, current is public
            // Check src method is not final

            //std::cerr<<"REPLACING METHOD: Subclass name: "<<methodsWithSignaturesInherited[srcMethod.first][srcSignature.first][0]->compilation_unit->SearchByTypeDFS(ClassDeclaration)->m_generated_tokens[2].m_lex<<"\n";

            //std::cerr<<"REPLACING METHOD: Superclass name: "<<srcSignature.second[0]->compilation_unit->SearchByTypeDFS(InterfaceDeclaration)->m_generated_tokens[2].m_lex<<"\n";

            CYAN();
            
            cout<<"Source is abstract, current is not\n";
            DEFAULT();

            // Staticness
            YELLOW();
            cout<<"Checking for staticness\n";
            if(srcSignature.second[0]->m_generated_tokens[0].m_generated_tokens[0].SearchByTypeDFS(T_STATIC)) {
              if(!methodsWithSignaturesInherited[srcMethod.first][srcSignature.first][0]->m_generated_tokens[0].m_generated_tokens[0].SearchByTypeDFS(T_STATIC)) {
                std::cerr<<"Abstract static src, current non abstract non static"<<std::endl;
                return false;
              }
            }
            else {
              if(methodsWithSignaturesInherited[srcMethod.first][srcSignature.first][0]->m_generated_tokens[0].m_generated_tokens[0].SearchByTypeDFS(T_STATIC)) {
                std::cerr<<"Abstract non static src, current non abstract static"<<std::endl;
                return false;
              }
            }
            cout<<"Staticness PASSED\n";

            cout<<"Checking for public...\n";
            // Check public
            if(srcSignature.second[0]->m_generated_tokens[0].m_generated_tokens[0].SearchByTypeDFS(T_PUBLIC)) {
              if(!methodsWithSignaturesInherited[srcMethod.first][srcSignature.first][0]->m_generated_tokens[0].m_generated_tokens[0].SearchByTypeDFS(T_PUBLIC)) {
                std::cerr<<"Abstract public src, current non abstract non public"<<std::endl;
                return false;
              }
            }
            cout<<"Public check PASSED...\n";
            
            cout<<"Checking for FINAL...\n";
            // Check finalness
            if(srcSignature.second[0]->m_generated_tokens[0].m_generated_tokens[0].SearchByTypeDFS(T_FINAL)) {
              std::cerr<<"Abstract final src, current non abstract"<<std::endl;
              return false;
            }
            cout<<"Final check PASSED...\n";

            // Check for return types
           
            cout<<"Checking for RETURN types...\n";
            if(!this->check_return_types(srcSignature.second[0], methodsWithSignaturesInherited[srcMethod.first][srcSignature.first][0])) {
              //std::cerr<<"Name of src Method: "<<srcMethod.first<<std::endl;
              //std::cerr<<"Token Types are: "<<srcSignature.second[0]->m_type<<" "<<methodsWithSignaturesInherited[srcMethod.first][srcSignature.first][0]->m_type<<std::endl;
              //std::cerr<<"Class names are: "<<srcSignature.second[0]->compilation_unit->SearchByTypeDFS(InterfaceDeclaration)->m_generated_tokens[2].m_lex<<" "<<methodsWithSignaturesInherited[srcMethod.first][srcSignature.first][0]->compilation_unit->SearchByTypeDFS(ClassDeclaration)->m_generated_tokens[2].m_lex<<std::endl;
              std::cerr<<"Abstract src different return types"<<std::endl;
              return false;
            }
            cout<<"RETURN type check passed...\n";
            DEFAULT();
          }
          // If both src and inherited methods are abstract
          else if(srcSignature.second[0]->Abstract && methodsWithSignaturesInherited[srcMethod.first][srcSignature.first][0]->Abstract) {
            // Check for return types
            
            CYAN();

            cout<<"Both methods are abstract\n";
            DEFAULT();

            if(!this->check_return_types(srcSignature.second[0], methodsWithSignaturesInherited[srcMethod.first][srcSignature.first][0])) {
              std::cerr<<"Abstract src, abstract current different return types"<<std::endl;
              return false;
            }

          }
        }
      }

      // If signatures don't match but names match for inherited methods, it means an overloaded superclass method
      // add to inherit set
      for(std::pair<std::string, std::vector<Token*>> srcSignature: srcMethod.second) {
        // If inherited method is abstract, class must be abstract
        if(srcSignature.second[0]->Abstract == true) {
          //cerr<<"METHOD IS ABSTRACT, type of token is: "<<srcSignature.second[0]->m_type<<endl;
          //std::cerr<<"Name of src Method: "<<srcMethod.first<<std::endl;
          //std::cerr<<"Token Types are: "<<srcSignature.second[0]->m_type<<" "<<methodsWithSignaturesInherited[srcMethod.first][srcSignature.first][0]->m_type<<std::endl;
          //std::cerr<<"Class names are: "<<srcSignature.second[0]->compilation_unit->SearchByTypeDFS(InterfaceDeclaration)->m_generated_tokens[2].m_lex<<" "<<methodsWithSignaturesInherited[srcMethod.first][srcSignature.first][0]->compilation_unit->SearchByTypeDFS(ClassDeclaration)->m_generated_tokens[2].m_lex<<std::endl;

          for(std::pair<std::string, std::vector<Token*>> currentConstructor: constructors) {
            if(currentConstructor.second[0]->compilation_unit->SearchByTypeDFS(ClassDeclaration)->m_generated_tokens[0].SearchByTypeBFS(T_ABSTRACT) == nullptr) {
              std::cerr<<"Inheriting ABSTRACT method, class not ABSTRACT"<<std::endl;
              return false;
            }
          }
        }
        methodsWithSignaturesInherited[srcMethod.first][srcSignature.first] = srcSignature.second;
      }
      //if(!checkMethods()) { return false; }
      //if(!checkConstructors()) { return false; }
      return true;
    }

    // add to inherit set
    for(std::pair<std::string, std::vector<Token*>> srcSignature: srcMethod.second) {
      // If inherited method is abstract, class must be abstract
      if(srcSignature.second[0]->Abstract == true) {
        //cerr<<"METHOD IS ABSTRACT, type of token is: "<<srcSignature.second[0]->m_type<<endl;
        for(std::pair<std::string, std::vector<Token*>> currentConstructor: constructors) {
          if(currentConstructor.second[0]->compilation_unit->SearchByTypeDFS(ClassDeclaration)->m_generated_tokens[0].SearchByTypeBFS(T_ABSTRACT) == nullptr) {
            std::cerr<<"Inheriting ABSTRACT method, class not ABSTRACT"<<std::endl;
            return false;
          }
        }
      }
      methodsWithSignaturesInherited[srcMethod.first][srcSignature.first] = srcSignature.second;
    }

    //if(!checkMethods()) { return false; }
    //if(!checkConstructors()) { return false; }
    return true;
  } else {
    
    YELLOW();
    
    cout<<"Current class is: ";

    for(std::pair<std::string, std::vector<Token*>> srcSignature: srcMethod.second) {
      // Check if signatures match
      if(methodsWithSignaturesDeclared[srcMethod.first].find(srcSignature.first) != methodsWithSignaturesDeclared[srcMethod.first].end()) {
   
        // This means method is replacing superclass method
        // So:
        // Check for return types
        // Check for both static or non static
        // Check if src method is public, current is public
        // Check src method is not final
       
        //std::cerr<<"REPLACING METHOD: Subclass name: "<<methodsWithSignaturesDeclared[srcMethod.first][srcSignature.first][0]->compilation_unit->SearchByTypeDFS(ClassDeclaration)->m_generated_tokens[2].m_lex<<"\n";

        //std::cerr<<"REPLACING METHOD: Superclass name: "<<srcSignature.second[0]->compilation_unit->SearchByTypeDFS(ClassDeclaration)->m_generated_tokens[2].m_lex<<"\n";

        cout<<"METHOD IS REPLACING SUPERCLASS METHOD\n";

        cout<<"Printing children of src method\n";
        cout<<"Name is: ";
        //cout<<srcSignature.second[0]->m_generated_tokens[0].m_generated_tokens[2].m_generated_tokens[0].m_lex<<"\n";
        //Token* compilationUnit = srcSignature.second[0]->compilation_unit;
        //Token* classDeclaration = compilationUnit->SearchByTypeDFS(ClassDeclaration);
        //cout<<"Class name is: ";
        //cout<<classDeclaration->m_generated_tokens[2].m_lex<<"\n";

        // Check Staticness
        if(srcSignature.second[0]->m_generated_tokens[0].m_generated_tokens[0].SearchByTypeDFS(T_STATIC)) {
          cout<<"Source method is STATIC\n";
          if(!methodsWithSignaturesDeclared[srcMethod.first][srcSignature.first][0]->m_generated_tokens[0].m_generated_tokens[0].SearchByTypeDFS(T_STATIC)) {
            std::cerr<<"Inheritance ERROR: Source method is STATIC current is NOT"<<std::endl;
            return false;
          }
        }
        else {
          cout<<"Source method is not STATIC\n";
          if(methodsWithSignaturesDeclared[srcMethod.first][srcSignature.first][0]->m_generated_tokens[0].m_generated_tokens[0].SearchByTypeDFS(T_STATIC)) {
            std::cerr<<"Inheritance ERROR: Source method is not STATIC current is STATIC"<<std::endl;
            return false;
          }
        }

        // Check public
        if(srcSignature.second[0]->m_generated_tokens[0].m_generated_tokens[0].SearchByTypeDFS(T_PUBLIC)) {
          if(!methodsWithSignaturesDeclared[srcMethod.first][srcSignature.first][0]->m_generated_tokens[0].m_generated_tokens[0].SearchByTypeDFS(T_PUBLIC)) {
            std::cerr<<"Inheritance ERROR: Public error"<<std::endl;
            return false;
          }
        }

        // Check finalness
        if(srcSignature.second[0]->m_generated_tokens[0].m_generated_tokens[0].SearchByTypeDFS(T_FINAL)) {
          std::cerr<<"Inheritance ERROR: Final superclass error"<<std::endl;
          return false;
        }
        
        // Check return types
        if(!check_return_types(srcSignature.second[0], methodsWithSignaturesDeclared[srcMethod.first][srcSignature.first][0])) {
          if(methodsWithSignaturesDeclared[srcMethod.first][srcSignature.first].size() > 1) {
            std::cerr<<"\n\n\n BLOW UP EVERYTHING\n\n"<<std::endl;
          }
          std::cerr<<"Inheritance ERROR: Return types are different"<<std::endl;
          return false;
        }
      }
    } 
  }

  //if(!checkMethods()) { return false; }
  //if(!checkConstructors()) { return false; }
  return true;
}

void environment::build_declared_set() {
  cout<<"\nBuild Declared set was called\n";
  for(std::pair<std::string,std::map<std::string,std::vector<Token*>>> kv_pair: methodsWithSignatures){
    for(std::pair<std::string, std::vector<Token*>> srcSignature: kv_pair.second) {
      for(Token* t: srcSignature.second) {
        methodsWithSignaturesDeclared[kv_pair.first][srcSignature.first].emplace_back(t);
      }
    }
  }
  cout<<"\nSize of original set is: "<< methodsWithSignatures.size();
  cout<<"\nSize of declared set is: "<< methodsWithSignaturesDeclared.size();
}

bool environment::replace_merge(environment& src){

  RED();
  cout<<"Replace merge was called \n";
  DEFAULT();
  std::string className = "Thread"; 
  if(src.GetClass(className) == nullptr) {
    cout<<"Could not find Thread class\n";
  }
  for(std::pair<std::string,std::map<std::string,std::vector<Token*>>> kv_pair: src.methodsWithSignatures){
    RED();
    cout<<"Checking if method is valid in replace merge\n";
    DEFAULT();
    if(valid_method(kv_pair)){
      for(std::pair<std::string, std::vector<Token*>> srcSignature: kv_pair.second) {
        for(Token* t: srcSignature.second) {
          methodsWithSignatures[kv_pair.first][srcSignature.first].emplace_back(t);    
        }
      }
    }else{
      // Return error
      std::cerr<<"Replace merge error: Not valid method"<<std::endl;
      return false;
    }
  }
  
  // Merge fields
  for(std::pair<std::string, Token*> kv_pair: src.fields){
    if(fields.find(kv_pair.first) == fields.end()){
      fields[kv_pair.first] = kv_pair.second;
    }
  }
  return true;
}
