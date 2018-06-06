#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cassert>
#include <map>

#include "ast.h"
#include "lexer.h"

#include "lime_tests.h"

const auto code = std::string{R"LEAF(

myVar mut int = 100 / (4 * 2 + 3)
other int = myVar + 2

)LEAF"};

#define RUN_TESTS

double run_expression(Node* ast) {
    double result{0.0};

    assert(ast->type == LIME_NODE_EXPRESSION); 

    auto op = ast->children[0];

    assert(op->type == LIME_NODE_OPERATOR); 

    auto a = op->children[0];
    auto b = op->children[1];
    
    auto a_num{0.0}, b_num{0.0};

    if (a->type == LIME_NODE_NUMBER_LITERAL)
        a_num = std::stof(a->token.word);
    
    if (b->type == LIME_NODE_NUMBER_LITERAL)
        b_num = std::stof(b->token.word);

    if (a->type == LIME_NODE_EXPRESSION)
        a_num = run_expression(a);
    
    if (b->type == LIME_NODE_EXPRESSION)
        b_num = run_expression(b);
    
    if (op->token.op == LIME_PLUS_OPERATOR)
        result = a_num + b_num;

    if (op->token.op == LIME_MINUS_OPERATOR)
        result = a_num - b_num;

    if (op->token.op == LIME_MULTIPLICATION_OPERATOR)
        result = a_num * b_num;

    if (op->token.op == LIME_DIVISION_OPERATOR)
        result = a_num / b_num;

    return result;
}

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

    for (const auto n : ast.children) {
        if (n->type == LIME_NODE_VARIABlE_ASSIGNMENT) {
            std::cout << n->identifier->word << " = ";
            std::cout << run_expression(n->children[0]) << "\n";
        }
    }
	
	out.close();
}
