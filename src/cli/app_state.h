#pragma once

#include "cli.h"
#include "core/transaction_service.h"
#include <string>

class AppState : public StateComponent {
public:
    explicit AppState(const std::string& db_path = "./farm.db");
    ~AppState();
    
    TransactionService* getTransactionService();
    
    const std::string& getDbPath() const { return _db_path; }
    
    bool isInitialized() const { return _tx_service != nullptr; }

private:
    void ensureDatabase();
    
    std::string _db_path;
    TransactionService* _tx_service;
};

AppState* getAppState(const Args& args);