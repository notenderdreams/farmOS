#include "color.h"
#include <iostream>

namespace color
{
    // Reset
    const char* RESET  = "\033[0m";
    // Styles
    const char* BOLD      = "\033[1m";
    const char* DIM       = "\033[2m";
    const char* ITALIC    = "\033[3m";
    const char* UNDERLINE = "\033[4m";
    // Colors
    const char* BLUE   = "\033[94m";
    const char* GREEN  = "\033[92m";
    const char* GREY   = "\033[2m";
    const char* RED    = "\033[91m";
    const char* YELLOW = "\033[93m";
    const char* TEAL = "\033[38;2;0;128;128m";

    void printError(const std::string& message)
    {
        std::cerr << RED << "Error: " << RESET << message << "\n";
    }

    void printSeperator()
    {
        std::cout << GREY << "---------------------------------\n" << RESET;
    }
}
