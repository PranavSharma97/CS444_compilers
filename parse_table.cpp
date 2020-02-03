#include "parse_table.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <exception>
#include <stdexcept>

ParseTable::ParseTable():
  m_states_count(-1)
{
  try{
    readin("parse_table.lr1");
  } catch (const std::exception& e){
    std::cerr<<"ERROR: Parse Table Readin Failed. "<<e.what()<<std::endl;
  }
}

ParseTable::ParseTable(std::string& input_file):
  m_states_count(-1)
{
  try{
    readin(input_file);
  } catch (const std::exception& e){
    std::cerr<<"ERROR: Parse Table Readin Failed."<<e.what()<<std::endl;
  }
}

void ParseTable::readin(std::string input_file){
  std::ifstream token_ifs("token_str_to_num.txt");
   
  // Read in token string to number converting table
  std::string err_msg = "Cannot open token_str_to_num.txt";
  if(!token_ifs.is_open()) throw std::runtime_error(err_msg);

  std::string token_name = "";
  TokenType t;
  int num = 0;

  std::map<std::string, TokenType> token_table;
  for(unsigned int line=0;;line++){
    if(token_ifs.eof()){
      token_ifs.close();
      break;
    }
    
    try{
      token_ifs>>token_name;
      token_ifs>>num;
      t = static_cast<TokenType>(num);
      token_table[token_name] = t;
      token_name.clear();
    } catch (const std::exception& e) {
      token_ifs.close();
      // TODO: Add line count to exception message
      throw e;
    }
  }

  
  // Read in parse table file
  
  std::ifstream ifs(input_file);
  if(!ifs.is_open()) {
    std::string err_message = "Cannot open ";
    err_message += input_file;
    throw std::runtime_error(err_message);
  }

  unsigned int line = 0;


  // Read In accepting states
  try{
    ifs>>num;
    line ++;

    for(int i = 0; i<num; i++,line++){
      token_name.clear();
      ifs>>token_name;
      // Check if the token name maps to an actual token type.
      if(token_table.find(token_name) == token_table.end()){
	std::string err = token_name;
	err += " does not exist.";
	ifs.close();
	throw std::out_of_range(err);
      }
      // Add add token to the terminal_token
      t = token_table[token_name];
      is_terminal_token[t] = true;
      // Print Warning if the specified accepting token is differnet from
      // token_types.h defines.
      if((static_cast<int>(t) >= 1000)){
	std::cerr<<"WARNING: token "<<token_name<<" specified by ";
	std::cerr<<input_file<<" is not a terminal token defined in";
	std::cerr<<"token_types.h"<<std::endl;
      }
    }
    
    // Read in the non terminal tokens
    ifs>>num;
    line++;

    for(int i = 0; i<num; i++,line++){
      token_name.clear();
      ifs>>token_name;
      // Check if the token name maps to an actual token type.
      if(token_table.find(token_name) == token_table.end()){
	std::string err = token_name;
	err += " does not exist.";
	ifs.close();
	throw std::out_of_range(err);
      }
      // Add add token to the terminal_token
      t = token_table[token_name];
      is_non_terminal_token[t] = true;
      // Print Warning if the specified accepting token is differnet from
      // token_types.h defines.
      if((static_cast<int>(t) < 1000)){
	std::cerr<<"WARNING: token "<<token_name<<" specified by ";
	std::cerr<<input_file<<" is not a non-terminal token defined in";
	std::cerr<<"token_types.h"<<std::endl;
      }
    }

    // Read In start state
    token_name.clear();
    ifs>>token_name;
    line++;

    if(token_table.find(token_name) == token_table.end()){
      std::string err = token_name;
      err += " does not exist.";
      ifs.close();
      throw std::out_of_range(err);
    }
    m_start_token = token_table[token_name];
    token_name.clear();
    
    // Read in reduce rules
    ifs>>num;
    line++;

    // Loop for 1 more time to skip newline
    for(int i = 0; i<=num; i++,line++){
      // Read in a whole line
      char L[512];
      ifs.getline(L,512);
      if(!i) continue;
      std::istringstream iss(L);
      // Get each token and make the rule in reverse order
      std::vector<TokenType> rule;
      token_name.clear();
      while(iss>>token_name){
	// Check if the token name maps to an actual token type.
	if(token_table.find(token_name) == token_table.end()){
	  std::string err = token_name;
	  err += " does not exist.";
	  ifs.close();
	  throw std::out_of_range(err);
	}
	rule.emplace(rule.begin(),token_table[token_name]);
	token_name.clear();
      }
      m_reduce_rules.emplace_back(rule);
    }
						       
    // Read in parse table.
    ifs>>m_states_count;
    line++;
    ifs>>num;
    line++;

    //add maps to parse_tabel
    for(int i = 0; i<m_states_count;i++){
      std::map<TokenType,std::pair<char,int>> entry;
      m_parse_table.emplace_back(entry);
    }
    
    // Readin entries
    token_name.clear();
    // Loop for 1 more time to get rid of newline
    int entries = num;
    for(int i = 0; i<=entries; i++,line++){
      // Read in a whole line
      char L[512];
      char action[10];
      int next_state = 0;
      ifs.getline(L,512);
      if(!i) continue;
      std::istringstream iss(L);

      // get state number
      iss >> num;
      // get token name
      iss >> token_name;
      // get action
      iss >> action;
      // get next state
      iss >> next_state;

      // Check if token_name valid
      if(token_table.find(token_name) == token_table.end()){
	std::string err = token_name;
	err += " does not exist.";
	ifs.close();
	std::cerr<<"LINE "<<line<<":"<<err<<std::endl;
	throw std::out_of_range(err);
      }

      t = static_cast<TokenType>(token_table[token_name]);

      // Check if state exists
      if(num >= m_states_count){
	ifs.close();
	std::cerr<<"LINE: "<<line<<":State number "<<num<<" out of range."<<std::endl;
	std::cerr<<m_states_count<<std::endl;
	throw std::out_of_range("State number out of range.");
      }

      // Check if entry exists
      if(m_parse_table[num].find(t) != m_parse_table[num].end()){
	ifs.close();
	std::cerr<<"LINE: "<<line<<":Parse table ambiguity."<<std::endl;
	throw std::invalid_argument("Parse table ambiguity.");
      }

      std::pair <char,int> transition(action[0],next_state);
      m_parse_table[num][t] = transition;
      token_name.clear();
    }
    std::cerr<<"FUCK!"<<std::endl;
    
  } catch (const std::exception& e){
    ifs.close();
    // TODO: Add line count to exception message
    throw e;
  }
  
}
