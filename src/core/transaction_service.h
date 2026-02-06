#pragma once 

#include "models/transaction.h"
#include <sqlite3.h>
#include <vector>
#include "core/types.h"
#include "core/database.h"

class TransactionService : public Database
{
public:
    TransactionService(sqlite3* db) ;
    TransactionService(const std::string& db_path) ;
    
    void initTable() ;
    void addTransaction(const Transaction& tx) ;
    std::vector<Transaction> getAllTransactions() ;
    Transaction getTransactionById(i64 tid) ;
    void updateStatus(i64 tid, TransactionStatus new_status) ;
};
