#pragma once 
#include "color.h"
#include <iostream>

using namespace asc;

namespace wx{
template<typename T>
T lineInput(const std::string& prompt)
{
    T value;
    std::cout<<GREEN<< prompt << RESET;
    std::cin >> value;
    std::cin.ignore();
    return value;
}
template<>
std::string lineInput<std::string>(const std::string& prompt)
{
    std::string value;
    std::cout<<GREEN<< prompt << RESET;
    std::getline(std::cin,value);
    return value;
}

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
            << BLUE
            <<"Enter your choice (1-"<<length<<"): "
            << RESET;
        std::cin>>str;
        std::cin.ignore();
        try {
            int choice = std::stoi(str);

            if(choice >= 1 && choice <= length){
                return convert(options[choice-1]);
            }
        } 
        catch (...) {}

        printError("Invalid Choice. Try again.");
    }
}
}