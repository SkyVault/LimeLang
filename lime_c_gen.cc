#include "lime_c_gen.h"
#include "lime.h"
#include "lime_types.h"

std::string LimeCGen::compile_expression(Node* node) {

    std::stringstream ss;

    Node* e = node;
    if (node->type == LIME_NODE_EXPRESSION)
        e = node->children[0];

    switch(e->type) {

        case LIME_NODE_NUMBER_LITERAL:
            ss << e->token.word;
            break;

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

                ss << indent << n->token.word << std::endl;

                break;
            }

            case LIME_NODE_PROC_DEFINITION: {

                

                break;
            }

            case LIME_NODE_VARIABlE_ASSIGNMENT: {
                if (n->identifier != nullptr) {

                    if (scope == 1) {
                        // Handle the global declaration
                        gvd << n->variable_type->word << " " << n->identifier->word << ";\n";

                        if (n->children.size() > 0) {
                            ss << indent << n->identifier->word << " = ";
                            ss << compile_expression(n->children[0]) << ";\n";
                        }
                    } else {
                        // Handle local declaration
                        ss << n->variable_type->word << " " << n->identifier->word << " = ";
                        ss << compile_expression(n->children[0]) << ";\n";
                    }
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
    header << "#include <stdio.h>";
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
