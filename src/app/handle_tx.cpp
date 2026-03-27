#include "handle_tx.h"
#include "cli/widgets.h"
#include "core/models/transaction.h"
#include "core/transaction_service.h"
#include "cli/cli.h"
#include "app_state.h"
#include "utils.h"
#include <iostream>

int txAdd(const Args& args) {
    auto* app_state = getAppState(args);
    if (!app_state) return 1;
    
    auto* tx_service = app_state->getTransactionService();
    if (!tx_service) {
        color::printError("Failed to initialize transaction service");
        return 1;
    }
    
    Transaction tx;
    
    tx.type = wx::selectInput<TransactionType>(
        "Transaction type:",
        tx::TransactionTypeStrs, 4,
        tx::stt
    );
    
    tx.direction = tx::typeToDir(tx.type);
    tx.amount = wx::lineInput<double>("Amount: ");
    tx.entity_type = wx::selectInput<TransactionEntityType>(
        "Entity type:",
        tx::TransactionEntityTypeStrs, 3,
        tx::ste
    );
    
    tx.entity_id = wx::lineInput<std::string>("Entity ID: ");
    tx.description = wx::lineInput<std::string>("Description: ");
    
    tx.date = getCurrentDate();
    std::cout << color::GREY << "Date set to: " << tx.date << color::RESET << "\n";
    
    try {
        tx_service->addTransaction(tx);
        std::cout << color::GREEN << "Transaction added successfully" << color::RESET << "\n";
    } catch (const std::exception& e) {
        color::printError(std::string("Failed to add transaction: ") + e.what());
        return 1;
    }

    return 0;
}

int txList(const Args& args) {
    auto* app_state = getAppState(args);
    if (!app_state) return 1;
    
    auto* tx_service = app_state->getTransactionService();
    if (!tx_service) {
        color::printError("Failed to initialize transaction service");
        return 1;
    }
    
    try {
        auto transactions = tx_service->getAllTransactions();
        
        if (transactions.empty()) {
            std::cout << color::YELLOW << "No transactions found" << color::RESET << "\n";
            return 0;
        }
        
        std::cout << color::BLUE << "Transactions:" << color::RESET << "\n";
        color::printSeperator();
        
        for (const auto& tx : transactions) {
            printTransaction(tx);
        }
    } catch (const std::exception& e) {
        color::printError(std::string("Failed to list transactions: ") + e.what());
        return 1;
    }
    
    return 0;
}

int txShow(const Args& args) {
    auto* app_state = getAppState(args);
    if (!app_state) return 1;
    
    auto* tx_service = app_state->getTransactionService();
    if (!tx_service) {
        color::printError("Failed to initialize transaction service");
        return 1;
    }
    
    std::string tid_str;
    loadArg(tid_str, 0, "transaction_id");
    
    try {
        i64 tid = std::stoll(tid_str);
        auto tx = tx_service->getTransactionById(tid);
        
        std::cout << color::BLUE << "Transaction #" << tx.tid << color::RESET << "\n";
        color::printSeperator();
        printTransaction(tx, false);
        std::cout << "\tDate: " << tx.date << "\n";
        std::cout << "\tStatus: " << tx::toStr(tx.status) << "\n";
        color::printSeperator();
    } catch (const std::exception& e) {
        color::printError(std::string("Failed to show transaction: ") + e.what());
        return 1;
    }
    return 0;
}

int txUpdateStatus(const Args& args) {
    auto* app_state = getAppState(args);
    if (!app_state) return 1;

    auto* tx_service = app_state->getTransactionService();
    if (!tx_service) {
        color::printError("Failed to initialize transaction service");
        return 1;
    }

    std::string tid_str;
    loadArg(tid_str, 0, "transaction_id");
    
    try {
        i64 tid = std::stoll(tid_str);

        Transaction tx = tx_service->getTransactionById(tid);
        std::cout << "Current status for ID: "<<tid<<std::endl;
        // TODO : can't access toStr so hardcoding 
        switch (tx.status)
        {
        case TransactionStatus::PENDING :
            std::cout << color::YELLOW << "PENDING";
            break;
        case TransactionStatus::CANCELLED :
            std::cout << color::RED << "CANCELLED";
            break;
        case TransactionStatus::COMPLETED :
            std::cout << color::GREEN << "COMPLETED";
            break;
        }
        std::cout<< color::RESET << "\n";

        if (tx.status != TransactionStatus::PENDING) {
            return 0;
        }

        TransactionStatus new_status = wx::selectInput<TransactionStatus>(
            "Modify status:",
            tx::TransactionStatusStrs, 3,
            tx::sts
        );

        tx_service->updateStatus(tid, new_status);
        std::cout << color::GREEN << "Transaction status updated" << color::RESET << "\n";
    } catch (const std::exception& e) {
        color::printError(std::string("Failed to update transaction: ") + e.what());
        return 1;
    }
    
    return 0;
}