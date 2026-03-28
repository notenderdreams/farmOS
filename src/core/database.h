#pragma once 

#include <sqlite3.h>
#include <string>


class Database {
public:
    Database(const std::string& path);
    Database(sqlite3* db);
    virtual ~Database();

    static sqlite3* getConn(const std::string& path);

    bool execute(const std::string& sql);
    sqlite3_stmt* prepare(const std::string& sql);
    virtual void initTable() = 0;


protected:
    sqlite3* db;
};