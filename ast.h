#pragma once

#include "lexer.h"
#include "lime_types.h"

#include <vector>
#include <tuple>
#include <cassert>
#include <map>
#include <algorithm>

enum NodeType {
    LIME_NODE_NONE,
    LIME_NODE_VARIABLE_DECLARATION,
    LIME_NODE_VARIABlE_ASSIGNMENT,
    LIME_NODE_EXPRESSION,
    LIME_NODE_NUMBER_LITERAL,
    LIME_NODE_STRING_LITERAL,
    LIME_NODE_CHARACTER_LITERAL,
    LIME_NODE_OPERATOR,
    LIME_NODE_CODE_BLOCK,
    LIME_NODE_IDENTIFIER,
    LIME_NODE_PROC_CALL,
    LIME_NODE_PROC_DEFINITION,
    LIME_NODE_PROC_DECLARATION,
    LIME_NODE_ARGUMENT_LIST,
    LIME_NODE_PARAMETER_LIST,
    LIME_NODE_WHILE_LOOP,
    LIME_NODE_RANDOM_IDENTIFIER, // Identifier that doesnt have any purpose

    LIME_NODE_IF_STATEMENT,
    LIME_NODE_ELSEIF_STATEMENT,
    LIME_NODE_ELSE_STATEMENT,

    LIME_NODE_RETURN,
    LIME_NODE_EMIT,
};

static const std::map<NodeType, const std::string> LimeNodeTypesNames = {
    {LIME_NODE_NONE,"NONE"},
    {LIME_NODE_VARIABlE_ASSIGNMENT,"VARIABlE_ASSIGNMENT"},
    {LIME_NODE_VARIABLE_DECLARATION, "VARIABLE_DECLARATION"},
    {LIME_NODE_EXPRESSION,"EXPRESSION"},
    {LIME_NODE_NUMBER_LITERAL,"NUMBER_LITERAL"},
    {LIME_NODE_STRING_LITERAL,"LIME_NODE_STRING_LITERAL"},
    {LIME_NODE_CHARACTER_LITERAL,"LIME_NODE_CHARACTER_LITERAL"},
    {LIME_NODE_OPERATOR,"OPERATOR"},
    {LIME_NODE_CODE_BLOCK,"CODE_BLOCK"},
    {LIME_NODE_IDENTIFIER,"IDENTIFIER"},
    {LIME_NODE_PROC_CALL,"PROC_CALL"},
    {LIME_NODE_PROC_DECLARATION,"PROC_DECLARATION"},
    {LIME_NODE_PROC_DEFINITION, "PROC_DEFINITION"},
    {LIME_NODE_ARGUMENT_LIST,"ARGUMENT_LIST"},
    {LIME_NODE_PARAMETER_LIST, "PARAMETER_LIST"},
    {LIME_NODE_WHILE_LOOP, "WHILE_LOOP"},
    {LIME_NODE_IF_STATEMENT, "IF_STATEMENT"},
    {LIME_NODE_ELSEIF_STATEMENT,"ELSEIF_STATEMENT"},
    {LIME_NODE_ELSE_STATEMENT, "ELSE_STATEMENT"},
    {LIME_NODE_RETURN,"RETURN"},
    {LIME_NODE_EMIT, "EMIT"},
    {LIME_NODE_RANDOM_IDENTIFIER, "LIME_NODE_RANDOM_IDENTIFIER"}, // Identifier that doesnt have any purpose
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
    {"==",     Equals },
    {">",     GreaterThan },
    {">=",    GreaterThanOrEqual },
    {"<",    LessThan },
    {"<=",    LessThanOrEqual },
    {"!=",    NotEqual },
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
            bool external{false};
        };
    };

    Node* add(Node* node);

	friend ostream& operator<<(ostream& os, const Node& node);

    std::string ToString(std::string indent) const;

    friend Node create_ast_from_tokens(const std::vector<Token>& tokens);
    friend void code_block_to_ast(Node* ast, std::vector<Token>& tokens);
};

ostream& operator<<(ostream& os, const Node& node);

typedef Node Ast;

struct CodeLens {
    std::vector<std::map<std::string, Node*>> variable_scope;
    std::map<std::string, Node*> functions;

    bool varExists(const std::string& name);
    void addVar(Node* node);

    bool procExists(const std::string& name);
    void addProc(Node* node);

    void push();
    void pop();
};

static CodeLens* Lens;
CodeLens* GetCodeLens();

void code_block_to_ast(Ast* ast, std::vector<Token>& tokens);
Node* create_ast_from_tokens(std::vector<Token>& tokens);
