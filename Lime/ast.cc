#include "ast.h"
#include "lime.h"
#include <sstream>

#define VDUMPD(v) for (auto it : v) std::cout << "it: " << *it << std::endl;
#define VDUMP(v) for (auto it : v) std::cout << "it: " << it << std::endl;

Node* handle_function_call(std::vector<Token>::iterator& it, std::vector<Token>::iterator last); 

Node::Node(Token& token): Node() {
    this->token = token;

    if (this->token.type == LIME_NUMBER)
        this->type = LIME_NODE_NUMBER_LITERAL;

    if (this->token.type == LIME_OPERATOR) 
        this->type = LIME_NODE_OPERATOR;
}

Node::Node(){}
Node::~Node(){}

IdentifierNode::IdentifierNode(Token& token): Node(token) {}

IdentifierNode::IdentifierNode(){
    // Initialize to nullptrs
    type_desc = nullptr;
    identifier = nullptr;
}

IdentifierNode::~IdentifierNode(){
    if (identifier != nullptr)
        delete identifier;
    if (type_desc != nullptr)
        delete type_desc;
}

bool ProcPrototypeNode::matches(ProcCallNode* proccall) {
    // * This does not handle optional parameters and named paramters
    // * This also does not support variadic parameters

    if (parameters.size() != proccall->parameters.size()) {
        Error("Wrong number of parameters for function call: " + 
            proccall->identifier->word + 
            ", got: " + 
            std::to_string(proccall->parameters.size()) + " but expected: " + std::to_string(parameters.size()), 
            proccall->identifier->line_number);

        return false;
    }

    for (int i = 0; i < proccall->parameters.size(); i++){
        if (std::get<1>(proccall->parameters[i]) != std::get<1>(parameters[i])){
            Error("Wrong parameter type", proccall->identifier->line_number);
            return false;
        }
    }

    return true;
}

Node* Node::add(Node* node) {
    children.push_back(node);
    return node;
}

std::string ToString(Node* node, std::string indent = "") {
    std::string result{""};

    const auto type = node->type;

    result += indent + "(" + LimeNodeTypesNames.find(node->type)->second;
    result += " line: " + std::to_string(node->token.line_number) + ",";
    if (node->token.word.size() > 0)
        result += " word: \"" + node->token.word + "\"";

    if (type == LIME_NODE_PROC_DEFINITION || type == LIME_NODE_PROC_DECLARATION) {
        auto n = static_cast<IdentifierNode*>(node);
        assert(n->identifier != nullptr);

        std::string type_name{"none"};

        if (n->type_desc != nullptr){
            if (n->type_desc->isCustomType){
                type_name = static_cast<LimeCustomTypeDesc*>(n->type_desc)->name;
            } else {
                type_name = LimeStringTypeMap.find(static_cast<LimeTypeDesc*>(n->type_desc)->type)->second;
            }
        }

        result += " proc: " + n->identifier->word + ", ";
        result += " return_type: " + type_name;
    }

    if (type == LIME_NODE_VARIABLE_ASSIGNMENT || type == LIME_NODE_VARIABLE_DECLARATION) {
        auto n = static_cast<IdentifierNode*>(node);
        assert(n->identifier != nullptr);
        result += " var: " + n->identifier->word + ",";
        //result += " type: " + cp + ",";
    }

    if (type == LIME_NODE_PROC_CALL) {
        // ! Fix this to use ProcCallNodeStruct
        auto n = static_cast<IdentifierNode*>(node);
        assert(n->identifier != nullptr);
        result += " proc: " + n->identifier->word + ",";
    }

    if (node->children.size() > 0) {
        result += " (\n";
        size_t i = 0;
        for(auto c : node->children) {
            result += ToString(c, indent + "  ");
            if (i != node->children.size() - 1)
                result += "\n";
            i += 1;
        }
        result += ")";
    } 

    return result + ")";
}

ostream& operator<<(ostream& os, Node& node) {
    std::string str = ToString(&node);
    os << str;
    return os;
}

std::vector<Token> GetExpressionTokens(std::vector<Token>::iterator& it, std::vector<Token>::iterator end) {
    auto Peek = [&]() -> auto {
        auto begin = it;
        begin++;
		if (begin == end) return begin;
        while(begin != end && begin->isWhiteSpace)
            ++begin;
        return begin;
    };

    auto begin = it;
    auto t = (*it).type;

    bool running{true};
    bool last_was_operator{true};
    while (running) {
        if (it == end) break;
        switch((*it).type) {
            case LIME_WHITESPACE:
            case LIME_NUMBER:
            case LIME_CLOSE_PAREN:
            case LIME_COMMA:
                ++it;
                break;

            case LIME_OPEN_PAREN:
                last_was_operator = true;
				++it;
				break;

            case LIME_OPERATOR:
                last_was_operator = true;
                ++it;
                break;
            
            case LIME_IDENTIFIER:
                if (!last_was_operator){
                    running = false;
                    break;
                }

                last_was_operator = false;

                // make sure the identifier isn't for a structure or procedure
                if ((it + 1) != end) {
                    auto next = Peek();

					if (next == end) {
						running = false;
						++it;
						continue;
					}
                    
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
    
    auto result = std::vector<Token>(begin, it);
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
        while(begin != end && begin->isWhiteSpace)
            ++begin;
        return begin;
    };

    while (i != end) {
        // TODO: Handle all types
        switch ((*i).type) {
			case LIME_IDENTIFIER: {
				auto t = Peek();
				if (t != end && t->type == LIME_OPEN_PAREN) {
					// This is probably a function call
					node->children.push_back(handle_function_call(i, end));
				}
				else {
					node->children.push_back(TokenToNode(*i));
				}
				break;
			}
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

                auto node = new IdentifierNode();
                node->type = LIME_NODE_VARIABLE_DECLARATION;
                node->identifier = new Token(*p);

                if (isType(it->word)){
                    node->type_desc = new LimeTypeDesc{ LimeTypeStringMap[it->word] };
                } else {
                    // ! We need to support custom struct types
                    Error("We dont yet support custom types...", -1);
                }

                result->children.push_back(node);

                // Make sure the next token is either a paren or comma
                auto n = it + 1;
                while (n != tokens.end() && n->isWhiteSpace) n++;

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

    // ! Fix this to use the ProcCallNode struct
    // * NOTES: Okay so we cant add the arguments to the parameters list because we dont know the type
    // * of each expression, so we probably need to do a pass on the ast after everything is packed together 
    auto proc_call_node = new ProcCallNode();
    proc_call_node->identifier = new Token(*ident);
    proc_call_node->type = LIME_NODE_PROC_CALL;
    proc_call_node->children.push_back(argument_node);
    return proc_call_node;
}

void code_block_to_ast(Node* ast, std::vector<Token>& tokens) {
    using namespace std;

    auto it = tokens.begin();
    auto back = tokens.end();

    auto Next = [&]() -> auto {
        ++it;
		if (it == tokens.end()) return it;
        while(it->isWhiteSpace){
            ++it;
			if (it == tokens.end()) return it;
        }
        return it;
    };

    auto Peek = [&]() -> auto {
        auto begin = it;
        begin++;
        while(begin != tokens.end() && begin->isWhiteSpace)
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

        if (it->word == ";")
            ++it;

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
                    } else if (next->word == "include") {
                        next = Next();                        

                        if (next->type != LIME_STRING) {
                            Error("Include statment expects a string literal with the name of the c file", next->line_number);
                        }
                        
                        auto node = new Node();
                        node->type = LIME_NODE_C_INCLUDE;
                        node->token.word = next->word.substr(1, next->word.size() - 2);

                        ast->children.push_back(node);

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

            case LIME_IDENTIFIER: {
                auto next = Peek();

                if (next->type == LIME_TYPE_IDENTIFIER || next->type == LIME_MUTABLE) {
                    // Handle LIME_NODE_VARIABLE_DECLARATION

                    auto node = new IdentifierNode();
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
                    if (isType(next->word)){
                        node->type_desc = new LimeTypeDesc{ LimeTypeStringMap[next->word] };
                    } else {
                        Error("We dont yet support custom types..", -1);
                    }

                    next = Next();
	
					if (next != tokens.end()) {
						if (next->type == LIME_MUTABLE)
							Error("Mutable keyword should be before the type", next->line_number);

						//if (next->type == LIME_)
						// BEER
						if (next->type == LIME_OPEN_SQUARE_BRACKET) {
							auto begin = Next();
							get_all_within_tokens(it, tokens.end(), "[", "]");

							node->isArray = true;

							auto expression = std::vector<Token>(begin, it - 1);
							for (const auto e : expression) {
								if (!e.isWhiteSpace)
									Error("We do not support fixed size arrays", e.line_number);
							}
							--it;
						}

						if (next->type == LIME_OPERATOR && next->op == LIME_ASSIGNMENT_OPERATOR) {
							// Handle expressions
							node->type = LIME_NODE_VARIABLE_ASSIGNMENT;

							++it;
							auto expression = GetExpressionTokens(it, tokens.end());
							auto expression_node = PackExpression(expression.begin(), expression.end());

							node->type = LIME_NODE_VARIABLE_ASSIGNMENT;
							node->children.push_back(expression_node);

						}
						else {
							if (!node->isArray) it = next - 1;
						}
					}

                    assert(node->identifier);
                    assert(node->type_desc);

                    ast->children.push_back(node);
                    
                } else if (next->type == LIME_OPERATOR) {
                    // Handle LIME_NODE_VARIABLE_ASSIGNMENT
                    if (
                        next->op == LIME_ASSIGNMENT_OPERATOR            ||
                        next->op == LIME_PLUS_EQUALS_OPERATOR           ||
                        next->op == LIME_MINUS_EQUALS_OPERATOR          ||
                        next->op == LIME_MULTIPLICATION_EQUALS_OPERATOR ||
                        next->op == LIME_DIVISION_EQUALS_OPERATOR
                        ) {
                        
                        // ! Fix use the AssignmentOperatorNode struct
                        auto node = new IdentifierNode();
                        node->token = *next;
                        node->identifier = new Token(*it);

                        // ! This seems not correct, why the ++it?

                        it = Next();
                        ++it;
                        auto expression         = GetExpressionTokens(it, tokens.end());
                        auto expression_node    = PackExpression(expression.begin(), expression.end());

                        node->type = LIME_NODE_VARIABLE_ASSIGNMENT;
                        node->children.push_back(expression_node);
                        ast->children.push_back(node);

                    } else if (next->op == LIME_LEFT_ARROW_OPERATOR) {
                        // * Handle array insertion
                        // ! Fix use the AssignmentOperatorNode struct
                        auto node = new IdentifierNode();
                        node->identifier = new Token(*it);

                        // ! This seems not correct, why the ++it?
                        it = Next();
                        ++it;
                        auto expression         = GetExpressionTokens(it, tokens.end());
                        auto expression_node    = PackExpression(expression.begin(), expression.end());

                        node->type = LIME_NODE_ARRAY_INSERTION;
                        node->children.push_back(expression_node);
                        ast->children.push_back(node);

                    } else {
                        Error("Identifier has a unsupported operator after it.", next->line_number);
                    }
                } else if (next->type == LIME_PROC) {
                    // Handling procedures
                    
                    //! Fix to use the ProcNode
                    auto node = new ProcPrototypeNode();
                    node->identifier = new Token(*it);

                    Next();
                    Next();

                    bool is_decl{true};

                    
                    auto expect_proc_tokens = [](const Token& it) {
                        if (it.type != LIME_TYPE_IDENTIFIER &&
                            it.type != LIME_OPEN_PAREN      &&
                            it.type != LIME_OPEN_CURLY_BRACKET) {

                            Error("Missplaced identifier for proc return type: " + it.word, it.line_number);
                        }
                    };

                    expect_proc_tokens(*it);

                    if (it->type == LIME_TYPE_IDENTIFIER) {
                        node->type_desc = new LimeTypeDesc{ LimeTypeStringMap[it->word] };
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

                        auto parameters = std::vector<Token>((it + 1), end);
                        //TODO: Handle parameters
                       
                        // Check the correctness of the parameters

                        auto parameters_node = parameters_to_node(parameters);

                        for (auto _param : parameters_node->children){
                            auto param = static_cast<IdentifierNode*>(_param);

                            assert(param->type_desc);

                            node->parameters.push_back({
                                param->token.word,
                                static_cast<LimeTypeDesc*>(param->type_desc)->type
                            });
                        }

                        node->children.push_back(parameters_node);

                        it = end;
                        Next();

                        expect_proc_tokens(*it);

                        if (it->type == LIME_TYPE_IDENTIFIER) {
                            node->type_desc = new LimeTypeDesc{ LimeTypeStringMap[it->word] };
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

                } else if (next->type == LIME_OPEN_PAREN) {
                    // This is probably a function call
                    ast->children.push_back(handle_function_call(it, tokens.end())); 
                } else {
                    Error("Random identifier: " + it->word, it->line_number);
                }

                break;
            }

            default: {
				//assert(0);
                break;
            }
        }

		if (it != tokens.end())
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

void CodeLens::addVar(IdentifierNode* node) {
    std::string copy = std::string{node->identifier->word};
    assert(variable_scope.size() != 0);
    variable_scope[variable_scope.size() - 1].insert(std::make_pair(copy, node));
}

IdentifierNode* CodeLens::getVar(const std::string& name) {
    for (int i = variable_scope.size() - 1; i >= 0; i--) {
        auto res = variable_scope[i].find(name);
        if (res != variable_scope[i].end())
            return res->second;
    }
    return nullptr;
}


bool CodeLens::procExists(const std::string& name) {
    if (functions.find(name) != functions.end()) 
        return true;
    return false;
}

ProcPrototypeNode* CodeLens::getProc(const std::string& name) {
    return functions[name];
}

void CodeLens::addProc(ProcPrototypeNode* node) {
    //! Fix
    std::string copy = std::string{node->identifier->word};
    assert(variable_scope.size() != 0);
    functions.insert(std::make_pair(copy, node));
}

void CodeLens::push() {
    variable_scope.push_back(std::map<std::string, IdentifierNode*>());
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

            case LIME_NODE_ARRAY_INSERTION: {
                auto id = static_cast<IdentifierNode*>(node);
                if (!Lens->varExists(id->identifier->word)) {
                    Error("Undefined identifier: " + id->identifier->word + ".", node->token.line_number);
                } 

                auto var = Lens->getVar(id->identifier->word);
                if (var->isArray == false) {
                    Error("Attempted to insert into a non array variable", var->token.line_number);
                }

                if (var->canMutate == false) {
                    Error("Attempted to modify an immutable variable: " + id->identifier->word, id->token.line_number);
                }

                // Check the expression
                if (id->children.size() > 0 && id->children[0]->type == LIME_NODE_EXPRESSION) {
                    AstPass(node->children[0]);
                } else {
                    Error("Array insertion requires a value", node->token.line_number);
                }

                break;
            }

            case LIME_NODE_VARIABLE_DECLARATION: {
                auto id = static_cast<IdentifierNode*>(node);
                if (id->type_desc == nullptr) {
                    Error("Variable declaration is missing a type identifier", node->token.line_number);
                }

                if (Lens->varExists(id->identifier->word)) {
                    Error("Redefinition of identifier: " + id->identifier->word + ".", id->token.line_number);
                }

                Lens->addVar(id);
                break;
            }

            case LIME_NODE_VARIABLE_ASSIGNMENT: {
                auto id = static_cast<IdentifierNode*>(node);
                if (id->type_desc != nullptr) {
                    // It is a declaration

                    if (Lens->varExists(id->identifier->word)) {
                        Error("Redefinition of identifier: " +id->identifier->word + ".",id->token.line_number);
                    }

                    Lens->addVar(id);
                } else {
                    if (!Lens->varExists(id->identifier->word)) {
                        Error("Undefined identifier: " +id->identifier->word + ".",id->token.line_number);
                    }
                }

                auto var = Lens->getVar(id->identifier->word);
                if (var->canMutate == false) {
                    Error("Attempted to modify an immutable variable: " +id->identifier->word,id->token.line_number);
                }

                // Check the expression
                if (id->children.size() > 0 &&id->children[0]->type == LIME_NODE_EXPRESSION) {
                    AstPass(id->children[0]);
                }

                break;
            }

            case LIME_NODE_PROC_DECLARATION: {
                auto id = static_cast<ProcPrototypeNode*>(node);
                // Make sure the function is unique
                if (Lens->procExists(id->identifier->word)) {
                    Error("Redefinition of procedure: " +id->identifier->word,id->token.line_number);
                }

                Lens->addProc(id);

                break;
            }

            // TODO: Make sure the prototypes match
            case LIME_NODE_PROC_DEFINITION: {
                auto id = static_cast<ProcPrototypeNode*>(node);
                // Make sure the function is unique
                auto prev = Lens->functions.find(id->identifier->word);
                if (prev != Lens->functions.end() && prev->second->type == LIME_NODE_PROC_DEFINITION){
                    Error("Redefinition of procedure: " +id->identifier->word,id->token.line_number);
                }

                Lens->addProc(id);

                Lens->push();
                for (auto v:id->children[0]->children) {
                    if (v->type == LIME_NODE_VARIABLE_DECLARATION) {
                        Lens->addVar(static_cast<IdentifierNode*>(v));
                    }
                }
                AstPass(node->children[1]);
                Lens->push();

                break;
            }

            case LIME_NODE_PROC_CALL: {
                auto id = static_cast<ProcCallNode*>(node);
                assert(id->identifier); 
                
                //TODO: Test that the arguments match the functions prototype
                if (!Lens->procExists(id->identifier->word)){
                    Error("Undefined procedure: " +id->identifier->word,id->identifier->line_number);
                }

                //auto proc = Lens->getProc(id->identifier->word);
                //if (!proc->matches(id)){
                //    Error("Arguments do not match the parameters for the function: " + id->identifier->word, id->identifier->line_number);
                //}

                break;
            }

            default:
                break;
        }
        prev = node;
    }

    return result;
}

void LinkProcCallsToPrototypes(Node* ast){
    for (const auto& node : ast->children) {
        switch(node->type){
            case LIME_NODE_PROC_CALL:{
                auto proc_call = static_cast<ProcCallNode*>(node);
                auto proc = (ProcPrototypeNode*)Lens->getProc(proc_call->identifier->word);

                // Get the types of each argument in the list.
				auto arg_list = proc_call->children[0];
				for (auto arg : arg_list->children) {
					switch (arg->type) {
					case LIME_NODE_EXPRESSION: {
						// Get the type of the expression

						break;
					}
					default: break;
					}
				}

                proc_call->proc = proc;
                break;
            }
            default:{
                if (node->children.size() > 0){
                    for(auto n : node->children){
                        LinkProcCallsToPrototypes(n);
                    }
                }
                break;
            }
        }
    }
}

Node* create_ast_from_tokens(std::vector<Token>& tokens) {
    Lens = new CodeLens();

    auto ast = new Node(); 

    code_block_to_ast(ast, tokens);

    //TODO: Remove this hack, this is just a hack so that it wont give us an
    // error when we call the print function
    //Lens->functions.insert(std::make_pair("printf", new ProcPrototypeNode()));
    //Lens->functions.insert(std::make_pair("getchar", new Node()));

    Lens->push();
    AstPass(ast);
    Lens->pop();

    LinkProcCallsToPrototypes(ast);

    return ast;
}