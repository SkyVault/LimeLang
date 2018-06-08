#include "ast.h"
#include "lime.h"
#include <sstream>

Node* handle_function_call(std::vector<Token>::iterator& it, std::vector<Token>::iterator last); 

Node::Node(Token& token) {
    this->token = token;

    if (this->token.type == LIME_NUMBER)
        this->type = LIME_NODE_NUMBER_LITERAL;

    if (this->token.type == LIME_OPERATOR) 
        this->type = LIME_NODE_OPERATOR;
}

Node::Node(){}

Node* Node::add(Node* node) {
    children.push_back(node);
    return node;
}

std::string Node::ToString(std::string indent = "") const {
    std::string result{""};

    result += indent + "(" + LimeNodeTypesNames.find(type)->second;
    result += " line: " + std::to_string(token.line_number) + ",";
    if (token.word.size() > 0)
        result += " word: \"" + token.word + "\"";

    if (type == LIME_NODE_PROC_DEFINITION || type == LIME_NODE_PROC_DECLARATION) {
        assert(identifier != nullptr);
        auto type_name = variable_type == nullptr ?  "none" : variable_type->word;
        result += " proc: " + identifier->word + ", ";
        result += " return_type: " + type_name;
    }

    if (type == LIME_NODE_VARIABLE_ASSIGNMENT || type == LIME_NODE_VARIABLE_DECLARATION) {
        assert(identifier != nullptr);
        result += " var: " + identifier->word + ",";
        //result += " type: " + cp + ",";
    }

    if (type == LIME_NODE_PROC_CALL) {
        assert(identifier != nullptr);
        result += " proc: " + identifier->word + ",";
    }

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

std::vector<Token> GetExpressionTokens(std::vector<Token>::iterator& it, std::vector<Token>::iterator end) {
    auto Peek = [&]() -> auto {
        auto begin = it;
        begin++;
        while((*begin).isWhiteSpace)
            ++begin;
        return begin;
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
            case LIME_COMMA:
            case LIME_OPERATOR:
                ++it;
                break;
            
            case LIME_IDENTIFIER:

                // make sure the identifier isn't for a structure or procedure
                if ((it + 1) != end) {
                    auto next = Peek();
                    
                    //TODO: Handle structures and other things that come after an identifier
                    switch(next->type) {
                        case LIME_PROC:                 running = false; break;
                        case LIME_ASSIGNMENT_OPERATOR:  running = false; break;
                        case LIME_TYPE_IDENTIFIER:      running = false; break;
                        case LIME_MUTABLE:              running = false; break;
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
        case LIME_STRING:
            result->type = LIME_NODE_STRING_LITERAL;
            break;
        case LIME_CHARACTER:
            result->type = LIME_NODE_CHARACTER_LITERAL;
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
            Error("Unary operator is not yet supported: " + op->token.ToString(), op->token.line_number);
            return;
        }

        if (index == (int)(node->children.size() - 1)){
            Error("Operator missing right operand", op->token.line_number);
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

    auto Peek = [&]() -> auto {
        auto begin = i;
        begin++;
        while(begin->isWhiteSpace)
            ++begin;
        return begin;
    };

    while (i != end) {
        // TODO: Handle all types
        switch ((*i).type) {
            case LIME_IDENTIFIER:
                if (Peek()->type == LIME_OPEN_PAREN) {
                    // This is probably a function call
                    node->children.push_back(handle_function_call(i, end)); 
                } else {
                    node->children.push_back(TokenToNode(*i)); 
                }
                break;
                
            case LIME_NUMBER: 
            case LIME_STRING:
            case LIME_CHARACTER:
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
                if (i->isWhiteSpace) break;
                Error("Un expected token in expression: " + i->word, i->line_number);
                break;
        }
        
        ++i;
    }

    // Sort the nodes into presidence
    SortExpression(node);

    return node;
}

void get_all_within_tokens(std::vector<Token>::iterator& it, std::vector<Token>::iterator end, std::string tstart = "{", std::string tend = "}") {
    int scope{0};
    while (it != end) {
        if (it->word == tstart) {
            scope++;
        }
        if (it->word == tend) {
            if (scope == 0) { ++it; return; }
            scope--;
        }
        it++;
    }
}

Node* parameters_to_node(std::vector<Token>& tokens) {
    /*
     * TODO: 
     * When working on these parameters we should start from the identifier
     * and then check the the next values are what we expect, instead
     * of starting with a type identifier
     */

    auto result = new Node();
    auto it = tokens.begin();

    result->type = LIME_NODE_PARAMETER_LIST;

    bool next_not_comma{true};
    while (it != tokens.end()) {

        switch(it->type) {
            case LIME_TYPE_IDENTIFIER:{ 
                auto p = it - 1;
                if (it == tokens.begin()) {
                    Error("Type is missing identifier: " + p->word, p->line_number);
                    return result;
                }

                while (p->isWhiteSpace && p != tokens.begin()) p--;

                if (p->type != LIME_IDENTIFIER) {
                    Error("Identifier expected but got: " + p->word, p->line_number);
                }

                auto node = new Node();
                node->type = LIME_NODE_VARIABLE_DECLARATION;
                node->identifier = new Token(*p);
                node->variable_type = new Token(*it);
                result->children.push_back(node);

                // Make sure the next token is either a paren or comma
                auto n = it + 1;
                while (n->isWhiteSpace) n++;

                if (n != tokens.end() && n->word != "" && n->type != LIME_COMMA && n->type != LIME_CLOSE_PAREN)
                    Error("Expected a comma or a closing paren but got: " + n->word, n->line_number);

                next_not_comma = false;
                break;
            }

            case LIME_COMMA:        /* Do nothing */ { 
                if (next_not_comma)
                    Error("Unexpected comma", it->line_number);
                next_not_comma = true;
                break; 
            }

            case LIME_IDENTIFIER:   /* Do nothing */ { next_not_comma = false; break; }

            default:
                if (it->isWhiteSpace == false)
                    Error("Miss placed token: " + it->word, it->line_number);
                break;
        }

        ++it;
    }

    return result;
}

Node* handle_function_call(std::vector<Token>::iterator& it, std::vector<Token>::iterator last) {
    auto ident = it;
    it += 2;
    auto end = it;

    get_all_within_tokens(end, last, "(", ")");

    auto arguments = std::vector<Token>(it, end - 1);

    auto argument_node = new Node();
    argument_node->type = LIME_NODE_ARGUMENT_LIST;

    it = end - 1;

    end = arguments.begin(); 
    auto start = end;  
    auto scope{0};
    while(end != arguments.end()){
        if (end->type == LIME_OPEN_PAREN)
            scope++;
        else if (end->type == LIME_CLOSE_PAREN)
            scope--;
        else if (end->type == LIME_COMMA && scope == 0) {
            auto expression = std::vector<Token>(start, end);
            auto expression_node = PackExpression(expression.begin(), expression.end());
            argument_node->children.push_back(expression_node);
            start = end + 1;
        }

        ++end;

        if (end == arguments.end()){
            auto expression = std::vector<Token>(start, end);
            auto expression_node = PackExpression(expression.begin(), expression.end());
            argument_node->children.push_back(expression_node);
            start = end;
        }
    }

    auto proc_call_node = new Node();
    proc_call_node->identifier = new Token(*ident);
    proc_call_node->type = LIME_NODE_PROC_CALL;
    proc_call_node->children.push_back(argument_node);
    return proc_call_node;
}

void code_block_to_ast(Node* ast, std::vector<Token>& tokens) {
    auto it = tokens.begin();
    auto back = tokens.end();

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

    auto Next = [&]() -> auto {
        ++it;
        while((*it).isWhiteSpace){
            ++it;
        }
        return it;
    };

    auto Peek = [&]() -> auto {
        auto begin = it;
        begin++;
        while(begin->isWhiteSpace)
            ++begin;
        return begin;
    };

    //auto NextI = [](std::vector<Token>::iterator& i) {
    //    ++i;
    //    while((*i)->isWhiteSpace)
    //        ++i;
    //    return *i;
    //};

    while (it != tokens.end()) {

        if ((*it).isWhiteSpace){
            ++it;
            continue;
        }

        auto it_type = it->type;

        // Handle operators
        switch((*it).type) {
            case LIME_NEWLINE: {
                break;
            }

            // HERE FOR NEW TOKEN
            case LIME_METATAG: {
                auto next = Next();
                if (next->type == LIME_IDENTIFIER) {

                    if (next->word == "emit") {
                        next = Next();                        

                        if (next->type != LIME_OPEN_CURLY_BRACKET) {
                            Error("The metatag emit requires a code block", next->line_number);
                        }

                        auto end = it + 1;
                        get_all_within_tokens(end, tokens.end());
                        auto items = std::vector<Token>(it + 1, end - 1);

                        std::stringstream ss;
                        for (auto v : items) {
                            ss << v.word;
                        }

                        auto node = new Node();
                        node->type = LIME_NODE_EMIT;
                        node->token.word = ss.str();
                        ast->children.push_back(node);

                        it = end-1;

                    } else {
                        Error("Unknown metatag: " + next->word, next->line_number);
                    }

                } else {
                    Error("Metatag $ requires an identifier", next->line_number);
                }

                break;
            }

            case LIME_RETURN: {
                ++it;
                auto expression = GetExpressionTokens(it, tokens.end());
                auto expression_node = PackExpression(expression.begin(), expression.end());

                auto node = new Node();
                node->type = LIME_NODE_RETURN;
                node->children.push_back(expression_node);
                ast->children.push_back(node);

                break;
            }

            // Handle while loops
            case LIME_WHILE: {

                ++it;
                auto beg = it;
                while (it != tokens.end() && it->type != LIME_OPEN_CURLY_BRACKET)
                    ++it;

                auto expression = std::vector<Token>(beg, it);
                auto expression_node = PackExpression(expression.begin(), expression.end());

                auto while_node = new Node();
                while_node->type = LIME_NODE_WHILE_LOOP;
                while_node->children.push_back(expression_node);

                auto end = it + 1;
                get_all_within_tokens(end, tokens.end());
                auto block = std::vector<Token>(it + 1, end - 1);
               
                auto block_node = new Node();

                code_block_to_ast(block_node, block);
                while_node->children.push_back(block_node);
                it = end-1;

                ast->children.push_back(while_node);

                break;
            }
            
            // Handle if statements
            case LIME_IF: 
            case LIME_ELIF: {

                ++it;
                auto beg = it;
                while (it != tokens.end() && it->type != LIME_OPEN_CURLY_BRACKET)
                    ++it;

                auto expression = std::vector<Token>(beg, it);
                auto expression_node = PackExpression(expression.begin(), expression.end());

                auto while_node = new Node();
                while_node->type = (it_type == LIME_IF ? LIME_NODE_IF_STATEMENT : LIME_NODE_ELSEIF_STATEMENT);
                while_node->children.push_back(expression_node);

                auto end = it + 1;
                get_all_within_tokens(end, tokens.end());
                auto block = std::vector<Token>(it + 1, end - 1);
               
                // HERE
                auto block_node = new Node();

                code_block_to_ast(block_node, block);
                while_node->children.push_back(block_node);
                it = end-1;

                ast->children.push_back(while_node);

                break;
            }

            case LIME_ELSE: {
                auto now = it;
                auto else_node = new Node();
                else_node->type = LIME_NODE_ELSE_STATEMENT;

                auto end = it + 1;
                if (Peek()->type != LIME_OPEN_CURLY_BRACKET) 
                    Error("Else statement requires a code block", now->line_number);
                get_all_within_tokens(end, tokens.end());
                auto block = std::vector<Token>(it + 1, end - 1);
               
                // HERE
                auto block_node = new Node();

                code_block_to_ast(block_node, block);
                else_node->children.push_back(block_node);
                it = end-1;

                ast->children.push_back(else_node);

                break;
            }

            // Handle procedures
            case LIME_PROC: {
                
                // Get the Identifier
                // TODO: Handle return type declarations
                
                auto [prev, res] = getPrev();
                assert(prev->type == LIME_IDENTIFIER);

                Next();
                bool is_decl{true};

                auto node = new Node();
                node->identifier = new Token(*prev);
                
                auto expect_proc_tokens = [](const Token& it) {
                    if (it.type != LIME_TYPE_IDENTIFIER &&
                        it.type != LIME_OPEN_PAREN      &&
                        it.type != LIME_OPEN_CURLY_BRACKET) {

                        Error("Missplaced identifier for proc return type: " + it.word, it.line_number);
                    }
                };

                expect_proc_tokens(*it);

                if (it->type == LIME_TYPE_IDENTIFIER) {
                    node->variable_type = new Token(*it);  
                    Next();
                } 

                if (it->type == LIME_OPEN_CURLY_BRACKET) {
                    // We know that there are no parameters
                    is_decl = false; 
                } else if (it->type == LIME_OPEN_PAREN) {
                    // There might be parameters
                     
                    // Get the parameters
                    // TODO: Extract this into its own function so that it 
                    // can be used for things like macros and stuff
                    auto end = it + 1;
                    while (true) {
                        if (end->type == LIME_CLOSE_PAREN){
                            break;
                        }

                        assert(end != back);

                        end++;
                    }

                    auto parameters = std::vector((it + 1), end);
                    //TODO: Handle parameters
                   
                    // Check the correctness of the parameters

                    auto parameters_node = parameters_to_node(parameters);
                    node->children.push_back(parameters_node);

                    it = end;
                    Next();

                    expect_proc_tokens(*it);

                    if (it->type == LIME_TYPE_IDENTIFIER) {
                        node->variable_type = new Token(*it);
                        Next();
                    }

                    if (it->type == LIME_OPEN_CURLY_BRACKET) {
                        is_decl = false;
                    }
                } 

                // Handle funciton code blocks
                if (is_decl == false) {
                    auto end = it + 1;
                    get_all_within_tokens(end, tokens.end());
                    auto block = std::vector<Token>(it + 1, end - 1);
                   
                    // HERE
                    auto block_node = new Node();

                    code_block_to_ast(block_node, block);
                    node->children.push_back(block_node);
                    it = end-1;
                }

                node->type = is_decl ? LIME_NODE_PROC_DECLARATION : LIME_NODE_PROC_DEFINITION;
                ast->children.push_back(node);

                break;
            }

            case LIME_IDENTIFIER: {

                auto next = Peek();
                if (next->type == LIME_TYPE_IDENTIFIER || next->type == LIME_MUTABLE) {
                    // Handle LIME_NODE_VARIABLE_DECLARATION

                    auto node = new Node();
                    node->type = LIME_NODE_VARIABLE_DECLARATION;
                    node->identifier = new Token(*it);

                    next = Next();
                    
                    if (next->type == LIME_MUTABLE) {
                        next = Next();
                        node->canMutate = true;
                    }

                    if (next->type != LIME_TYPE_IDENTIFIER) {
                        Error("Type expected but got: " + next->word, next->line_number);
                    }

                    // NOTE: This seems fishy
                    node->variable_type = new Token(*next);
                    next = Next();
                    
                    if (next->type == LIME_MUTABLE) 
                        Error("Mutable keyword should be before the type", next->line_number);

                    if (next->type == LIME_OPERATOR && next->op == LIME_ASSIGNMENT_OPERATOR) {
                        // Handle expressions
                        node->type = LIME_NODE_VARIABLE_ASSIGNMENT;
                        
                        ++it;
                        auto expression         = GetExpressionTokens(it, tokens.end());
                        auto expression_node    = PackExpression(expression.begin(), expression.end());

                        node->type = LIME_NODE_VARIABLE_ASSIGNMENT;
                        node->children.push_back(expression_node);

                    } else {
                        it = next - 1;
                    }

                    assert(node->identifier);
                    assert(node->variable_type);

                    ast->children.push_back(node);
                    
                } else if (next->type == LIME_OPERATOR) {
                    // Handle LIME_NODE_VARIABLE_ASSIGNMENT
                    if (next->op == LIME_ASSIGNMENT_OPERATOR) {
                        auto node = new Node();
                        node->identifier = new Token(*it);
                        node->type = LIME_NODE_VARIABLE_ASSIGNMENT;

                        it = Next();
                        ++it;
                        auto expression         = GetExpressionTokens(it, tokens.end());
                        auto expression_node    = PackExpression(expression.begin(), expression.end());

                        node->type = LIME_NODE_VARIABLE_ASSIGNMENT;
                        node->children.push_back(expression_node);
                        ast->children.push_back(node);

                    } else {
                        Error("Identifier has a unsupported operator after it.", next->line_number);
                    }

                } else if (next->type == LIME_OPEN_PAREN) {
                    // This is probably a function call
                    ast->children.push_back(handle_function_call(it, tokens.end())); 
                } else {
                    Error("Random identifier", it->line_number);
                }

                if (Peek()->type == LIME_OPEN_PAREN) {
                }
                    
                break;
                // Get the identifier
                //auto [prev, res] = getPrev();
                //assert(res); 

                //auto node = new Node();

                //if ((*prev).type == LIME_TYPE_IDENTIFIER) {
                //    node->variable_type = new Token(*prev);
                //    prev--;
                //    while((*prev).isWhiteSpace)
                //        prev--;

                //    if ((*prev).type == LIME_MUTABLE) {
                //        node->canMutate = true;
    
                //        prev--;
                //        while((*prev).isWhiteSpace)
                //            prev--;
                //    }
                //} 

                //assert((*prev).type == LIME_IDENTIFIER);

                //++it;
                //auto expression         = GetExpressionTokens(it, tokens.end());
                //auto expression_node    = PackExpression(expression.begin(), expression.end());

                //node->type = LIME_NODE_VARIABLE_ASSIGNMENT;
                //node->children.push_back(expression_node);
                //node->identifier = new Token(*prev); // This might cause issues if the tokens go out of scope...

                //ast->children.push_back(node);
            }

            default: {

                break;
            }
        }
        ++it;
    }
}

//LimeNodeTypes GetTypeOfExpression() {}

CodeLens* GetCodeLens() { return Lens; }

bool CodeLens::varExists(const std::string& name) {
    for (int i = variable_scope.size() - 1; i >= 0; i--) {
        if (variable_scope[i].find(name) != variable_scope[i].end())
            return true;
    }
    return false;
}

void CodeLens::addVar(Node* node) {
    std::string copy = std::string{node->identifier->word};
    assert(variable_scope.size() != 0);
    variable_scope[variable_scope.size() - 1].insert(std::make_pair(copy, node));
}

bool CodeLens::procExists(const std::string& name) {
    if (functions.find(name) != functions.end()) 
        return true;
    return false;
}

void CodeLens::addProc(Node* node) {
    std::string copy = std::string{node->identifier->word};
    assert(variable_scope.size() != 0);
    functions.insert(std::make_pair(copy, node));
}

void CodeLens::push() {
    variable_scope.push_back(std::map<std::string, Node*>());
}

void CodeLens::pop() {
    assert(variable_scope.empty() == false);
    variable_scope.pop_back();
}

bool AstPass(Node* ast) {
    bool result{false};
    //assert(ast->type == 0);

    Node* prev = nullptr;
    for (const auto& node : ast->children) {
        switch(node->type) {

            case LIME_NODE_OPERATOR: {
                // Check operands
                AstPass(node);
                break;
            }

            case LIME_NODE_IDENTIFIER: {
                if (ast->type == LIME_NODE_EXPRESSION || ast->type == LIME_NODE_OPERATOR) {
                    // Treat the identifier as a variable value
                    if (!Lens->varExists(node->token.word)) {
                        Error("Undefined identifier: " + node->token.word + ".", node->token.line_number);
                    }
                }

                break;
            }

            case LIME_NODE_EXPRESSION: {
                AstPass(node);
                break;
            }

            case LIME_NODE_IF_STATEMENT:
            case LIME_NODE_ELSEIF_STATEMENT:
            case LIME_NODE_WHILE_LOOP: {
                assert(node->children.size() == 2);

                AstPass(node->children[0]);

                Lens->push();
                AstPass(node->children[1]);
                Lens->pop();
                
                if (node->type == LIME_NODE_ELSEIF_STATEMENT) {
                    if (prev == nullptr || prev->type == LIME_NODE_ELSEIF_STATEMENT)
                        if (prev->type != LIME_NODE_IF_STATEMENT || prev->type != LIME_NODE_ELSEIF_STATEMENT) {
                            Error("Randomly placed \'elif\' statement, should this just be an \'if\'?", node->token.line_number);
                        }
                }


                auto name = (std::string{
                        (node->type == LIME_NODE_IF_STATEMENT ? "If statement" : 
                         (node->type == LIME_NODE_WHILE_LOOP ? "While loop" : "Elif"))});
                    
                if (node->children[0]->children.size() == 0) {
                    Error(name + " is missing an expression", node->token.line_number);
                }

                if (node->children[0]->children[0]->type == LIME_NODE_EXPRESSION)
                    if (node->children[0]->children[0]->children.size() == 0) {
                        Error(name + " has an empty expression", node->token.line_number);
                    }

                break;
            }

            case LIME_NODE_ELSE_STATEMENT: {
                Lens->push();
                AstPass(node->children[0]);
                Lens->pop();
                break;
            }

            case LIME_NODE_RETURN: {
                if (node->children.size() > 0)
                    AstPass(node->children[0]);
                break;
            }

            case LIME_NODE_VARIABLE_ASSIGNMENT: {

                if (node->variable_type != nullptr) {
                    // It is a declaration

                    if (Lens->varExists(node->identifier->word)) {
                        Error("Redefinition of identifier: " + node->identifier->word + ".", node->token.line_number);
                    }

                    Lens->addVar(node);
                } else {
                    if (!Lens->varExists(node->identifier->word)) {
                        Error("Undefined identifier: " + node->identifier->word + ".", node->token.line_number);
                    }
                }

                // Check the expression
                if (node->children.size() > 0 && node->children[0]->type == LIME_NODE_EXPRESSION) {
                    AstPass(node->children[0]);
                }

                break;
            }

            case LIME_NODE_PROC_DECLARATION: {
                // Make sure the function is unique
                if (Lens->procExists(node->identifier->word)) {
                    Error("Redefinition of procedure: " + node->identifier->word, node->token.line_number);
                }

                Lens->addProc(node);

                break;
            }

            // TODO: Make sure the prototypes match
            case LIME_NODE_PROC_DEFINITION: {
                // Make sure the function is unique
                auto prev = Lens->functions.find(node->identifier->word);
                if (prev != Lens->functions.end() && prev->second->type == LIME_NODE_PROC_DEFINITION){
                    Error("Redefinition of procedure: " + node->identifier->word, node->token.line_number);
                }

                Lens->addProc(node);

                Lens->push();
                for (auto v: node->children[0]->children) {
                    if (v->type == LIME_NODE_VARIABLE_DECLARATION) {
                        Lens->addVar(v);
                    }
                }
                AstPass(node->children[1]);
                Lens->push();

                break;
            }

            case LIME_NODE_PROC_CALL: {
                assert(node->identifier); 
                
                //TODO: Test that the arguments match the functions prototype
                if (!Lens->procExists(node->identifier->word)){
                    Error("Undefined procedure: " + node->identifier->word, node->identifier->line_number);
                }

                break;
            }

            default:
                break;
        }
        prev = node;
    }

    return result;
}

Node* create_ast_from_tokens(std::vector<Token>& tokens) {
    Lens = new CodeLens();

    auto ast = new Node(); 

    code_block_to_ast(ast, tokens);

    //TODO: Remove this hack, this is just a hack so that it wont give us an
    // error when we call the print function
    Lens->functions.insert(std::make_pair("printf", new Node()));
    //Lens->functions.insert(std::make_pair("getchar", new Node()));

    Lens->push();
    AstPass(ast);
    Lens->pop();

    return ast;
}
