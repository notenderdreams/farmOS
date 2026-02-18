#pragma once
#include <string>
#include <stdexcept>
#include "core/types.h"

enum class InventoryCategory
{
    MEDICINE,
    FEED,
    EQUIPMENT,
    MILK,
    EGG,
    OTHER
};

enum class InventoryUnit
{
    KG,
    LITER,
    PIECE,
    TON,
    PACK
};

enum class InventoryStatus
{
    AVAILABLE,
    LOW_STOCK,
    OUT_OF_STOCK
};

struct Inventory
{
    i64 item_id;

    std::string name;
    InventoryCategory category;

    f64 quantity;
    InventoryUnit unit;

    f64 reorder_level = 0.0;

    std::string created_at;
    std::string updated_at;

    InventoryStatus status = InventoryStatus::AVAILABLE;
};


namespace inv {

    extern const char *InventoryCategoryStrs[];
    extern const char *InventoryUnitStrs[];
    extern const char *InventoryStatusStrs[];

    static const char* toStr(InventoryCategory c)
    {
        return InventoryCategoryStrs[static_cast<int>(c)];
    }

    static const char* toStr(InventoryUnit u)
    {
        return InventoryUnitStrs[static_cast<int>(u)];
    }

    static const char* toStr(InventoryStatus s)
    {
        return InventoryStatusStrs[static_cast<int>(s)];
    }

    inline InventoryCategory stc(const std::string& s)
    {
        if (s == "MEDICINE") return InventoryCategory::MEDICINE;
        if (s == "FEED") return InventoryCategory::FEED;
        if (s == "EQUIPMENT") return InventoryCategory::EQUIPMENT;
        if (s == "MILK") return InventoryCategory::MILK;
        if (s == "EGG") return InventoryCategory::EGG;
        if (s == "OTHER") return InventoryCategory::OTHER;
        throw std::runtime_error("Invalid inventory category string: " + s);
    }

    inline InventoryUnit stu(const std::string& s)
    {
        if (s == "KG") return InventoryUnit::KG;
        if (s == "LITER") return InventoryUnit::LITER;
        if (s == "PIECE") return InventoryUnit::PIECE;
        if (s == "TON") return InventoryUnit::TON;
        if (s == "PACK") return InventoryUnit::PACK;
        throw std::runtime_error("Invalid inventory unit string: " + s);
    }

    inline InventoryStatus sts(const std::string& s)
    {
        if (s == "AVAILABLE") return InventoryStatus::AVAILABLE;
        if (s == "LOW_STOCK") return InventoryStatus::LOW_STOCK;
        if (s == "OUT_OF_STOCK") return InventoryStatus::OUT_OF_STOCK;
        throw std::runtime_error("Invalid inventory status string: " + s);
    }

    inline InventoryStatus computeStatus(f64 quantity, f64 reorder_level)
    {
        if (quantity <= 0)
            return InventoryStatus::OUT_OF_STOCK;

        if (quantity <= reorder_level)
            return InventoryStatus::LOW_STOCK;

        return InventoryStatus::AVAILABLE;
    }

}
