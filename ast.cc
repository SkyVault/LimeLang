#include "ast.h"

Node::Node(Token& token) {
    this->token = token;

    if (this->token.type == LIME_NUMBER)
        this->type = LIME_NODE_NUMBER_LITERAL;

    if (this->token.type == LIME_OPERATOR) 
        this->type = LIME_NODE_OPERATOR;
}

Node::Node(){}

std::string Node::ToString(std::string indent = "") const {
    std::string result{""};

    result += indent + "(" + LimeNodeTypesNames.find(type)->second;
    result += " $line:" + std::to_string(line_number);
    if (token.word.size() > 0)
        result += " $word: [" + token.word + "]";

    if (children.size() > 0) {
        result += " (\n";
        size_t i = 0;
        for(auto c : children) {
            result += c->ToString(indent + "  ");
            if (i != children.size() - 1)
                result += "\n";
            i += 1;
        }
        result += ")";
    } 

    return result + ")";
}

ostream& operator<<(ostream& os, const Node& node) {
    std::string str = node.ToString();
    os << str;
    return os;
}

Ast::Ast() {
    this->type = LIME_NODE_CODE_BLOCK;
}

std::vector<Token> GetExpressionTokens(std::vector<Token>::iterator& it, std::vector<Token>::iterator end) {
    auto Peek = [&]() -> auto {
        auto begin = it;
        begin++;
        while((*begin).isWhiteSpace)
            ++begin;
        return *begin;
    };

    auto begin = it;
    auto t = (*it).type;

    bool running{true};
    while (running) {
        if (it == end) break;
        switch((*it).type) {
            case LIME_WHITESPACE:
            case LIME_NUMBER:
            case LIME_OPEN_PAREN:
            case LIME_CLOSE_PAREN:
            case LIME_OPERATOR:
                ++it;
                break;
            
            case LIME_IDENTIFIER:

                // make sure the identifier isnt for a structure or procedure
                if ((it + 1) != end) {
                    ++it; auto next = Peek(); --it;
                    
                    //TODO: Handle structures and other things that come after an identifier
                    switch(next.type) {
                        case LIME_PROC:                 running = false; break;
                        case LIME_ASSIGNMENT_OPERATOR:  running = false; break;

                        default:
                           break; 
                    }
                }
                
                ++it;
                break;
            default:
                running = false;
                break;
        }
    }
    
    auto result = std::vector(begin, it);
    --it;
    return result;
};

Node* HandleFunctionCall(std::vector<Token>::iterator begin) {
    return nullptr;
}

Node* TokenToNode(Token& token) {
    Node* result = new Node(token);

    switch (token.type) {
        case LIME_NUMBER:
            result->type = LIME_NODE_NUMBER_LITERAL;
            break;
        case LIME_IDENTIFIER:
            result->type = LIME_NODE_IDENTIFIER;
            break;
        case LIME_OPERATOR:
            result->type = LIME_NODE_OPERATOR;
            result->precedence = OrderOfPrecedenceTable.find(token.word)->second;
            break;
        default:
            assert(0);
            break;
    }

    return result;
}

void SortExpression(Node* node){
    // Strip whitespace
    auto it = node->children.begin();
    while(it != node->children.end())
    {
        if ((*it)->token.isWhiteSpace)
            node->children.erase(it--);
        it++;
    }

    auto getHighestPrecedence = [](Node* expression){
        auto high{0}, result{0};

        auto i{0};
        for (const auto& node : expression->children) {
            if (node->precedence > high && node->children.size() == 0) {
                result = i;
                high = node->precedence;
            }
            i++;
        }

        return result;
    };

    if (node->type == LIME_NODE_EXPRESSION)
        if (node->children.size() == 1)
            if (node->children[0]->type == LIME_NODE_EXPRESSION){
                SortExpression(node->children[0]);
                return;
            }

    while (node->children.size() > 1) {
        const auto index = getHighestPrecedence(node); 
        auto op = node->children[index];

        if (index == 0) {
            std::cout << "Unary operation not supported?" << std::endl;
            return;
        }

        if (index == (int)(node->children.size() - 1)){
            std::cout << "Operator missing right operand" << std::endl;
            return;
        }

        auto left = node->children[index - 1];
        auto right = node->children[index + 1];

        if (left->type == LIME_NODE_EXPRESSION) SortExpression(left);
        if (right->type == LIME_NODE_EXPRESSION) SortExpression(right);
        
        op->children.push_back(left);
        op->children.push_back(right);

        node->children.erase(node->children.begin() + (index - 1), node->children.begin() + (index + 2));
        node->children.insert(node->children.begin() + (index - 1), op);
    }
}

Node* PackExpression(std::vector<Token>::iterator it, std::vector<Token>::iterator end) {
    auto node = new Node();
    node->type = LIME_NODE_EXPRESSION;

    auto i = it;

    while (i != end) {
        //std::cout << "><>" << *i << std::endl;

        // TODO: Handle all types
        switch ((*i).type) {
            case LIME_NUMBER: 
            case LIME_IDENTIFIER:
            case LIME_OPERATOR:
                node->children.push_back(TokenToNode(*i)); 
                break;

            case LIME_OPEN_PAREN: {
                // Get all in parentheses 
                // NOTE: This does not include the surrounding parenthesis
                
                auto expr_end = (i + 1);
                auto scope = 0;
                while(expr_end != end){
                    if ((*expr_end).type == LIME_OPEN_PAREN)
                        scope++;
                    else if ((*expr_end).type == LIME_CLOSE_PAREN) {
                        if (scope == 0)
                            break;
                        scope--;
                    }
                    expr_end++;
                }
                
                auto expression = std::vector<Token>((i + 1), expr_end);
                auto expression_node = PackExpression(expression.begin(), expression.end());
                node->children.push_back(expression_node);
                i = expr_end;

                break;
            }
            case LIME_CLOSE_PAREN:
                break;
            default:
                //assert(0);
                break;
        }
        
        ++i;
    }

    // Sort the nodes into presidence
    SortExpression(node);

    return node;
}

void code_block_to_ast(Ast* ast, std::vector<Token>& tokens) {
    auto it = tokens.begin();

    using namespace std;
    auto getPrev = [&]() -> auto {
        auto prev = it;
        do {
            if (prev == tokens.begin())
                return std::make_tuple(prev, false);
            --prev;
        } while((*prev).isWhiteSpace);
        return std::make_tuple(prev--, true);
    };

    //auto Next = [&]() -> auto {
    //    ++it;
    //    while((*it)->isWhiteSpace)
    //        ++it;
    //    return *it;
    //};

    //auto Peek = [&]() -> auto {
    //    auto begin = it;
    //    begin++;
    //    while((*begin)->isWhiteSpace)
    //        ++begin;
    //    return *begin;
    //};

    //auto NextI = [](std::vector<Token>::iterator& i) {
    //    ++i;
    //    while((*i)->isWhiteSpace)
    //        ++i;
    //    return *i;
    //};

    while (it != tokens.end()) {

        if ((*it).isWhiteSpace){
            if ((*it).type == LIME_NEWLINE)
                ast->line_number++;
            ++it;
            continue;
        }

        // Handle operators
        switch((*it).type) {
            case LIME_NEWLINE: {
                ast->line_number++;
                break;
            }

            case LIME_OPERATOR: {
                switch((*it).op) {
                    case LIME_ASSIGNMENT_OPERATOR: {
                            
                        // Get the identifier
                        auto [prev, res] = getPrev();
                        assert(res); 

                        auto node = new Node();

                        if ((*prev).type == LIME_TYPE_IDENTIFIER) {
                            node->variable_type = new Token(*prev);
                            prev--;
                            while((*prev).isWhiteSpace)
                                prev--;

                            if ((*prev).type == LIME_MUTABLE) {
                                node->canMutate = true;
            
                                prev--;
                                while((*prev).isWhiteSpace)
                                    prev--;
                            }
                        } 

                        assert((*prev).type == LIME_IDENTIFIER);

                        ++it;
                        auto expression         = GetExpressionTokens(it, tokens.end());
                        auto expression_node    = PackExpression(expression.begin(), expression.end());

                        node->type = LIME_NODE_VARIABlE_ASSIGNMENT;
                        node->children.push_back(expression_node);
                        node->identifier = new Token(*prev); // This might cause issues if the tokens go out of scope...

                        ast->children.push_back(node);

                        break;
                    }

                    default: {
                        //assert(false);
                        break;
                    }
                }

                break;
            }

            default: {

                break;
            }
        }
        ++it;
    }
}

//LimeNodeTypes GetTypeOfExpression() {}

Ast create_ast_from_tokens(std::vector<Token>& tokens) {
    auto ast = Ast{};

    code_block_to_ast(&ast, tokens);

    

    std::cout << (ast) << std::endl;

    return ast;
}
