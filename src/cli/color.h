#pragma once
#include <string>

namespace asc
{
    extern const char* RESET;
    extern const char* BLUE;
    extern const char* GREEN;
    extern const char* GREY;
    extern const char* RED;
    extern const char* YELLOW;

    void printError(const std::string& message);
    void printSeperator();
}
