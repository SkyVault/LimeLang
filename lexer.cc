#include "lexer.h"
#include "lime_utils.h"
#include <cctype>

ostream& operator<<(ostream& os, const Token& token) {
	auto str = token.ToString();
    os<<str;
	return os;
}

std::string Token::ToString() const {
	auto t = type;
    std::string result{""};
    result += "Token {";
	result += "  .word = " + (word == "\n" ? "\\n" : word);
    result += "  .type = " + LimeTokenTypesNames.find(t)->second;
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

	const std::string operators = "=+-*/<>^";

	const auto GetOperator = [](std::string word){
		if (word == "=") return LIME_ASSIGNMENT_OPERATOR;
		if (word == "+") return LIME_PLUS_OPERATOR;
		if (word == "-") return LIME_MINUS_OPERATOR;
		if (word == "*") return LIME_MULTIPLICATION_OPERATOR;
		if (word == "/") return LIME_DIVISION_OPERATOR;
		cout << "Unhandled operator: " << word << endl;
		return LIME_NONE_OPERATOR;
	};

	for (const auto& word : words){
		if (word == "\n") result.push_back(Token{ .word = word, .type = LIME_NEWLINE, .isWhiteSpace = true });
		else if (word == "\t") result.push_back(Token{ .word = word, .type = LIME_TABULAR, .isWhiteSpace = true });
		else if (word == " ") result.push_back(Token{ .word = word, .type = LIME_WHITESPACE, .isWhiteSpace = true });
		else if (word == "(") result.push_back(Token{ .word = word, .type = LIME_OPEN_PAREN });
		else if (word == ")") result.push_back(Token{ .word = word, .type = LIME_CLOSE_PAREN });
		else if (word == "proc") result.push_back(Token{ .word = word, .type = LIME_PROC });
		else if (word == "mut") result.push_back(Token{ .word = word, .type = LIME_MUTABLE });
		else if (operators.find(word) != std::string::npos)
			result.push_back(Token{
					.word=word, 
					.type=LIME_OPERATOR,
					.op = GetOperator(word),
			});
        else if (isNumber(word)) {
            result.push_back(Token{ 
                    .word = word, 
                    .type = LIME_NUMBER
            });
        }
		else 
			result.push_back(Token{
				.word = word,
				.type = LIME_IDENTIFIER
			});
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
