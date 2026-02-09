#pragma once 

namespace wx{
template<typename T>
T lineInput(const std::string& prompt)
{
    T value;
    std::cout<< prompt;
    std::cin >> value;
    std::cin.ignore();
    return value;
}
template<>
std::string lineInput<std::string>(const std::string& prompt)
{
    std::string value;
    std::cout<< prompt;
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
    std::cout<< prompt;
    for(int i=1;i<=length;++i){
        std::cout<<i<<". "<<options[i]<<std::endl;
    }

    int choice;
    // !BUG for now it runs infinitely if string is provied 
    while(true){
        std::cout<<"Enter your choice (1-"<<length<<"): ";
        std::cin>>choice;
        std::cin.ignore();

        if(choice >= 1 && choice <= length){
            return convert(options[choice-1]);
        }
        std::cout<<"Invalid choice. Please try again."<<std::endl;
    }
}
}