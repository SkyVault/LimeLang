#include "lime_c_gen.h"
#include "lime.h"
#include "lime_types.h"

std::string LimeCGen::compile_expression(Node* node) {
    std::stringstream ss;

    Node* e = node;
    while (e->type == LIME_NODE_EXPRESSION)
        e = e->children[0];

    switch(e->type) {
        case LIME_NODE_IDENTIFIER: {
            ss << e->token.word;
            break;
        }
        case LIME_NODE_NUMBER_LITERAL:
            ss << e->token.word;
            break;
        case LIME_NODE_STRING_LITERAL:
            ss << e->token.word;
            break;
        case LIME_NODE_CHARACTER_LITERAL:
            ss << e->token.word;
            break;
        case LIME_NODE_PROC_CALL: {

            ss << e->identifier->word << "(";
            if (e->children.size() > 0 && e->children[0]->type == LIME_NODE_ARGUMENT_LIST) {
                int i{0};
                for (const auto& a : e->children[0]->children) {
                    ss << compile_expression(a);
                    if (i < (int)(e->children[0]->children.size() - 1))
                        ss << ", ";
                    ++i;
                }
            }
            ss << ")";

            break;
        }

        case LIME_NODE_OPERATOR: {

            auto a = e->children[0];
            auto b = e->children[1];
            ss << compile_expression(a) << " " << e->token.word << " " << compile_expression(b);

            break;
        }

        default:
            break;
    }

    if (node->type == LIME_NODE_EXPRESSION)
        return "(" + ss.str() + ")";
    return ss.str();
}

std::string LimeCGen::compile_code_block(Node* node, const std::string indent) {
    scope++;
    std::stringstream ss;
    for (const auto n : node->children) {
        switch(n->type) {
            case LIME_NODE_EMIT: {
                ss << indent << n->token.word;
                break;
            }

            case LIME_NODE_C_INCLUDE: {
                header << "#include <" << n->token.word << ">\n";
                break;
            }

            case LIME_NODE_IF_STATEMENT: 
            case LIME_NODE_ELSEIF_STATEMENT: {
                auto expression = n->children[0];
                auto block = n->children[1];

                ss << indent << (n->type == LIME_NODE_IF_STATEMENT ? "if" : "else if");
                ss << " (" << compile_expression(expression) << ") {\n";
                ss << indent << compile_code_block(block, "    ") << "\n";
                ss << indent << "}\n";

                break;
            }

            case LIME_NODE_ELSE_STATEMENT: {
                ss << indent << "else {\n";
                ss << indent << compile_code_block(n->children[0], "    ") << "\n";
                ss << indent << "}\n";
                break;
            }

            case LIME_NODE_WHILE_LOOP: {
                // TODO: Stop using C's while loop and move to goto statements
                // This is less efficient because of optimizations that cant
                // happen, but this will force us to not rely on c

                auto expression = n->children[0];
                auto block = n->children[1];

                ss << indent << "while (" << compile_expression(expression) << ") {\n";
                ss << indent << compile_code_block(block, "    ") << "\n";
                ss << indent << "}\n";

                break;
            }

            case LIME_NODE_PROC_CALL: {

                ss << indent;
                ss << n->identifier->word << "(";
                if (n->children.size() > 0 && n->children[0]->type == LIME_NODE_ARGUMENT_LIST) {
                    int i{0};
                    for (const auto& a : n->children[0]->children) {
                        ss << compile_expression(a);
                        if (i < (int)(n->children[0]->children.size() - 1))
                            ss << ", ";
                        ++i;
                    }
                }
                ss << ");\n";

                break;
            }

            case LIME_NODE_RETURN: {
                if (n->children.size() > 0) {
                    ss << indent << "return " << compile_expression(n->children[0]) << ";";
                } else 
                    ss << indent << "return;";
                break;
            }

            case LIME_NODE_PROC_DEFINITION: {

                std::stringstream prot;
                prot << (n->variable_type == nullptr ? "void" : n->variable_type->word);
                prot << " " << n->identifier->word << "(";
                std::string block = "";
                for (auto child : n->children) {
                    if (child->type == 0) {
                        block = compile_code_block(child, "    ");
                    }
                    else if (child->type == LIME_NODE_PARAMETER_LIST) {
                        int i = 0;
                        for (const auto& param : child->children) {
                            prot << param->variable_type->word << " " << param->identifier->word;
                            if (i < (int)(child->children.size() - 1))
                                prot << ", ";
                            ++i;
                        }
                    }
                }
                prot << ")";
                gfprot << prot.str() << ";\n";

                gfdef << prot.str() << "{\n";
                gfdef << block << "\n";
                gfdef << "}\n";

                break;
            }

            case LIME_NODE_PROC_DECLARATION: {

                std::stringstream prot;
                prot << (n->variable_type == nullptr ? "void" : n->variable_type->word);
                prot << " " << n->identifier->word << "(";
                std::string block = "";
                for (auto child : n->children) {
                    if (child->type == 0) {
                        block = compile_code_block(child, "    ");
                    }
                    else if (child->type == LIME_NODE_PARAMETER_LIST) {
                        int i = 0;
                        for (const auto& param : child->children) {
                            prot << param->variable_type->word << " " << param->identifier->word;
                            if (i < (int)(child->children.size() - 1))
                                prot << ", ";
                            ++i;
                        }
                    }
                }
                prot << ")";
                gfprot << prot.str() << ";\n";

                break;
            }

            case LIME_NODE_ARRAY_INSERTION: {

                    // ss << indent;
                    // ss << n->identifier->word << " = ";
                    // ss << compile_expression(n->children[0]) << ";\n";

                ss << indent; 
                ss << "ARRAY_ADD(" << n->identifier->word << ", " << compile_expression(n->children[0]) << ");\n";
                
                break;
            }

            case LIME_NODE_VARIABLE_DECLARATION: {
                assert(n->variable_type != nullptr);

                if (scope == 1) {
					assert(n->variable_type != nullptr);
					assert(n->identifier != nullptr);
                    // Handle the global declaration
                    gvd << n->variable_type->word << " " << n->identifier->word << ";\n";
                } else {
                    // Handle local declaration
                    ss << n->variable_type->word << " " << n->identifier->word << ";\n";
                }

                // * Array declarations should always initialize

                if (n->isArray)
                    ss << indent << "ARRAY_INIT(" << n->identifier->word << ");\n";

                break;
            }

            case LIME_NODE_VARIABLE_ASSIGNMENT: {

                std::string op = "=";
                if (n->variable_type != nullptr) {
                    if (scope == 1) {
                        // Handle the global declaration
                        gvd << n->variable_type->word << " " << n->identifier->word << ";\n";

                        if (n->children.size() > 0) {
                            ss << indent << n->identifier->word << " " << op << " ";
                            ss << compile_expression(n->children[0]) << ";\n";
                        }
                    } else {
                        // Handle local declaration
                        ss << n->variable_type->word << " " << n->identifier->word << " = ";
                        ss << compile_expression(n->children[0]) << ";\n";
                    }
                } else {
                    switch (n->token.op){
                        case LIME_PLUS_EQUALS_OPERATOR:
                        case LIME_MINUS_EQUALS_OPERATOR:
                        case LIME_MULTIPLICATION_EQUALS_OPERATOR:
                        case LIME_DIVISION_EQUALS_OPERATOR:
                            op = n->token.word;
                            break;
                        default: break;
                    }

                    ss << indent;
                    ss << n->identifier->word << " " << op << " ";
                    ss << compile_expression(n->children[0]) << ";\n";
                }

                break;
            }

            default:
                std::cout << "Code gen does not handle:\n" << *n << std::endl;
                break;
        }
    }
    scope--;
    return ss.str();
}

void LimeCGen::compile_ast_to_c(Node* node, const std::string& out_file) {
    scope = 0;

    header << "// Lime Version: " << std::string{LIME_VERSION} << "\n";
    header << "#include <stdio.h>\n";
    header << "#include \"arrays.c\"";
    header << TypeDefs << "\n";

    const auto uuid = gen_uuid();
    code << "int main(int num_arg_" << uuid << ", char** args_" << uuid << ") {\n";

    code << compile_code_block(node, "    ");

    code << "    " << "return 0;\n";
    code << "}\n";
    
    std::stringstream result;
    result << header.str();
    result << gfprot.str();
    result << gfdef.str();
    result << gvd.str();
    result << code.str();

    std::cout << result.str() << std::endl;
    auto file = std::ofstream(out_file);
    file<<result.str() << std::endl;
    file.close();
}
