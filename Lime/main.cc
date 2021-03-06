#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cassert>
#include <map>

#include "ast.h"
#include "lexer.h"
#include "lime_c_gen.h"
#include "lime.h"

#include "lime_tests.h"

//#define RUN_TESTS
#define LOG_TO_FILES

int main(const int num_args, const char** args) {
#ifdef RUN_TESTS
    std::cout << "\n";
    if (!limeRunTests())
        return -1;
    std::cout << "\n";
#endif

	const auto content = LoadFileToString("test.lime");
	auto tokens = TokenizeString(content);

#ifdef LOG_TO_FILES
    auto tfile = std::ofstream("tokens_out.txt");
    for (auto t : tokens) {
        tfile << t << std::endl;
    }
    tfile.close();
#endif

    // Ast
    auto ast = create_ast_from_tokens(tokens);
    // std::cout << *ast << std::endl;

#ifdef LOG_TO_FILES
    auto afile = std::ofstream("ast_out.txt");
    afile << *ast << std::endl;
    afile.close();
#endif

    LimeCGen cgen;
    cgen.compile_ast_to_c(ast, "out.c");

    std::cout << " ==== out ==== " << std::endl;
    system("gcc out.c -o out.exe && out.exe");
	std::cin.get();

}
