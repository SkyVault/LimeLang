#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include "ast.h"
#include "lexer.h"

#include "lime_tests.h"

const auto code = std::string{R"LEAF(

myVar mut int = 100 / (4 * 2 + 3)
other int = myVar + 2

)LEAF"};

#define RUN_TESTS

int main() {
#ifdef RUN_TESTS
    std::cout << "\n";
    if (!limeRunTests())
        return -1;
    std::cout << "\n";
#endif

	//const auto content = LoadFileToString("test.lime");
	auto tokens = TokenizeString(code);

	auto out = ofstream("tokenizer_output.txt");

	for (const auto& tok : tokens){
		out << tok << endl;
	}

    // Ast
    const auto ast = create_ast_from_tokens(tokens);
	
	out.close();
}
