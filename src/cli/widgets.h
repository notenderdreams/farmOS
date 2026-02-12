/*
 *  This header file contains widgets implementations
 *  - lineInput
 *  - selectInput
 */


#pragma once
#include "color.h"
#include <iostream>

namespace wx{
/*
 * For single number or value;
 * for std::string it will read the full line (spaces included)
 * Arguments:
 *      std::string prompt;
 *
 * usage :
 *       double price = wx::lineInput<double>("Price: ");
 *       std::string name = wx::lineInput<std::string>("Name: ");
 */
template<typename T>
T lineInput(const std::string& prompt)
{
    T value;
    std::cout<<color::GREEN<< prompt << color::RESET;
    std::cin >> value;
    std::cin.ignore();
    return value;
}
template<>
std::string lineInput<std::string>(const std::string& prompt)
{
    std::string value;
    std::cout<<color::GREEN<< prompt << color::RESET;
    std::getline(std::cin,value);
    return value;
}

/*
 * Used to take enum inputs.
 * Arguments:
 *      - std::string prompt;
 *      - array of string as the option list
 *      - length of the array
 *      - a converter function that will take the string
 *        and returns corresponding enum.
 *
 * Usage:
 *      enum class TType { BUY, SELL };
 *      char* array_of_strings[] = {"BUY", "SELL"};
 *      int len = 2;
 *
 *      TType converter (string& s) {
 *          if (s == "BUY")    return TType::BUY;
 *          if (s == "SELL")   return TType::SELL;
 *          throw std::runtime_error("Invalid type: " + s);
 *      }
 *
 *      string prompt = "Transaction Type:";
 *
 *      E t = wx::selectInput<TType>(
 *          prompt,
 *          array_of_strings,
 *          len,
 *          converter
 *      );
 */
template<typename E>
E selectInput(
    const std::string& prompt,
    const char* options[],
    int length,
    E(*convert)(const std::string&)
){
    std::cout<< prompt <<std::endl;
    for(int i=0;i<length;++i){
        std::cout<<i + 1<<". "<<options[i]<<std::endl;
    }

    std::string str;
    while(true){
        std::cout
            << color::BLUE
            <<"Enter your choice (1-"<<length<<"): "
            << color::RESET;
        std::cin>>str;
        std::cin.ignore();
        try {
            int choice = std::stoi(str);

            if(choice >= 1 && choice <= length){
                return convert(options[choice-1]);
            }
        }
        catch (...) {}

        color::printError("Invalid Choice. Try again.");
    }
}
}
