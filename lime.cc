#include "lime.h"
#include <sstream>

void Error(std::string message, int line_number = 1) {
//#ifdef COLORED_OUTPUT
//#ifdef unix
    std::stringstream ss;
    ss << "Lime::Error::line(" << line_number << "):: ";
    ss << message << std::endl;
    std::cout << "\033[1;91m" << ss.str() << "\033[0m" << std::endl;

//endif
#if 0
    std::cout << "Lime::Error::line(" << line_number << "):: " << message << std::endl;
#endif

    std::cin.get();
    exit(-1);
}

std::string gen_uuid(int num) {
    std::stringstream ss;

    static std::vector<std::string> uuid_strings;
    
    while(std::find(uuid_strings.begin(), uuid_strings.end(), ss.str()) != uuid_strings.end() || ss.str() == "") {
        for (int i = 0; i < num; i++)
            ss << char_set[rand() % char_set.size()];
    }

    uuid_strings.push_back(ss.str());

    return ss.str();
}
