#include "inventory.h"

namespace inv {

    const char *InventoryCategoryStrs[] = {
        "MEDICINE",
        "FEED",
        "EQUIPMENT",
        "MILK",
        "EGG",
        "OTHER"
    };

    const char *InventoryUnitStrs[] = {
        "KG",
        "LITER",
        "PIECE",
        "TON",
        "PACK"
    };

    const char *InventoryStatusStrs[] = {
        "AVAILABLE",
        "LOW_STOCK",
        "OUT_OF_STOCK"
    };

}
