#include<map>
#include<string>
#include "token_types.h"
#include<iostream>
#include<fstream>

using namespace std;

map<string, TokenType> readSpecialWords() {
    map<string, TokenType> specialWordsMap;
    ifstream specialWordsFile;
    specialWordsFile.open("special_words_to_kinds");
    string word;
    int kind;
    while(!specialWordsFile.eof()) {
        specialWordsFile>>word;
        specialWordsFile>>kind;
        specialWordsMap[word] = static_cast<TokenType>(kind);
    }

    return specialWordsMap;
}

int main(int argc, char* argv[]) {
    map<string, TokenType> specialWordsMap = readSpecialWords();
    for (pair<string, TokenType> element: specialWordsMap) {
        cout<<element.first<<" "<<element.second<<endl;
    }
    string possiblyIdentifier = argv[1];
    if (specialWordsMap.find(possiblyIdentifier) != specialWordsMap.end()) {
        //cout<<possiblyIdentifier<<" "<<specialWordsMap[possiblyIdentifier]<<endl;
        return specialWordsMap[possiblyIdentifier]; 
    } else {
        //cout<<possiblyIdentifier<<" "<<TokenType::T_IDENTIFIER<<endl;
        return TokenType::T_IDENTIFIER;
    }
}
