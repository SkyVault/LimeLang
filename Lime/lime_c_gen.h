#pragma once

#include <string>
#include <sstream>
#include <fstream>

#include "ast.h"
#include "lexer.h"

const std::string TypeDefs = R"TYPEDEFS(
#define none NULL
typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned int    u32;
typedef unsigned long   u64;

typedef int             i8;
typedef int             i16;
typedef int             i32;
typedef int             i64;

typedef float           f32;
typedef double          f64;
//typedef long            int;
typedef unsigned int    uint;
typedef float           real;
)TYPEDEFS";

struct LimeCGen {
    std::string compile_code_block(Node* node, const std::string indent = "");
    std::string compile_expression(Node* node);

    void compile_ast_to_c(Node* node, const std::string& out_file);

private:
    std::stringstream code, gfdef, gfprot, gvd, header;
    int scope{0};
};
