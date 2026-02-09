#include "color.h"
#include <iostream>

namespace asc
{
    const char* RESET  = "\033[0m";
    const char* BLUE   = "\033[94m";
    const char* GREEN  = "\033[92m";
    const char* GREY   = "\033[2m";
    const char* RED    = "\033[91m";
    const char* YELLOW = "\033[93m";

    void printError(const std::string& message)
    {
        std::cerr << RED << "Error: " << RESET << message << "\n";
    }

    void printSeperator()
    {
        std::cout << GREY << "---------------------------------\n" << RESET;
    }
}
