#pragma once

#include <string>
#include <cctype>
#include <iostream>

enum LimeErrorLevel{ 
    LimeError,
    LimeWarning,
    LimeNote
};

void LimeAssert(bool expression, int line_number, LimeErrorLevel lvl, const std::string& msg);
bool isNumber(const std::string& word);
