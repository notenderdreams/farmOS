#include "app_state.h"
#include "cli/color.h"
#include <iostream>

AppState::AppState(const std::string& db_path)
    : _db_path(db_path),
      _tx_service(nullptr),
      _animal_service(nullptr),
      _employee_service(nullptr),
      _hr_service(nullptr),
      _inv_service(nullptr) {}

AppState::~AppState() {
    delete _tx_service;
    delete _animal_service;
    delete _employee_service;
    delete _hr_service;
    delete _inv_service;
}

TransactionService* AppState::getTransactionService() {
    ensureTransactionService(); return _tx_service;
}
void AppState::ensureTransactionService() {
    if (!_tx_service) {
        try { _tx_service = new TransactionService(_db_path); _tx_service->initTable(); }
        catch (const std::exception& e) { color::printError(std::string("Transaction service: ") + e.what()); }
    }
}

AnimalService* AppState::getAnimalService() {
    ensureAnimalService(); return _animal_service;
}
void AppState::ensureAnimalService() {
    if (!_animal_service) {
        try { _animal_service = new AnimalService(_db_path); _animal_service->initTable(); }
        catch (const std::exception& e) { color::printError(std::string("Animal service: ") + e.what()); }
    }
}

InventoryService* AppState::getInventoryService() {
    ensureInventoryService(); return _inv_service;
}
void AppState::ensureInventoryService() {
    if (!_inv_service) {
        try { _inv_service = new InventoryService(_db_path); _inv_service->initTable(); }
        catch (const std::exception& e) { color::printError(std::string("Inventory service: ") + e.what()); }
    }
}

EmployeeService* AppState::getEmployeeService() {
    ensureEmployeeService(); return _employee_service;
}
void AppState::ensureEmployeeService() {
    if (!_employee_service) {
        try { _employee_service = new EmployeeService(_db_path); _employee_service->initTable(); }
        catch (const std::exception& e) { color::printError(std::string("Employee service: ") + e.what()); }
    }
}

HRService* AppState::getHRService() {
    ensureHRService(); return _hr_service;
}
void AppState::ensureHRService() {
    if (!_hr_service) {
        try { _hr_service = new HRService(_db_path); _hr_service->initTable(); }
        catch (const std::exception& e) { color::printError(std::string("HR service: ") + e.what()); }
    }
}

// ── Session ───────────────────────────────────────────────────────────────────

void AppState::setSession(i64 hr_id, HRRole role) {
    _session_hr_id = hr_id;
    _session_role  = role;
    _has_session   = true;
}

bool   AppState::hasSession()      const { return _has_session; }
i64    AppState::getSessionId()    const { return _session_hr_id; }
HRRole AppState::getSessionRole()  const { return _session_role; }
bool   AppState::isRole(HRRole r)  const { return _has_session && _session_role == r; }

// ── Helper ────────────────────────────────────────────────────────────────────

AppState* getAppState(const Args& args) {
    if (!args.cli) { color::printError("CLI context not available"); return nullptr; }
    auto* state = args.cli->getState<AppState>();
    if (!state)  { color::printError("AppState not registered");    return nullptr; }
    return state;
}