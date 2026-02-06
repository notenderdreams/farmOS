#include "core/transaction_service.h"
#include "models/schema.h"
#include <stdexcept>


TransactionService::TransactionService(sqlite3* db) : db(db) {}

void TransactionService::initTable()
{
    char* err = nullptr;
    if(sqlite3_exec(
        db,std::string(farmos::models::TRANSACTIONS_TABLE).c_str(),
        nullptr,
        nullptr,
        &err
    )!= SQLITE_OK){
        std::string msg = err;
        sqlite3_free(err);
        throw std::runtime_error(msg);
    }
} 

void TransactionService::addTransaction(const Transaction& tx)
{
    const char* sql = R"(
        INSERT INTO transactions (
            t_type, direction, amount, entity_type, entity_id,
            description, date, status
        ) 
        VALUES (?, ?, ?, ?, ?, ?, ?, ?);
    )";

    sqlite3_stmt* stmt = nullptr;
    if(sqlite3_prepare_v2(
        db, sql, -1, &stmt, nullptr
    ) != SQLITE_OK){
        throw std::runtime_error("DB: Failed to prepare statement ");
    }

    sqlite3_bind_text(stmt, 1, Transaction::toStr(tx.type), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, Transaction::toStr(tx.direction), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 3, tx.amount);
    sqlite3_bind_text(stmt, 4, Transaction::toStr(tx.entity_type), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, tx.entity_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, tx.description.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 7, tx.date.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 8, Transaction::toStr(tx.status), -1, SQLITE_TRANSIENT);

    if(sqlite3_step(stmt) != SQLITE_DONE){
        sqlite3_finalize(stmt);
        throw std::runtime_error("DB: Failed ot insert the transaciton");
    }
    sqlite3_finalize(stmt);
}