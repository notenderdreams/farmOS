#include "handle_inventory.h"
#include "cli/widgets.h"
#include "cli/color.h"
#include "core/models/inventory.h"
#include "core/models/transaction.h"
#include "core/inventory_service.h"
#include "core/transaction_service.h"
#include "cli/cli.h"
#include "app_state.h"
#include "utils.h"
#include <iostream>


static void printInventoryItem(const Inventory& item, bool show_separator = true) {
    std::cout << color::GREEN << "ID: " << color::RESET << item.item_id << "\n";
    std::cout << "\tName:          " << item.name                      << "\n";
    std::cout << "\tCategory:      " << inv::toStr(item.category)      << "\n";
    std::cout << "\tQuantity:      " << item.quantity
              << " " << inv::toStr(item.unit)                          << "\n";
    std::cout << "\tReorder Level: " << item.reorder_level
              << " " << inv::toStr(item.unit)                          << "\n";


    switch (item.status) {
        case InventoryStatus::AVAILABLE:
            std::cout << "\tStatus:        "
                      << color::GREEN << "AVAILABLE" << color::RESET   << "\n";
            break;
        case InventoryStatus::LOW_STOCK:
            std::cout << "\tStatus:        "
                      << color::YELLOW << "LOW STOCK" << color::RESET  << "\n";
            break;
        case InventoryStatus::OUT_OF_STOCK:
            std::cout << "\tStatus:        "
                      << color::RED << "OUT OF STOCK" << color::RESET  << "\n";
            break;
    }

    if (show_separator)
        color::printSeperator();
}


static void logInventoryTransaction(
    TransactionService*   tx_service,
    TransactionType       type,
    double                amount,
    const std::string&    item_name,
    const std::string&    description)
{
    Transaction tx;
    tx.type        = type;
    tx.direction   = tx::typeToDir(type);
    tx.amount      = amount;
    tx.entity_type = TransactionEntityType::GOODS;
    tx.entity_id   = item_name;
    tx.description = description;
    tx.date        = getCurrentDate();
    tx.status      = TransactionStatus::COMPLETED;

    tx_service->addTransaction(tx);
}


int inventoryAdd(const Args& args) {
    auto* app_state = getAppState(args);
    if (!app_state) return 1;

    auto* inv_service = app_state->getInventoryService();
    if (!inv_service) {
        color::printError("Failed to initialize inventory service");
        return 1;
    }

    auto* tx_service = app_state->getTransactionService();

    Inventory item;

    item.name = wx::lineInput<std::string>("Name: ");

    item.category = wx::selectInput<InventoryCategory>(
        "Category:",
        inv::InventoryCategoryStrs, 6,
        inv::stc
    );

    item.quantity = wx::lineInput<double>("Quantity: ");

    item.unit = wx::selectInput<InventoryUnit>(
        "Unit:",
        inv::InventoryUnitStrs, 5,
        inv::stu
    );

    item.reorder_level = wx::lineInput<double>("Reorder level: ");

    try {
        inv_service->addItem(item);
        std::cout << color::GREEN << "Item added successfully" << color::RESET << "\n";
    } catch (const std::exception& e) {
        color::printError(std::string("Failed to add item: ") + e.what());
        return 1;
    }

    if (tx_service && item.quantity > 0) {
        double cost = wx::lineInput<double>("Purchase cost (0 to skip): ");
        if (cost > 0) {
            try {
                logInventoryTransaction(
                    tx_service,
                    TransactionType::BUY,
                    cost,
                    item.name,
                    "Initial stock purchase: " + item.name
                );
                std::cout << color::GREY
                          << "  BUY transaction logged"
                          << color::RESET << "\n";
            } catch (const std::exception& e) {
                color::printError(std::string("Failed to log transaction: ") + e.what());
            }
        }
    }

    return 0;
}

int inventoryList(const Args& args) {
    auto* app_state = getAppState(args);
    if (!app_state) return 1;

    auto* inv_service = app_state->getInventoryService();
    if (!inv_service) {
        color::printError("Failed to initialize inventory service");
        return 1;
    }

    try {
        auto items = inv_service->getAllItems();

        if (items.empty()) {
            std::cout << color::YELLOW << "No inventory items found" << color::RESET << "\n";
            return 0;
        }

        std::cout << color::BLUE << "Inventory:" << color::RESET << "\n";
        color::printSeperator();

        for (const auto& item : items)
            printInventoryItem(item);

    } catch (const std::exception& e) {
        color::printError(std::string("Failed to list inventory: ") + e.what());
        return 1;
    }

    return 0;
}

int inventoryShow(const Args& args) {
    auto* app_state = getAppState(args);
    if (!app_state) return 1;

    auto* inv_service = app_state->getInventoryService();
    if (!inv_service) {
        color::printError("Failed to initialize inventory service");
        return 1;
    }

    std::string id_str;
    loadArg(id_str, 0, "item_id");

    try {
        i64 iid = std::stoll(id_str);
        auto item = inv_service->getItemById(iid);

        std::cout << color::BLUE << "Item #" << item.item_id << color::RESET << "\n";
        color::printSeperator();
        printInventoryItem(item, false);
        std::cout << "\tAdded:    " << item.created_at << "\n";
        std::cout << "\tUpdated:  " << item.updated_at << "\n";
        color::printSeperator();

    } 
    catch (const std::exception& e) {
        color::printError(std::string("Failed to show item: ") + e.what());
        return 1;
    }

    return 0;
}

int inventoryUpdateQuantity(const Args& args) {
    auto* app_state = getAppState(args);
    if (!app_state) return 1;

    auto* inv_service = app_state->getInventoryService();
    if (!inv_service) {
        color::printError("Failed to initialize inventory service");
        return 1;
    }

    auto* tx_service = app_state->getTransactionService();

    std::string id_str;
    loadArg(id_str, 0, "item_id");

    try {
        i64 iid      = std::stoll(id_str);
        auto item    = inv_service->getItemById(iid);
        double old_q = item.quantity;

        std::cout << "Current quantity for '"
                  << color::GREEN << item.name << color::RESET
                  << "': " << old_q << " " << inv::toStr(item.unit) << "\n";

        double new_q = wx::lineInput<double>("New quantity: ");

        inv_service->updateQuantity(iid, new_q);
        std::cout << color::GREEN << "Quantity updated" << color::RESET << "\n";


        if (tx_service && new_q != old_q) {
            double diff = std::abs(new_q - old_q);
            double value = wx::lineInput<double>("Transaction value (0 to skip): ");

            if (value > 0) {
                if (new_q > old_q) {
                    logInventoryTransaction(
                        tx_service,
                        TransactionType::BUY,
                        value,
                        item.name,
                        "Restock: " + item.name
                            + " (+" + std::to_string(diff) + " "
                            + inv::toStr(item.unit) + ")"
                    );
                    std::cout << color::GREY
                              << "  BUY transaction logged"
                              << color::RESET << "\n";
                } 
                else {
                    logInventoryTransaction(
                        tx_service,
                        TransactionType::SELL,
                        value,
                        item.name,
                        "Stock reduction: " + item.name
                            + " (-" + std::to_string(diff) + " "
                            + inv::toStr(item.unit) + ")"
                    );
                    std::cout << color::GREY
                              << "  SELL transaction logged"
                              << color::RESET << "\n";
                }
            }
        }

    } 
    catch (const std::exception& e) {
        color::printError(std::string("Failed to update quantity: ") + e.what());
        return 1;
    }

    return 0;
}

int inventoryDelete(const Args& args) {
    auto* app_state = getAppState(args);
    if (!app_state) return 1;

    auto* inv_service = app_state->getInventoryService();
    if (!inv_service) {
        color::printError("Failed to initialize inventory service");
        return 1;
    }

    std::string id_str;
    loadArg(id_str, 0, "item_id");

    try {
        i64 iid = std::stoll(id_str);
        inv_service->deleteItem(iid);
        std::cout << color::GREEN << "Item deleted" << color::RESET << "\n";

    } 
    catch (const std::exception& e) {
        color::printError(std::string("Failed to delete item: ") + e.what());
        return 1;
    }

    return 0;
}
