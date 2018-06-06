#include "lime_utils.h"

void LimeAssert(bool expression, int line_number, LimeErrorLevel lvl, const std::string& msg) {
    //std::cout << "Lime 
}

bool isNumber(const std::string& word) {
    if (word.size() == 0) return false;

    auto it = word.begin();
    bool isDecimal{false};

    if (word.length() == 1)
        if (!std::isdigit(word[0])) return false;

    if (*it == '-') {
        ++it;
    }

    while(it != word.end()){
        if (*it == '.'){
            if (!isDecimal) {isDecimal = true; ++it; }
            else return false;    
        }
        if (!std::isdigit(*it)) return false;
        ++it;
    }
    return true;
}

