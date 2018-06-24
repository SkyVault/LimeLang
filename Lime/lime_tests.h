#include "lexer.h"
#include "lime_utils.h"

#include <iostream>
#include <functional>
#include <string>
#include <tuple>

std::tuple<bool, std::string> isNumberTests() {
    using namespace std;
    if (!isNumber("-3.14159")) return {false, "-3.14159"};
    if (isNumber("hello")) return {false, "hello"};
    if (isNumber("43d.123.d")) return {false, "43d.123.d"};
    if (isNumber("1231-12312")) return {false, "1231-12312"};
    if (isNumber("-")) return {false, "-"};
    if (isNumber(".")) return {false, "."};

    return make_tuple(true, "");
}

bool runTest(std::function<std::tuple<bool, std::string>()> fn, const std::string& name){
    auto [result, which] = fn();
    std::cout << "Running Test: " << name << " :: Status [" << (result ? "SUCCESS]" : ("FAILED] -> Input: " + which)) << "\n";
    return result;
}

bool limeRunTests() {
    if (!runTest(isNumberTests, "isNumberTests")) return false;
    
    return true;
}
