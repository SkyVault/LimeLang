#pragma once

#include "lexer.h"
#include "lime_types.h"

#include <vector>
#include <tuple>
#include <cassert>
#include <map>

enum NodeType {
    LIME_NODE_NONE,
    LIME_NODE_VARIABlE_ASSIGNMENT,
    LIME_NODE_EXPRESSION,
    LIME_NODE_NUMBER_LITERAL,
    LIME_NODE_OPERATOR,
    LIME_NODE_CODE_BLOCK,
    LIME_NODE_IDENTIFIER,
    LIME_NODE_FUNCTION_CALL,
    LIME_NODE_PROCEDURE_DEFINITION,
    LIME_NODE_PROCEDURE_DECLARATION,
    LIME_NODE_ARGUMENT_LIST,
};

static const std::map<NodeType, const std::string> LimeNodeTypesNames = {
    {LIME_NODE_NONE,"NONE"},
    {LIME_NODE_VARIABlE_ASSIGNMENT,"VARIABlE_ASSIGNMENT"},
    {LIME_NODE_EXPRESSION,"EXPRESSION"},
    {LIME_NODE_NUMBER_LITERAL,"NUMBER_LITERAL"},
    {LIME_NODE_OPERATOR,"OPERATOR"},
    {LIME_NODE_CODE_BLOCK,"CODE_BLOCK"},
    {LIME_NODE_IDENTIFIER,"IDENTIFIER"},
    {LIME_NODE_FUNCTION_CALL,"FUNCTION_CALL"},
    {LIME_NODE_PROCEDURE_DECLARATION,"PROCEDURE_DECLARATION"},
    {LIME_NODE_PROCEDURE_DEFINITION, "PROCEDURE_DEFINITION"},
    {LIME_NODE_ARGUMENT_LIST,"ARGUMENT_LIST"}
};

enum OrderOfPrecedence {
    Comma,
    PlusEqual,
    MinusEqual,
    TimesEqual,
    DivideEqual,
    ModEqual,
    Equals,
    GreaterThan,
    GreaterThanOrEqual,
    LessThan,
    LessThanOrEqual,
    NotEqual,
    Sub,
    Add,
    Mod,
    Div,
    Mult,
    UnaryMinus,
    UnaryPlus
};

static const std::map<std::string, OrderOfPrecedence> OrderOfPrecedenceTable = {
    {",",      Comma },
    {"+=",     PlusEqual },
    {"-=",     MinusEqual },
    {"*=",     TimesEqual },
    {"/=",     DivideEqual },
    {"%=",     ModEqual },
    {"eq",     Equals },
    {"gr",     GreaterThan },
    {"gre",    GreaterThanOrEqual },
    {"les",    LessThan },
    {"lee",    LessThanOrEqual },
    {"neq",    NotEqual },
    {"-",      Sub },
    {"+",      Add },
    {"%",      Mod },
    {"/",      Div },
    {"*",      Mult }
};

struct Node {
    Node(Token& token);
    Node();

    Token token;
//    int line_number {1};
    int precedence  {0};

    NodeType type{LIME_NODE_NONE};

    std::vector<Node*> children;

    union {
        struct { 
            Token* identifier{nullptr}; 
            Token* variable_type{nullptr};
            bool canMutate{false};
        };
    };

	friend ostream& operator<<(ostream& os, const Node& node);

    std::string ToString(std::string indent) const;

    friend Node create_ast_from_tokens(const std::vector<Token>& tokens);
    friend void code_block_to_ast(Node* ast, std::vector<Token>& tokens);

};

ostream& operator<<(ostream& os, const Node& node);

typedef Node Ast;

struct CodeLens {

};

void code_block_to_ast(Ast* ast, std::vector<Token>& tokens);
Ast create_ast_from_tokens(std::vector<Token>& tokens);
