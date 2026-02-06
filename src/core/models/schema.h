#pragma once
#include <string_view>

namespace farmos::models{       

inline constexpr const std::string_view TRANSACTIONS_TABLE = R"(
    CREATE TABLE IF NOT EXISTS transactions (
        t_id INTEGER PRIMARY KEY AUTOINCREMENT,
        t_type TEXT NOT NULL,
        direction TEXT NOT NULL,
        amount REAL NOT NULL,
        entity_type TEXT,
        entity_id TEXT,
        description TEXT,
        date TEXT NOT NULL,
        status TEXT NOT NULL,
        created_at TEXT DEFAULT CURRENT_TIMESTAMP
    );
)";

} // namespace farmos::models

