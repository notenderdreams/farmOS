#pragma once 

#include <sqlite3.h>
#include <string>


class Database {
public:
    Database(const std::string& path);
    Database(sqlite3* db);
    virtual ~Database();

    bool execute(const std::string& sql);
    sqlite3_stmt* prepare(const std::string& sql);

protected:
    sqlite3* db;
};