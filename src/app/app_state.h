#pragma once

#include "cli/cli.h"
#include "core/transaction_service.h"
#include "core/animal_service.h"
#include "core/inventory_service.h"
#include <string>

class AppState : public StateComponent {
public:
    explicit AppState(const std::string& db_path = "./farm.db");
    ~AppState();

    TransactionService* getTransactionService();
    AnimalService*      getAnimalService();
    InventoryService*   getInventoryService();

    const std::string& getDbPath() const { return _db_path; }

private:
    void ensureTransactionService();
    void ensureAnimalService();
    void ensureInventoryService();

    std::string         _db_path;
    TransactionService* _tx_service;
    AnimalService*      _animal_service;
    InventoryService*   _inv_service;
};

AppState* getAppState(const Args& args);
