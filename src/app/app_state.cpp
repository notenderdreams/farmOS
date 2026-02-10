#include "app_state.h"
#include "cli/color.h"
#include <iostream>

AppState::AppState(const std::string& db_path) 
    : _db_path(db_path), _tx_service(nullptr) {
}

AppState::~AppState() {
    if (_tx_service) {
        delete _tx_service;
    }
}

TransactionService* AppState::getTransactionService() {
    ensureDatabase();
    return _tx_service;
}

void AppState::ensureDatabase() {
    if (!_tx_service) {
        try {
            _tx_service = new TransactionService(_db_path);
            _tx_service->initTable();
        } catch (const std::exception& e) {
            asc::printError(std::string("Failed to initialize database: ") + e.what());
        }
    }
}

AppState* getAppState(const Args& args) {
    if (!args.cli) {
        asc::printError("CLI context not available");
        return nullptr;
    }
    
    auto* state = args.cli->getState<AppState>();
    if (!state) {
        asc::printError("AppState not registered");
        return nullptr;
    }
    return state;
}


