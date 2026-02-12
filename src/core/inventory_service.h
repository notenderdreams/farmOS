#pragma once

#include "core/database.h"
#include "core/models/inventory.h"
#include <vector>

class InventoryService : public Database
{
public:
    InventoryService(sqlite3* db);
    InventoryService(const std::string& db_path);

    void initTable();

    void addItem(const Inventory& item);
    std::vector<Inventory> getAllItems();
    Inventory getItemById(i64 item_id);

    void updateQuantity(i64 item_id, f64 new_quantity);
    void deleteItem(i64 item_id);

private:
    InventoryStatus computeStatus(f64 quantity, f64 reorder_level);
};
