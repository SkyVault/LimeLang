#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cassert>
#include <map>

#include "ast.h"
#include "lexer.h"
#include "lime_c_gen.h"

#include "lime_tests.h"

const auto code = std::string{R"LEAF(
#[ Procedure Tests ] 

ProcTest1 proc int
ProcTest2 proc() none
ProcTest3 proc(a) none

ProcTest4 proc { 
    test mut int = 100
}

ProcTest5 proc() none {}

AddEm proc(a int, b int) {
    ret a + b
}

#[ Expression Tests ] 

myVar mut int = 100 / (4 * 2 + 3)
# myVar mut int = / 100 (* 4 (+ 2 3))
other int = myVar + 2

myVar = myVar + 2

)LEAF"};

#define RUN_TESTS
#define LOG_TO_FILES

int main() {
#ifdef RUN_TESTS
    std::cout << "\n";
    if (!limeRunTests())
        return -1;
    std::cout << "\n";
#endif

	//const auto content = LoadFileToString("test.lime");
	auto tokens = TokenizeString(code);

#ifdef LOG_TO_FILES
    auto tfile = std::ofstream("tokens_out.txt");
    for (auto t : tokens) {
        tfile << t << std::endl;
    }
    tfile.close();
#endif

    // Ast
    auto ast = create_ast_from_tokens(tokens);

    LimeCGen cgen;
    cgen.compile_ast_to_c(&ast, "out.c");

#ifdef LOG_TO_FILES
    auto afile = std::ofstream("ast_out.txt");
    afile << ast << std::endl;
    afile.close();
#endif
}
