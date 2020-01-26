#pragma_once

enum DFAStates {
	START = 0,
	// Accepting States
	SLASH = 1,
	DOUBLE_SLASH = 2,
	SLASH_STAR = 3,
	// SLASH_STAR_STAR = 4,  this is stte is not needed
	STAR_SLASH = 5,
	EQUAL = 6,
	PLUS = 7,
	MINUS = 8,
	STAR = 9,
	AND = 10,
	OR = 11,
	EXP = 12,
	PERCENT = 13,
	LESS = 14,
	GREATER = 15,
	NOT = 16,
	TILDE = 17,
	LEFT_ROUND_BRACKET = 18,
	RIGHT_ROUND_BRACKET = 19,
	LEFT_SQUARE_BRACKET = 20,
	RIGHT_SQUARE_BRACKET = 21,
	LEFT_CURLY_BRACKET = 22,
	RIGHT_CURLY_BRACKET = 23,
	DOT = 24,
	COMMA = 25,
	QUESTION = 26,
	COLON = 27,
	SEMICOLON = 28,
	INTEGER = 29,
	// SINGLE_QUOTE = 30, the first ' is not an accepting state
	// DOUBLE_QUOTE = 31, the first " is not an accepting state
	PLUS_EQUAL = 32,
	MINUS_EQUAL = 33,
	STAR_EQUAL = 34,
	SLASH_EQUAL = 35,
	AND_EQUAL = 36,
	OR_EQUAL = 37,
	EXP_EQUAL = 38,
	PERCENT_EQUAL = 39,
	LESS_LESS = 40,
	GREATER_GREATER = 41,
	OR_OR = 42,
	AND_AND = 43,
	EQUAL_EQUAL = 44,
	NOT_EQUAL = 45,
	LESS_EQUAL = 46,
	GREATER_EQUAL = 47,
	PLUS_PLUS = 48,
	MINUS_MINUS = 49,
	LESS_LESS_EQUAL = 50,
	GREATER_GREATER_EQUAL = 51,
	GREATER_GREATER_GREATER = 52,
	GREATER_GREATER_GREATER_EQUAL = 53,
	FLOAT = 54,
	CHARACTER = 55,
	STRING = 56,
	POSSIBLY_IDENTIFIER = 57,
	IDENTIFIER = 58,

	// State where everything other than 

	// The Non-Accepting States
	SINGLE_QUOTE = 101,
	OPEN_CHAR_LITERAL = 102,
	SINGLE_QUOTE_SLASH = 103,
	DOUBLE_QUOTE = 104,
	DOUBLE_QUOTE_SLASH = 105,
};
