#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <experimental/filesystem>

#include "generate_code.h"
#include "token.h"
#include "token_types.h"
#include "package.h"
#include "color_print.h"

void saveRegister(std::ofstream &file, std::string reg){
  file << "push " << reg << std::endl;
}

void saveRegisters(std::ofstream &file){
  saveRegister(file, "ebp");
  file << "mov ebp, esp" << std::endl;
  file << "sub esp, 12" << std::endl;
  saveRegister(file, "ebx");
  saveRegister(file, "esi");
  saveRegister(file, "edi");
}

void restoreRegister(std::ofstream &file, std::string reg){
  file << "pop " << reg << std::endl;
}

void restoreRegisters(std::ofstream &file){
  restoreRegister(file, "edi");
  restoreRegister(file, "esi");
  restoreRegister(file, "ebx");
  file << "mov esp, ebp" << std::endl;
  restoreRegister(file, "ebp");
}

int numOfFormalParameters(Token *ListToken){
  int level = 1;
  while (ListToken->m_generated_tokens.size() > 0){
    ListToken = &(ListToken->m_generated_tokens[0]);
    level += 1;
  }
  return level;
}

void setLocationForFormalParameterList(Token *ListToken, int offset){
  for(Token& n: ListToken->m_generated_tokens){
    if (n.m_type == FormalParameter){
      n.loc = "ebp + " + std::to_string(offset);
    }
    else{
      offset -= 4;
      setLocationForFormalParameterList(&n, offset);
    }
  }
}

void GenerateCode::pushArguments(std::ofstream &file, Token *argumentList){
  for(Token& n: argumentList->m_generated_tokens){
    if (n.m_type == ArgumentList){
      pushArguments(file, &n);
    }
    else if (n.m_type == T_COMMA){
      file << "push ebx" << std::endl;
    }
    else{
      generateToken(file, &n);
    }
  }
}

Token *getDeclaration(Token *name){
  if (name->declaration) return name->declaration;
  for(Token& n: name->m_generated_tokens){
    Token *result = getDeclaration(&n);
    if (result) return result;
  }
  return nullptr;
}

void GenerateCode::generateToken(std::ofstream &file, Token *t){
  if (t->m_type == MethodDeclaration){
    Token *methodDeclarator =  t->SearchByTypeDFS(MethodDeclarator);

    // currently using declaration as method label
    Token *identifierToken = &(methodDeclarator->m_generated_tokens[0]);
    file << ";" << identifierToken->m_lex << std::endl;
    file << identifierToken->declaration << ":" << std::endl;
    saveRegisters(file);
    Token *formalParams = methodDeclarator->SearchByTypeDFS(FormalParameterList);
    if (formalParams){
      int offset = numOfFormalParameters(formalParams)*4+4;
      setLocationForFormalParameterList(formalParams,offset);
    }
    bool skipHeader = true;
    for(Token& n: t->m_generated_tokens){
      if (!skipHeader) generateToken(file, &n);
      skipHeader = false;
    }
    restoreRegisters(file);
    file << "ret" << std::endl;
   }

  else if (t->m_type == MethodInvocation){
    Token *argumentList = t->SearchByTypeDFS(ArgumentList);
    if (argumentList) { 
      pushArguments(file, argumentList);
      file << "push ebx" << std::endl;
    }
    Token *labelToken = getDeclaration(&(t->m_generated_tokens[0]));
    file << "call " << labelToken << std::endl;
    // currently method declarations are not connected
    if (labelToken) {
      Token *formalParameterList = labelToken->SearchByTypeDFS(FormalParameterList);
      if (formalParameterList) {
        int offset = numOfFormalParameters(formalParameterList)*4;
        file << "add esp, " << offset << std::endl;
      }
    }
    file << "mov eax, ebx" << std::endl;
  }
  
  else if (t->m_type == INT_LITERAL){
    file << "mov ebx, " << t->m_lex << std::endl;
  }
  else if (t->m_type == T_IDENTIFIER){
    if (t->declaration) file << "mov ebx, [" << t->declaration->loc << "]" << std::endl;
  }
  else if (t->m_type == ReturnStatement){
    if (t->m_generated_tokens[1].m_generated_tokens.size() == 0){
      generateToken(file, &(t->m_generated_tokens[1]));
      file << "mov eax, ebx" << std::endl;
    }
    else {
      generateToken(file, &(t->m_generated_tokens[1]));
    }
  }

  else {
    for(Token& n: t->m_generated_tokens){
      generateToken(file, &n);
    }
  }
}

int GenerateCode::Generate(){
  system("rm -r ./output");
  system("mkdir output");
  std::string fileName = "fileName";
  int index = 0;
  for(Token* tree_ptr: tree_ptrs){
    std::ofstream file("output/"+fileName+std::to_string(index)+".s");
    if (index == 0){
      file << "global _start" << std::endl;
      file << "_start:" << std::endl;
    }
    generateToken(file, tree_ptr);
    file.close();
    index++;
  }
}

GenerateCode::GenerateCode(const std::vector<Token*>& ptrs){
  tree_ptrs = ptrs;
}

GenerateCode::~GenerateCode(){
}
