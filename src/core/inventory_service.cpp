#include "core/inventory_service.h"
#include "core/models/schema.h"
#include <stdexcept>

InventoryService::InventoryService(sqlite3* db)
    : Database(db) {}

InventoryService::InventoryService(const std::string& db_path)
    : Database(db_path) {}

void InventoryService::initTable()
{
    execute(std::string(farmos::models::INVENTORY_TABLE));
}

void InventoryService::addItem(const Inventory& item)
{
    const char* sql = R"(
        INSERT INTO inventory (
            name, category, quantity, unit, reorder_level
        )
        VALUES (?, ?, ?, ?, ?);
    )";

    sqlite3_stmt* stmt = prepare(sql);

    sqlite3_bind_text(stmt, 1, item.name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, inv::toStr(item.category), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 3, item.quantity);
    sqlite3_bind_text(stmt, 4, inv::toStr(item.unit), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 5, item.reorder_level);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        throw std::runtime_error("DB: Failed to insert inventory item");
    }

    sqlite3_finalize(stmt);
}

std::vector<Inventory> InventoryService::getAllItems()
{
    const char* sql = "SELECT * FROM inventory;";
    sqlite3_stmt* stmt = prepare(sql);

    std::vector<Inventory> items;

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        Inventory item;

        item.item_id = sqlite3_column_int64(stmt, 0);
        item.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        item.category = inv::stc(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
        item.quantity = sqlite3_column_double(stmt, 3);
        item.unit = inv::stu(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
        item.reorder_level = sqlite3_column_double(stmt, 5);
        item.created_at = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        item.updated_at = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));

        item.status = inv::computeStatus(item.quantity, item.reorder_level);

        items.push_back(item);
    }

    sqlite3_finalize(stmt);
    return items;
}

Inventory InventoryService::getItemById(i64 item_id)
{
    const char* sql = "SELECT * FROM inventory WHERE item_id = ?;";
    sqlite3_stmt* stmt = prepare(sql);

    sqlite3_bind_int64(stmt, 1, item_id);

    Inventory item;

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        item.item_id = sqlite3_column_int64(stmt, 0);
        item.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        item.category = inv::stc(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
        item.quantity = sqlite3_column_double(stmt, 3);
        item.unit = inv::stu(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
        item.reorder_level = sqlite3_column_double(stmt, 5);
        item.created_at = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        item.updated_at = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));

        item.status = inv::computeStatus(item.quantity, item.reorder_level);
    }
    else
    {
        sqlite3_finalize(stmt);
        throw std::runtime_error("DB: No inventory item found with given ID");
    }

    sqlite3_finalize(stmt);
    return item;
}

void InventoryService::updateQuantity(i64 item_id, f64 new_quantity)
{
    const char* sql = R"(
        UPDATE inventory
        SET quantity = ?, updated_at = CURRENT_TIMESTAMP
        WHERE item_id = ?;
    )";

    sqlite3_stmt* stmt = prepare(sql);

    sqlite3_bind_double(stmt, 1, new_quantity);
    sqlite3_bind_int64(stmt, 2, item_id);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        throw std::runtime_error("DB: Failed to update inventory quantity");
    }

    if (sqlite3_changes(db) == 0)
    {
        sqlite3_finalize(stmt);
        throw std::runtime_error("DB: No inventory item found with given ID");
    }

    sqlite3_finalize(stmt);
}

void InventoryService::deleteItem(i64 item_id)
{
    const char* sql = "DELETE FROM inventory WHERE item_id = ?;";
    sqlite3_stmt* stmt = prepare(sql);

    sqlite3_bind_int64(stmt, 1, item_id);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        throw std::runtime_error("DB: Failed to delete inventory item");
    }

    sqlite3_finalize(stmt);
}
