#pragma once

#include <string>
#include <map>

enum LimeTypes {
    Unknown,

    None,

    Uint8,
    Uint16,
    Uint32,
    Uint64,

    Int8,
    Int16,
    Int32,
    Int64,

    Float,
    Float32,

    String,
    Boolean,

    Num_Types
};

static std::map<std::string, LimeTypes> LimeTypeStringMap = {
    {"none",    None},
    {"u8",      Uint8},
    {"u16",     Uint16},
    {"u32",     Uint32},
    {"u64",     Uint64},
    {"i8",      Int8},
    {"i16",     Int16},
    {"i32",     Int32},
    {"i64",     Int64},
    {"f32",     Float32},
    {"f64",     Float},
    {"int",     Int64},
    {"uint",    Uint64},
    {"float",   Float},
    {"real",    Float},
};

static std::map<LimeTypes, std::string> LimeStringTypeMap = {
    {None,      "none"},
    {Uint8,     "u8"},
    {Uint16,    "u16"},
    {Uint32,    "u32"},
    {Uint64,    "u64"},
    {Int8,      "i8"},
    {Int16,     "i16"},
    {Int32,     "i32"},
    {Int64,     "i64"},
    {Float32,   "f32"},
    {Float,     "f64"},
    {Int64,     "int"},
    {Uint64,    "uint"},
    {Float,     "float"},
    {Float,     "real"},
};

struct TypeDesc { 
    bool isCustomType{false}; 
    bool isPointer{false};
};

struct LimeTypeDesc: TypeDesc {
    LimeTypeDesc(LimeTypes _type): type(_type) {isCustomType = false;}
    LimeTypes type{None};
};

struct LimeCustomTypeDesc: TypeDesc {
    LimeCustomTypeDesc(const std::string& _name): name(_name) {isCustomType = true;}
    std::string name{""};
};

bool isType(const std::string& str);
