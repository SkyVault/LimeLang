#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <functional>

#include "lime_types.h"

using namespace std;

enum LimeTokenTypes {
	LIME_IDENTIFIER,
	LIME_NUMBER,
	LIME_NEWLINE,
	LIME_TABULAR,
	LIME_WHITESPACE,
	LIME_OPERATOR,
    LIME_CAMMA,

    LIME_PROC,

    LIME_OPEN_PAREN,
    LIME_CLOSE_PAREN,

    LIME_OPEN_CURLY_BRACKET,
    LIME_CLOSE_CURLY_BRACKET,

    LIME_MUTABLE,

    LIME_TYPE_IDENTIFIER,

	LIME_NUM_TOKENS
};

enum LimeOperators {
	LIME_ASSIGNMENT_OPERATOR,
	LIME_PLUS_OPERATOR,
	LIME_MINUS_OPERATOR,
	LIME_MULTIPLICATION_OPERATOR,
	LIME_DIVISION_OPERATOR,
	LIME_NONE_OPERATOR
};

static const std::map<LimeTokenTypes, const std::string> LimeTokenTypesNames = {
	{LIME_IDENTIFIER, "LIME_IDENTIFIER"},
	{LIME_NUMBER, "LIME_NUMBER"},
	{LIME_NEWLINE, "LIME_NEWLINE"},
	{LIME_TABULAR, "LIME_TABULAR"},
	{LIME_WHITESPACE, "LIME_WHITESPACE"},
	{LIME_OPERATOR, "LIME_OPERATOR"},

	{LIME_OPERATOR, "LIME_PROC"},

	{LIME_OPEN_PAREN, "LIME_OPEN_PAREN"},
	{LIME_CLOSE_PAREN, "LIME_CLOSE_PAREN"},

	{LIME_MUTABLE, "LIME_MUTABLE"},

	{LIME_TYPE_IDENTIFIER, "LIME_TYPE_IDENTIFIER"},
};

//static_assert(LimeTokenTypesNames.size() == LIME_NUM_TOKENS);

struct Token {
    Token();
    Token(std::string word, LimeTokenTypes type, bool isWhiteSpace);
    Token(const Token& token);

	std::string word{""};
	
	LimeTokenTypes type{LIME_IDENTIFIER};
	LimeOperators op{LIME_ASSIGNMENT_OPERATOR};
    
    bool isWhiteSpace{false};
    int line_number{1};

    std::string ToString() const;

	friend ostream& operator<<(ostream& os, const Token& token);
};

ostream& operator<<(ostream& os, const Token& token);

typedef std::vector<Token> TokenList;

const std::string LoadFileToString(const std::string& path);
vector<string> SplitTokensToWordsAndSymbols(const string& code);
vector<Token> WordsToTokens(const vector<string>& words);
vector<Token> TokenizeString(const std::string& code);
