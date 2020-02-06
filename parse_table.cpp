#include "parse_table.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <exception>
#include <stdexcept>

#include "color_print.h"

ParseTable::ParseTable():
  m_states_count(-1)
{
  try{
    readin("data/parser/parse_table.lr1");
  } catch (const std::exception& e){
    RED();
    std::cerr<<"ERROR: Parse Table Readin Failed. "<<e.what()<<std::endl;
    DEFAULT();
  }
}

ParseTable::ParseTable(std::string& input_file):
  m_states_count(-1)
{
  try{
    readin(input_file);
  } catch (const std::exception& e){
    RED();
    std::cerr<<"ERROR: Parse Table Readin Failed."<<e.what()<<std::endl;
    DEFAULT();
  }
}

void ParseTable::readin(std::string input_file){
  std::ifstream token_ifs("data/parser/token_str_to_num.txt");
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
	YELLOW();
	std::cerr<<"WARNING: token "<<token_name<<" specified by ";
	std::cerr<<input_file<<" is not a terminal token defined in";
	std::cerr<<"token_types.h"<<std::endl;
	DEFAULT();
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
	YELLOW();
	std::cerr<<"WARNING: token "<<token_name<<" specified by ";
	std::cerr<<input_file<<" is not a non-terminal token defined in";
	std::cerr<<"token_types.h"<<std::endl;
	DEFAULT();
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
      if(!i) { line-- ; continue; }
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
    
    PURPLE();
    std::cout<<m_states_count<<","<<line<<","<<input_file<<std::endl;
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
      if(!i) { line--; continue; }
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
	RED();
	std::cerr<<"LINE "<<line<<":"<<err<<std::endl;
	DEFAULT();
	throw std::out_of_range(err);
      }

      t = static_cast<TokenType>(token_table[token_name]);

      // Check if state exists
      if(num >= m_states_count){
	ifs.close();
	RED();
	std::cerr<<"LINE: "<<line<<":State number "<<num<<" out of range."<<std::endl;
	std::cerr<<m_states_count<<std::endl;
	DEFAULT();
	throw std::out_of_range("State number out of range.");
      }

      // Check if entry exists
      if(m_parse_table[num].find(t) != m_parse_table[num].end()){
	ifs.close();
	RED();
	std::cerr<<"LINE: "<<line<<":Parse table ambiguity."<<std::endl;
	DEFAULT();
	throw std::invalid_argument("Parse table ambiguity.");
      }

      std::pair <char,int> transition(action[0],next_state);
      m_parse_table[num][t] = transition;
      token_name.clear();
    }
    
  } catch (const std::exception& e){
    ifs.close();
    // TODO: Add line count to exception message
    throw e;
  }
  
}

bool ParseTable::parse(std::vector<Token>& token_vec){
  // Add eof to the end of token_vec, bof to the stack.
  const Token Token_EOF = Token(TokenType::T_EOF,"");
  const Token Token_BOF = Token(TokenType::T_BOF,"");
  token_vec.emplace(token_vec.begin(),Token_BOF);
  token_vec.emplace_back(Token_EOF);
  LR1StackLayer initialState = LR1StackLayer(0);
  m_stack.push(initialState);

  
  // loop to read in
  unsigned int tv_len = token_vec.size();
  Token token_of_interest;
  int current_state = 0;
  
  for(unsigned int i = 0; i<tv_len;){
    // If current token_of_interest is TOKEN_EMPTY
    // Get a new token from token vector
    if(token_of_interest.m_type == TokenType::TOKEN_EMPTY){
      token_of_interest = token_vec[i];
    }

    // Determine the action

    // Check state
    if(current_state >= m_states_count){
      RED();
      std::cerr<<"PARSER ERROR: State "  <<current_state<< " does not exist. Token";
      std::cerr<<" of interest is "<<token_of_interest<<", index i = "<<i;
      std::cerr<<std::endl;
      DEFAULT();
      return false;
    }
    
    // Check if transition exists
    std::map<TokenType, std::pair<char,int>>& entry = m_parse_table[current_state];
    if(entry.find(token_of_interest.m_type) == entry.end()){
      RED();
      std::cerr<<"PARSER ERROR: State "<< current_state <<" has no transition given";
      std::cerr<<" token "<<token_of_interest<<". Index i = "<< i;
      std::cerr<<",stack looks like:"<<std::endl;
      while(!m_stack.empty()){
	std::cerr<<m_stack.top()<<std::endl;
	m_stack.pop();
      }
      DEFAULT();
      return false;
    }

    // get action and state or rule number
    char action = entry[token_of_interest.m_type].first;
    int state_rule = entry[token_of_interest.m_type].second;

    if (action == 's') {
      // If shift, place that on the stack
      // change current state, create new layer with
      // token_of_interest and the new state, increase index
      LR1StackLayer new_layer(state_rule, token_of_interest);
      m_stack.push(new_layer);
      token_of_interest.clear();
      current_state = state_rule;
      i++;
    } else if (action == 'r') {
      // If reduce, reduce to a new token and try to read in again
      // Check if rule number is correct
      if(state_rule >= m_reduce_rules.size()){
	RED();
	std::cerr<<"PARSER ERROR: Reduction rule"<<state_rule<<" does not";
	std::cerr<<" exist. Token of interest is "<<token_of_interest<<" index";
	std::cerr<<" i = "<<i<<std::endl;
	DEFAULT();
	return false;						       
      }
      
      std::vector<TokenType> reduce_rule = m_reduce_rules[state_rule];

      // Prepare a vector for recording the tokens
      std::vector<Token> reduced_tokens;
      int counter = reduce_rule.size() - 1;
      for(TokenType t:reduce_rule){
	// Check if it's the last token, i.e. the target token to reduce to
	if(!counter){
	  token_of_interest.clear();
	  token_of_interest = Token(t,state_rule,reduced_tokens);
	  current_state = m_stack.top().m_state;
	  break;
	}
	// Pop layers from stack and check if it's the same type
	LR1StackLayer layer = m_stack.top();
	m_stack.pop();
	if(layer.m_token.m_type != t){
	  RED();
	  std::cerr<<"PARSER ERROR: Reduce "<< state_rule<<" failed.";
	  std::cerr<<" State = "<< current_state << ", failed token = ";
	  std::cerr<<layer.m_token<<", expected "<<Token(t,"");
	  std::cerr<<std::endl<<"Reduce "<<state_rule<<" is: ";
	  // Print the reduce rule
	  std::cerr<<Token(reduce_rule[reduce_rule.size()-1],"")<<" -> ";
	  for(int index = reduce_rule.size()-2; index >= 0; index --){
	    std::cerr<<Token(reduce_rule[i],"")<<" ";
	  }
	  std::cerr<<std::endl;
	  std::cerr<<"Currently at: "<<counter<<", read in from right to left.";
	  std::cerr<<std::endl;
	  DEFAULT();
	  return false;
	} else {
	  // put the poped token into reduced_tokens vector
	  reduced_tokens.emplace(reduced_tokens.begin(),layer.m_token);
	}
	counter --;
      }
    } else{
      // Otherwise throw an exception indicate a crash.
      PURPLE();
      std::cerr<<"PARSER CRASHED: unknown action "<<action<<std::endl;
      DEFAULT();
      throw std::exception();
    }
  }
  return true;
}
