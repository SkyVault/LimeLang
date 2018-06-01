#include "lexer.h"

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
};

static const std::map<NodeType, const std::string> LimeNodeTypesNames = {
    {LIME_NODE_NONE,"LIME_NODE_NONE"},
    {LIME_NODE_VARIABlE_ASSIGNMENT,"LIME_NODE_VARIABlE_ASSIGNMENT"},
    {LIME_NODE_EXPRESSION,"LIME_NODE_EXPRESSION"},
    {LIME_NODE_NUMBER_LITERAL,"LIME_NODE_NUMBER_LITERAL"},
    {LIME_NODE_OPERATOR,"LIME_NODE_OPERATOR"},
    {LIME_NODE_CODE_BLOCK,"LIME_NODE_CODE_BLOCK"},
};

struct Node {
    Node(Token& token);
    Node();

    Token token;
    int line_number{1};
    NodeType type{LIME_NODE_NONE};

    std::vector<Node*> children;

    union {
        struct { 
            Token* identifier{nullptr}; 
            bool canMutate{false};
        };

    };

	friend ostream& operator<<(ostream& os, const Node& node);

    std::string ToString(std::string indent) const;
};

ostream& operator<<(ostream& os, const Node& node);

struct Ast : public Node {
    Ast();

    friend Ast create_ast_from_tokens(const std::vector<Token>& tokens);
    friend void code_block_to_ast(Ast* ast, std::vector<Token>& tokens);

private:
    int line_number{1};
};

struct CodeLens {

};

void code_block_to_ast(Ast* ast, std::vector<Token>& tokens);
Ast create_ast_from_tokens(std::vector<Token>& tokens);
