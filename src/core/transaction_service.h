#pragma once 

#include "models/transaction.h"
#include <sqlite3.h>
#include <vector>
#include "core/types.h"

class TransactionService 
{
public:
    explicit TransactionService(sqlite3* db) ;
    
    void initTable() ;
    void addTransaction(const Transaction& tx) ;
    std::vector<Transaction> getAllTransactions() ;
    Transaction getTransactionById(i64 tid) ;
    
private:
    sqlite3* db;
};
