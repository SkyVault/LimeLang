#include "lexer.h"
#include "lime_utils.h"
#include <cctype>
#include <cassert>

Token::Token()
{
    
}

Token::Token(std::string word, LimeTokenTypes type, bool isWhiteSpace = false):
    word(word), 
    type(type), 
    isWhiteSpace(isWhiteSpace)
{
    
}

Token::Token(const Token& token) {
    type = token.type;
    word = token.word;
    op = token.op;
    isWhiteSpace = token.isWhiteSpace;
    line_number = token.line_number;
}

ostream& operator<<(ostream& os, const Token& token) {
	auto str = token.ToString();
    os<<str;
	return os;
}

std::string Token::ToString() const {
	auto t = type;
    std::string result{""};
    result += "Token {";
    result += "  word: " + (word == "\n" ? "\\n" : word);
    result += "  type: " + LimeTokenTypesNames.find(t)->second;
	result += "}";
	return result;
}

const std::string LoadFileToString(const std::string& path){
	std::string line{""};
	std::string result{""};

	auto file = std::ifstream(path);

	while(std::getline(file, line))
		result += line + "\n";

	file.close();

	return result;
}

vector<string> SplitTokensToWordsAndSymbols(const string& code){
	vector<string> result;

	auto it = code.begin();
	auto word = std::string{""};
	std::string delimiters = " \n\t\r!@#$%^&*()`~-+=<>,./?\"\';:[]{}\\|";

	bool in_string{false};
    bool in_number_literal{false};

	while (it != code.end()){

		if (*it == '\"'){
			in_string = !in_string;
			if (!in_string){
				result.push_back(word + std::string{*it});
				word.clear();
				++it;
				continue;
			}
		}

        // Handle number literals
        if (*it == '.' && std::isdigit(*(it + 1))) {
            word += std::string{*it}; ++it;   

            while(std::isdigit(*it))
                word += std::string{(*it++)};
             
            result.push_back(word);
            word.clear();
        }

        if (*it == '-') {
            if (*(it + 1) == '.') {
                if ((it + 2) == code.end())     goto done;
                if (!std::isdigit(*(it + 2)))   goto done;
            }

            while ((*it == '.') || (*it == '-') || std::isdigit(*it))
                word += std::string{(*it++)};

            result.push_back(word);
            word.clear();
        }
        done:;

        // Handle comments
        if (*it == '#') {
            if (*(it + 1) == '(') {
                int depth{0};
                it += 2;
                while(true) {
                    if ((*it) == '#' ) {
                        if (*(it + 1) == ')'){
                            if (depth == 0) break;
                            else depth--;
                        } else if (*(it + 1) == '(')
                            depth++;
                    }
                    ++it;
                }
            }
            else {
                while((*it) != '\n')
                    it++;
            }
        }

		if (in_string){
			word += std::string{*it};
			++it;
			continue;
		}

        // Handle multi character operators
        switch(*it) {
            case '+': 
            case '-':
            case '*':
            case '/':
            case '=':
            case '!':
            case '>':
            case '<': {

                if (*(it + 1) == '=') {
                    if (word.size() > 0){
                        result.push_back(word);
                        word.clear();
                    }

                    auto r = *it + std::string{*(it + 1)};
                    result.push_back(r);
                    it += 2;
                }

                break;
            }
            default: 
                break;
        }

		if (delimiters.find(*it) != std::string::npos) {
			if (word.length() > 0)
				result.push_back(word);
			word.clear();
			result.push_back(std::string{*it});
		} else {
			word += std::string{*it};
		}
		++it;
	}

	if (word.length() > 0)
		result.push_back(word);

	return result;
}

vector<Token> WordsToTokens(const vector<string>& words) {
	vector<Token> result;

    int line_number = 1;

	const std::string operators = "=+-*/<>^";

	const auto GetOperator = [](std::string word){
		if (word == "=") return LIME_ASSIGNMENT_OPERATOR;
		if (word == "+") return LIME_PLUS_OPERATOR;
		if (word == "-") return LIME_MINUS_OPERATOR;
		if (word == "*") return LIME_MULTIPLICATION_OPERATOR;
		if (word == "/") return LIME_DIVISION_OPERATOR;

		if (word == "==") return LIME_COMPAIRISON_OPERATOR;
		if (word == "!=") return LIME_NOT_EQUAL_OPERATOR;
		if (word == ">=") return LIME_GREATER_EQUAL_OPERATOR;
        if (word == "<=") return LIME_LESS_EQUAL_OPERATOR;
        
		if (word == ">") return LIME_GREATER;
        if (word == "<") return LIME_LESS;

		cout << "Unhandled operator: " << word << endl;
		return LIME_NONE_OPERATOR;
	};

	const auto IsOperator = [](std::string word){
		if (word == "=") return true;
		if (word == "+") return true;
		if (word == "-") return true;
		if (word == "*") return true;
		if (word == "/") return true;

		if (word == "==") return true;
		if (word == "!=") return true;
		if (word == ">=") return true;
        if (word == "<=") return true;
        
		if (word == ">") return true;
        if (word == "<") return true;
		return false;
	};

    auto NToken = [&](std::string word, LimeTokenTypes type, bool isWhiteSpace = false) {
        auto t = Token(word, type, isWhiteSpace);
        t.line_number = line_number;
        result.push_back(t);
    };

	for (const auto& word : words){
		if (word == "\n") {
            NToken( word, LIME_NEWLINE, true);
            line_number++;
        }
		else if (word == "\t") NToken( word, LIME_TABULAR, true);
		else if (word == " ") NToken( word, LIME_WHITESPACE, true);
		else if (word == "(") NToken( word, LIME_OPEN_PAREN );
		else if (word == ")") NToken( word, LIME_CLOSE_PAREN );
		else if (word == "{") NToken( word, LIME_OPEN_CURLY_BRACKET);
		else if (word == "}") NToken( word, LIME_CLOSE_CURLY_BRACKET);
		else if (word == "proc") NToken( word, LIME_PROC );
		else if (word == "mut") NToken( word, LIME_MUTABLE );
        else if (word == ",") NToken(word, LIME_COMMA);
        else if (word == "while") NToken(word, LIME_WHILE);
        else if (word == "for") NToken(word, LIME_FOR);
        else if (word == "if") NToken(word, LIME_IF);
        else if (word == "ret") NToken(word, LIME_RETURN);
        else if (word == "$") NToken(word, LIME_METATAG);
        else if (word == "else") NToken(word, LIME_ELSE);
        else if (word == "elif") NToken(word, LIME_ELIF);
		else if (IsOperator(word)){
            auto token = Token(word, LIME_OPERATOR);
            token.op = GetOperator(word);
            token.line_number = line_number;
			result.push_back(token);
        }
        else if (isNumber(word)) {
            NToken(word, LIME_NUMBER);
        }
        else if (isType(word)) {
            NToken(word, LIME_TYPE_IDENTIFIER);
        }
		else 
			NToken(word, LIME_IDENTIFIER);
	}
	
	return result;
}

vector<Token> TokenizeString(const std::string& code) {
    auto words = SplitTokensToWordsAndSymbols(code);

    //for (const auto& word : words){
    //    std::cout << ">>>>> " << word << std::endl;
    //}

	return WordsToTokens(words);
}
