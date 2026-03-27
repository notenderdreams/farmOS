#include "app_state.h"
#include "cli/color.h"
#include <iostream>

AppState::AppState(const std::string& db_path)
    : _db_path(db_path),
      _db(nullptr),
      _tx_service(nullptr),
      _animal_service(nullptr),
      _inv_service(nullptr) {
    try {
        auto conn = Database::getConn(_db_path);
        _db = conn;
        _tx_service = new TransactionService(conn);
        _animal_service = new AnimalService(conn);
        _inv_service = new InventoryService(conn);

        _tx_service->initTable();
        _animal_service->initTable();
        _inv_service->initTable();
    } catch (const std::exception& e) {
        color::printError(std::string("Failed to initialize services: ") + e.what());
    }
}

AppState::~AppState() {
    delete _tx_service;
    delete _animal_service;
    delete _inv_service;
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
