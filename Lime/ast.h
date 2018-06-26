#pragma once
using namespace std;

#include "lexer.h"
#include "lime_types.h"

#include <vector>
#include <tuple>
#include <cassert>
#include <map>
#include <fstream>
#include <iostream>
#include <algorithm>

enum NodeType {
    LIME_NODE_NONE,
    LIME_NODE_VARIABLE_DECLARATION,
    LIME_NODE_VARIABLE_ASSIGNMENT,
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

    LIME_NODE_ARRAY_INSERTION,

    LIME_NODE_IF_STATEMENT,
    LIME_NODE_ELSEIF_STATEMENT,
    LIME_NODE_ELSE_STATEMENT,

    LIME_NODE_RETURN,
    LIME_NODE_EMIT,
    LIME_NODE_C_INCLUDE,
};

static const std::map<NodeType, const std::string> LimeNodeTypesNames = {
    {LIME_NODE_NONE,"NONE"},
    {LIME_NODE_VARIABLE_ASSIGNMENT,"VARIABLE_ASSIGNMENT"},
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
    {LIME_NODE_C_INCLUDE,"C_INCLUDE"},
    {LIME_NODE_RANDOM_IDENTIFIER, "RANDOM_IDENTIFIER"}, // Identifier that doesnt have any purpose
    {LIME_NODE_ARRAY_INSERTION, "ARRAY_INSERTION"},
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

// struct FunctionPrototype {
    
// };

struct Node {
    Node(Token& token);
    Node();
    ~Node();

    Token token;

    NodeType type{LIME_NODE_NONE};

    std::vector<Node*> children;
    int precedence  {0};

    Node* add(Node* node);

	friend ostream& operator<<(ostream& os, const Node& node);
    friend Node create_ast_from_tokens(const std::vector<Token>& tokens);
    friend void code_block_to_ast(Node* ast, std::vector<Token>& tokens);
};

struct IdentifierNode: public Node {
    IdentifierNode(Token& token);
    IdentifierNode();
    ~IdentifierNode();

    Token* identifier; 
    Token* variable_type;
    bool canMutate;
    bool isArray{false};
    bool external;
};

struct ProcPrototypeNode;

struct ProcCallNode: public IdentifierNode {
    std::vector<std::tuple<std::string, LimeTypes>> parameters;
    ProcPrototypeNode* proc{nullptr};
};

struct ProcPrototypeNode : public IdentifierNode {
    std::string name{""};
    LimeTypes return_type{LimeTypes::None};
    std::vector<std::tuple<std::string, LimeTypes>> parameters;

    bool matches(ProcCallNode* proccall);
};

std::string ToString(Node* node, std::string indent);

ostream& operator<<(ostream& os, Node& node);

typedef Node Ast;

struct CodeLens {
    std::vector<std::map<std::string, IdentifierNode*>> variable_scope;
    std::map<std::string, ProcPrototypeNode*> functions;

    bool varExists(const std::string& name);
    void addVar(IdentifierNode* node);
    IdentifierNode* getVar(const std::string& name);

    bool procExists(const std::string& name);
    void addProc(ProcPrototypeNode* node);
    ProcPrototypeNode* getProc(const std::string& name);

    void push();
    void pop();
};

static CodeLens* Lens;
CodeLens* GetCodeLens();

void code_block_to_ast(Ast* ast, std::vector<Token>& tokens);
Node* create_ast_from_tokens(std::vector<Token>& tokens);
