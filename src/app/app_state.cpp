#include "app_state.h"
#include "cli/color.h"
#include <iostream>

AppState::AppState(const std::string& db_path)
    : _db_path(db_path),
      _tx_service(nullptr),
      _animal_service(nullptr),
      _inv_service(nullptr) {}

AppState::~AppState() {
    delete _tx_service;
    delete _animal_service;
    delete _inv_service;
}


TransactionService* AppState::getTransactionService() {
    ensureTransactionService();
    return _tx_service;
}

void AppState::ensureTransactionService() {
    if (!_tx_service) {
        try {
            _tx_service = new TransactionService(_db_path);
            _tx_service->initTable();
        } catch (const std::exception& e) {
            color::printError(std::string("Failed to initialize transaction service: ") + e.what());
        }
    }
}


AnimalService* AppState::getAnimalService() {
    ensureAnimalService();
    return _animal_service;
}

void AppState::ensureAnimalService() {
    if (!_animal_service) {
        try {
            _animal_service = new AnimalService(_db_path);
            _animal_service->initTable();
        } catch (const std::exception& e) {
            color::printError(std::string("Failed to initialize animal service: ") + e.what());
        }
    }
}

InventoryService* AppState::getInventoryService() {
    ensureInventoryService();
    return _inv_service;
}

void AppState::ensureInventoryService() {
    if (!_inv_service) {
        try {
            _inv_service = new InventoryService(_db_path);
            _inv_service->initTable();
        } catch (const std::exception& e) {
            color::printError(std::string("Failed to initialize inventory service: ") + e.what());
        }
    }
}

InventoryService* AppState::getInventoryService() {
    ensureInventoryService();
    return _inv_service;
}

void AppState::ensureInventoryService() {
    if (!_inv_service) {
        try {
            _inv_service = new InventoryService(_db_path);
            _inv_service->initTable();
        } catch (const std::exception& e) {
            color::printError(std::string("Failed to initialize inventory service: ") + e.what());
        }
    }
}


AppState* getAppState(const Args& args) {
    if (!args.cli) {
        color::printError("CLI context not available");
        return nullptr;
    }

    auto* state = args.cli->getState<AppState>();
    if (!state) {
        color::printError("AppState not registered");
        return nullptr;
    }
    return state;
}
