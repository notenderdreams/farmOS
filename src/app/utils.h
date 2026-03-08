#pragma once
#include <iostream>
#include <string>
#include <chrono>
#include <format>

inline std::string getCurrentDate(){
    auto now = std::chrono::system_clock::now();
    auto tt_now = std::chrono::system_clock::to_time_t(now);
    std::tm* tm_now = std::localtime(&tt_now);

    char buff[11];
    std::strftime(buff,sizeof(buff),"%Y-%m-%d", tm_now);
    return std::string(buff);
}

inline void printTransaction(const Transaction& tx, bool show_separator = true) {
    std::cout << color::GREEN << "ID: " << color::RESET << tx.tid << "\n";
    std::cout << "\tType: " << tx::toStr(tx.type) << "\n";
    std::cout << "\tDirection: " << tx::toStr(tx.direction) << "\n";
    std::cout << "\tAmount: $" << tx.amount << "\n";
    std::cout << "\tEntity: " << tx::toStr(tx.entity_type) << " (" << tx.entity_id << ")\n";
    std::cout << "\tDescription: " << tx.description << "\n";
    std::cout << "\tDate: " << tx.date << "\n";
    std::cout << "\tStatus: " << tx::toStr(tx.status) << "\n";
    
    if (show_separator) {
        color::printSeperator();
    }
}
