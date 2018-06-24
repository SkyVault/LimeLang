#include "lime_types.h"

bool isType(const std::string& str) {
    return LimeTypeStringMap.find(str) != LimeTypeStringMap.end();
}
