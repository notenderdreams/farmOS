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
    extern const char* RESET;
    extern const char* BLUE;
    extern const char* GREEN;
    extern const char* GREY;
    extern const char* RED;
    extern const char* YELLOW;

    void printError(const std::string& message);
    void printSeperator();
}
