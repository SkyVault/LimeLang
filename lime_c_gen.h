#pragma once

#include <string>
#include <fstream>
#include "ast.h"

struct LimeCGen {
    void compile_code_block(Node* node);
    void compile_ast_to_c(Node* node, const std::string& out_file);

private:
    std::string pre{""};
    std::string glob{""};
    std::string bod{""};
};
