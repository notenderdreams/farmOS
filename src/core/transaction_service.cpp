#include "core/transaction_service.h"
#include "models/schema.h"
#include <stdexcept>
#include <vector> 


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

std::vector<Transaction> TransactionService::getAllTransactions()
{
    const char* sql = "SELECT * FROM transactions;";
    sqlite3_stmt* stmt = nullptr;

    if(sqlite3_prepare_v2(
        db, sql, -1, &stmt, nullptr
    ) != SQLITE_OK){
        throw std::runtime_error("DB: Failed to prepare statement ");
    }

    std::vector<Transaction> transactions;

    while(sqlite3_step(stmt) == SQLITE_ROW){
        Transaction t;
        t.tid = sqlite3_column_int64(stmt, 0);
        t.type = tx::stt(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
        t.direction = tx::stdi(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
        t.amount = sqlite3_column_double(stmt, 3);  
        t.entity_type = tx::ste(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
        t.entity_id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        t.description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        t.date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        t.status = tx::sts(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8)));

        transactions.push_back(t);

    }
    sqlite3_finalize(stmt);
    return transactions;
}

Transaction TransactionService::getTransactionById(i64 tid)
{
    const char* sql = "SELECT * FROM transactions WHERE t_id = ?;";
    sqlite3_stmt* stmt = nullptr;

    if(sqlite3_prepare_v2(
        db, sql, -1, &stmt, nullptr
    ) != SQLITE_OK){
        throw std::runtime_error("DB: Failed to prepare statement ");
    }

    sqlite3_bind_int64(stmt, 1, tid);

    Transaction t;
    if(sqlite3_step(stmt) == SQLITE_ROW){
        t.tid = sqlite3_column_int64(stmt, 0);
        t.type = tx::stt(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
        t.direction = tx::stdi(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
        t.amount = sqlite3_column_double(stmt, 3);  
        t.entity_type = tx::ste(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
        t.entity_id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        t.description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        t.date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        t.status = tx::sts(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8)));
    } else {
        sqlite3_finalize(stmt);
        throw std::runtime_error("DB: No transaction found with the given ID");
    }
    sqlite3_finalize(stmt);
    return t;
}

void TransactionService::updateStatus(i64 tid, TransactionStatus new_status)
{
    const char* sql = "UPDATE transactions SET status = ? WHERE t_id = ?;";
    sqlite3_stmt* stmt = nullptr;

    if(sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK){
        throw std::runtime_error("DB: Failed to prepare statement ");
    }

    sqlite3_bind_text(stmt, 1, Transaction::toStr(new_status), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 2, tid);

    if(sqlite3_step(stmt) != SQLITE_DONE){
        sqlite3_finalize(stmt);
        throw std::runtime_error("DB: Failed to update transaction status");
    }

    if(sqlite3_changes(db) == 0){
        sqlite3_finalize(stmt);
        throw std::runtime_error("DB: No transaction found with the given ID");
    }

    sqlite3_finalize(stmt);
}
