/*
 *   This header file contains necessary ascii
 *   sequences for colored printing and some
 *   printing utilities.
 */


#pragma once
#include <string>

namespace color
{
    // Usage:     std::cout << BLUE << "FarmOS" << RESET;
    // Reset
    extern const char* RESET;
    // Styles
    extern const char* BOLD;
    extern const char* DIM;
    extern const char* ITALIC;
    extern const char* UNDERLINE;
    // Colors
    extern const char* BLUE;
    extern const char* GREEN;
    extern const char* GREY;
    extern const char* RED;
    extern const char* YELLOW;
    extern const char* TEAL;

    void printError(const std::string& message);
    void printSeperator();
}
