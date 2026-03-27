#pragma once
#include "cli/cli.h"
#include "core/transaction_service.h"
#include "core/animal_service.h"
#include "core/employee_service.h"
#include "core/hr_service.h"
#include "core/models/hr.h"
#include <string>
#include <optional>

class AppState : public StateComponent {
public:
    explicit AppState(const std::string& db_path = "./farm.db");
    ~AppState();

    TransactionService* getTransactionService();
    AnimalService*      getAnimalService();
    EmployeeService*    getEmployeeService();
    HRService*          getHRService();

    const std::string& getDbPath() const { return _db_path; }

    // ── Session ───────────────────────────────────────────────────────────────
    void                     setSession(i64 hr_id, HRRole role);
    bool                     hasSession()  const;
    i64                      getSessionId()   const;
    HRRole                   getSessionRole() const;
    bool                     isRole(HRRole r) const;

private:
    void ensureTransactionService();
    void ensureAnimalService();
    void ensureEmployeeService();
    void ensureHRService();

    std::string         _db_path;
    TransactionService* _tx_service;
    AnimalService*      _animal_service;
    EmployeeService*    _employee_service;
    HRService*          _hr_service;

    // Session — set at startup via farmos.cpp login prompt
    bool                _has_session = false;
    i64                 _session_hr_id   = 0;
    HRRole              _session_role    = HRRole::MANAGER;
};

AppState* getAppState(const Args& args);
