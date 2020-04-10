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
  file << "mov " << reg << ", esp" << std::endl;
  file << "sub esp, 12" << std::endl;
}

void saveRegisters(std::ofstream &file){
  saveRegister(file, "ebp");
  saveRegister(file, "esp");
  saveRegister(file, "ebx");
  saveRegister(file, "esi");
  saveRegister(file, "edi");
}

void restoreRegister(std::ofstream &file, std::string reg){
  file << "mov esp, " << reg << std::endl;
  file << "pop " << reg << std::endl;
}

void restoreRegisters(std::ofstream &file){
  restoreRegister(file, "edi");
  restoreRegister(file, "esi");
  restoreRegister(file, "ebx");
  restoreRegister(file, "esp");
  restoreRegister(file, "ebp");
}

void generateToken(std::ofstream &file, Token *t){
  if (t->m_type == MethodDeclarator){
    // currently using declaration as method label
    Token *identifierToken = &(t->m_generated_tokens[0]);
    file << identifierToken->declaration << ":" << std::endl;
    saveRegisters(file);
    for(Token& n: t->m_generated_tokens){
      generateToken(file, &n);
    }
    restoreRegisters(file);
  }

  else if (t->m_type == FormalParameterList){
  }

  for(Token& n: t->m_generated_tokens){
    generateToken(file, &n);
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
