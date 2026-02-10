#pragma once
#include <string>
#include <chrono>
#include <format>

std::string getCurrentDate(){
    auto now = std::chrono::system_clock::now();
    auto tt_now = std::chrono::system_clock::to_time_t(now);
    std::tm* tm_now = std::localtime(&tt_now);

    char buff[11];
    std::strftime(buff,sizeof(buff),"%Y-%m-%d", tm_now);
    return std::string(buff);
}

inline void printTransaction(const Transaction& tx, bool show_separator = true) {
    std::cout << asc::GREEN << "ID: " << asc::RESET << tx.tid << "\n";
    std::cout << "  Type: " << tx::toStr(tx.type) << "\n";
    std::cout << "  Direction: " << tx::toStr(tx.direction) << "\n";
    std::cout << "  Amount: $" << tx.amount << "\n";
    std::cout << "  Entity: " << tx::toStr(tx.entity_type) << " (" << tx.entity_id << ")\n";
    std::cout << "  Description: " << tx.description << "\n";
    std::cout << "  Date: " << tx.date << "\n";
    std::cout << "  Status: " << tx::toStr(tx.status) << "\n";
    
    if (show_separator) {
        asc::printSeperator();
    }
}