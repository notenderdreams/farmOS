#include "core/database.h"
#include <stdexcept>

Database::Database(const std::string& path) : db(nullptr)
{
    if (sqlite3_open(path.c_str(), &db) != SQLITE_OK) {
        throw std::runtime_error(
            "Failed to open database: " + 
            std::string(sqlite3_errmsg(db))
        );
    }
}

Database::Database(sqlite3* db) : db(db) {}

Database::~Database()
{
    if (db) sqlite3_close(db);
}

bool Database::execute(const std::string& sql)
{
    char* err = nullptr;
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &err) != SQLITE_OK) {
        std::string msg = err ? err : "Unknown error";
        sqlite3_free(err);
        throw std::runtime_error(msg);
    }
    return true;
}

sqlite3_stmt* Database::prepare(const std::string& sql)
{
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error(
            "DB: Failed to prepare statement: " + 
            std::string(sqlite3_errmsg(db))
        );
    }
    return stmt;
}