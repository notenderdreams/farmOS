#pragma once 

#include "models/transaction.h"
#include <sqlite3.h>

class TransactionService 
{
    public:
    explicit TransactionService(sqlite3* db) ;
    
    void initTable() ;
    void addTransaction(const Transaction& tx) ;
    
    private:
    sqlite3* db;
};
