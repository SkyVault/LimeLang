#include "lime.h"

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
