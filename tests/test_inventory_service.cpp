#include <gtest/gtest.h>
#include "core/inventory_service.h"
#include <sqlite3.h>
#include <memory>

class InventoryTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        ASSERT_EQ(sqlite3_open(":memory:", &db), SQLITE_OK);
        inv_service = std::make_unique<InventoryService>(db);
        inv_service->initTable();
    }

    void TearDown() override
    {
        inv_service.reset();
        sqlite3_close(db);
        db = nullptr;
    }

    sqlite3* db = nullptr;
    std::unique_ptr<InventoryService> inv_service;
};

TEST_F(InventoryTest, insertsInventoryItem)
{
    Inventory item{};
    item.name = "Cow Feed";
    item.category = InventoryCategory::FEED;
    item.quantity = 100.0;
    item.unit = InventoryUnit::KG;
    item.reorder_level = 20.0;

    EXPECT_NO_THROW(inv_service->addItem(item));

    sqlite3_stmt* stmt = nullptr;
    ASSERT_EQ(sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM inventory;", -1, &stmt, nullptr), SQLITE_OK);
    ASSERT_EQ(sqlite3_step(stmt), SQLITE_ROW);
    int count = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    EXPECT_EQ(count, 1);
}

TEST_F(InventoryTest, getAllItems)
{
    Inventory item1{};
    item1.name = "Antibiotics";
    item1.category = InventoryCategory::MEDICINE;
    item1.quantity = 50.0;
    item1.unit = InventoryUnit::PIECE;
    item1.reorder_level = 10.0;

    Inventory item2{};
    item2.name = "Chicken Feed";
    item2.category = InventoryCategory::FEED;
    item2.quantity = 200.0;
    item2.unit = InventoryUnit::KG;
    item2.reorder_level = 50.0;

    EXPECT_NO_THROW(inv_service->addItem(item1));
    EXPECT_NO_THROW(inv_service->addItem(item2));

    std::vector<Inventory> items;
    EXPECT_NO_THROW(items = inv_service->getAllItems());

    EXPECT_EQ(items.size(), 2);

    EXPECT_EQ(items[0].name, "Antibiotics");
    EXPECT_EQ(items[0].category, InventoryCategory::MEDICINE);
    EXPECT_EQ(items[0].quantity, 50.0);
    EXPECT_EQ(items[0].unit, InventoryUnit::PIECE);

    EXPECT_EQ(items[1].name, "Chicken Feed");
    EXPECT_EQ(items[1].category, InventoryCategory::FEED);
    EXPECT_EQ(items[1].quantity, 200.0);
    EXPECT_EQ(items[1].unit, InventoryUnit::KG);
}

TEST_F(InventoryTest, getItemById)
{
    Inventory item{};
    item.name = "Tractor Oil";
    item.category = InventoryCategory::EQUIPMENT;
    item.quantity = 15.0;
    item.unit = InventoryUnit::LITER;
    item.reorder_level = 5.0;

    EXPECT_NO_THROW(inv_service->addItem(item));

    std::vector<Inventory> allItems = inv_service->getAllItems();
    ASSERT_EQ(allItems.size(), 1);
    i64 itemId = allItems[0].item_id;

    Inventory fetchedItem;
    EXPECT_NO_THROW(fetchedItem = inv_service->getItemById(itemId));

    EXPECT_EQ(fetchedItem.item_id, itemId);
    EXPECT_EQ(fetchedItem.name, "Tractor Oil");
    EXPECT_EQ(fetchedItem.category, InventoryCategory::EQUIPMENT);
    EXPECT_EQ(fetchedItem.quantity, 15.0);
    EXPECT_EQ(fetchedItem.unit, InventoryUnit::LITER);
    EXPECT_EQ(fetchedItem.reorder_level, 5.0);
}

TEST_F(InventoryTest, updateQuantity)
{
    Inventory item{};
    item.name = "Milk Storage";
    item.category = InventoryCategory::MILK;
    item.quantity = 100.0;
    item.unit = InventoryUnit::LITER;
    item.reorder_level = 30.0;

    EXPECT_NO_THROW(inv_service->addItem(item));

    auto allItems = inv_service->getAllItems();
    ASSERT_EQ(allItems.size(), 1);
    i64 itemId = allItems[0].item_id;

    EXPECT_EQ(allItems[0].quantity, 100.0);

    // Update quantity to 250.0
    EXPECT_NO_THROW(inv_service->updateQuantity(itemId, 250.0));

    Inventory updatedItem = inv_service->getItemById(itemId);
    EXPECT_EQ(updatedItem.quantity, 250.0);
}

TEST_F(InventoryTest, deleteItem)
{
    Inventory item{};
    item.name = "Old Equipment";
    item.category = InventoryCategory::EQUIPMENT;
    item.quantity = 5.0;
    item.unit = InventoryUnit::PIECE;
    item.reorder_level = 0.0;

    EXPECT_NO_THROW(inv_service->addItem(item));

    auto allItems = inv_service->getAllItems();
    ASSERT_EQ(allItems.size(), 1);
    i64 itemId = allItems[0].item_id;

    // Delete the item
    EXPECT_NO_THROW(inv_service->deleteItem(itemId));

    // Verify it's deleted
    auto itemsAfterDelete = inv_service->getAllItems();
    EXPECT_EQ(itemsAfterDelete.size(), 0);
}

TEST_F(InventoryTest, getItemByIdInvalid)
{
    i64 invalidId = 999;
    EXPECT_THROW(inv_service->getItemById(invalidId), std::runtime_error);
}

TEST_F(InventoryTest, updateQuantityInvalidId)
{
    i64 invalidId = 999;
    EXPECT_THROW(inv_service->updateQuantity(invalidId, 50.0), std::runtime_error);
}

TEST_F(InventoryTest, statusComputationAvailable)
{
    Inventory item{};
    item.name = "Eggs";
    item.category = InventoryCategory::EGG;
    item.quantity = 100.0;  // Above reorder level
    item.unit = InventoryUnit::PIECE;
    item.reorder_level = 20.0;

    EXPECT_NO_THROW(inv_service->addItem(item));

    auto items = inv_service->getAllItems();
    ASSERT_EQ(items.size(), 1);
    
    // Status should be AVAILABLE (quantity > reorder_level)
    EXPECT_EQ(items[0].status, InventoryStatus::AVAILABLE);
}

TEST_F(InventoryTest, statusComputationLowStock)
{
    Inventory item{};
    item.name = "Fertilizer";
    item.category = InventoryCategory::OTHER;
    item.quantity = 15.0;  // At or below reorder level
    item.unit = InventoryUnit::KG;
    item.reorder_level = 20.0;

    EXPECT_NO_THROW(inv_service->addItem(item));

    auto items = inv_service->getAllItems();
    ASSERT_EQ(items.size(), 1);
    
    // Status should be LOW_STOCK (quantity <= reorder_level but > 0)
    EXPECT_EQ(items[0].status, InventoryStatus::LOW_STOCK);
}

TEST_F(InventoryTest, statusComputationOutOfStock)
{
    Inventory item{};
    item.name = "Empty Container";
    item.category = InventoryCategory::EQUIPMENT;
    item.quantity = 0.0;  // Zero quantity
    item.unit = InventoryUnit::PIECE;
    item.reorder_level = 5.0;

    EXPECT_NO_THROW(inv_service->addItem(item));

    auto items = inv_service->getAllItems();
    ASSERT_EQ(items.size(), 1);
    
    // Status should be OUT_OF_STOCK (quantity <= 0)
    EXPECT_EQ(items[0].status, InventoryStatus::OUT_OF_STOCK);
}

TEST_F(InventoryTest, statusUpdateAfterQuantityChange)
{
    Inventory item{};
    item.name = "Hay";
    item.category = InventoryCategory::FEED;
    item.quantity = 100.0;  // Start with AVAILABLE status
    item.unit = InventoryUnit::TON;
    item.reorder_level = 20.0;

    EXPECT_NO_THROW(inv_service->addItem(item));

    auto items = inv_service->getAllItems();
    ASSERT_EQ(items.size(), 1);
    i64 itemId = items[0].item_id;
    
    // Initially AVAILABLE
    EXPECT_EQ(items[0].status, InventoryStatus::AVAILABLE);

    // Update to low stock level
    EXPECT_NO_THROW(inv_service->updateQuantity(itemId, 15.0));
    Inventory lowStockItem = inv_service->getItemById(itemId);
    EXPECT_EQ(lowStockItem.status, InventoryStatus::LOW_STOCK);

    // Update to out of stock
    EXPECT_NO_THROW(inv_service->updateQuantity(itemId, 0.0));
    Inventory outOfStockItem = inv_service->getItemById(itemId);
    EXPECT_EQ(outOfStockItem.status, InventoryStatus::OUT_OF_STOCK);

    // Restock
    EXPECT_NO_THROW(inv_service->updateQuantity(itemId, 100.0));
    Inventory restockedItem = inv_service->getItemById(itemId);
    EXPECT_EQ(restockedItem.status, InventoryStatus::AVAILABLE);
}

TEST_F(InventoryTest, multipleItemsDifferentCategories)
{
    Inventory medicine{};
    medicine.name = "Vaccine";
    medicine.category = InventoryCategory::MEDICINE;
    medicine.quantity = 30.0;
    medicine.unit = InventoryUnit::PIECE;
    medicine.reorder_level = 10.0;

    Inventory feed{};
    feed.name = "Grain";
    feed.category = InventoryCategory::FEED;
    feed.quantity = 500.0;
    feed.unit = InventoryUnit::KG;
    feed.reorder_level = 100.0;

    Inventory equipment{};
    equipment.name = "Buckets";
    equipment.category = InventoryCategory::EQUIPMENT;
    equipment.quantity = 20.0;
    equipment.unit = InventoryUnit::PIECE;
    equipment.reorder_level = 5.0;

    EXPECT_NO_THROW(inv_service->addItem(medicine));
    EXPECT_NO_THROW(inv_service->addItem(feed));
    EXPECT_NO_THROW(inv_service->addItem(equipment));

    auto items = inv_service->getAllItems();
    EXPECT_EQ(items.size(), 3);

    // Verify all items are present with correct categories
    bool hasMedicine = false, hasFeed = false, hasEquipment = false;
    for (const auto& item : items) {
        if (item.category == InventoryCategory::MEDICINE) hasMedicine = true;
        if (item.category == InventoryCategory::FEED) hasFeed = true;
        if (item.category == InventoryCategory::EQUIPMENT) hasEquipment = true;
    }

    EXPECT_TRUE(hasMedicine);
    EXPECT_TRUE(hasFeed);
    EXPECT_TRUE(hasEquipment);
}

TEST_F(InventoryTest, uniqueNameConstraint)
{
    Inventory item1{};
    item1.name = "Duplicate Item";
    item1.category = InventoryCategory::OTHER;
    item1.quantity = 10.0;
    item1.unit = InventoryUnit::PIECE;
    item1.reorder_level = 5.0;

    EXPECT_NO_THROW(inv_service->addItem(item1));

    // Try to add another item with the same name
    Inventory item2{};
    item2.name = "Duplicate Item";  // Same name
    item2.category = InventoryCategory::FEED;
    item2.quantity = 20.0;
    item2.unit = InventoryUnit::KG;
    item2.reorder_level = 10.0;

    // Should throw due to UNIQUE constraint on name
    EXPECT_THROW(inv_service->addItem(item2), std::runtime_error);
}

TEST_F(InventoryTest, allInventoryCategories)
{
    Inventory medicine{};
    medicine.name = "Medicine Test";
    medicine.category = InventoryCategory::MEDICINE;
    medicine.quantity = 10.0;
    medicine.unit = InventoryUnit::PIECE;
    medicine.reorder_level = 5.0;

    Inventory feed{};
    feed.name = "Feed Test";
    feed.category = InventoryCategory::FEED;
    feed.quantity = 100.0;
    feed.unit = InventoryUnit::KG;
    feed.reorder_level = 20.0;

    Inventory equipment{};
    equipment.name = "Equipment Test";
    equipment.category = InventoryCategory::EQUIPMENT;
    equipment.quantity = 5.0;
    equipment.unit = InventoryUnit::PIECE;
    equipment.reorder_level = 2.0;

    Inventory milk{};
    milk.name = "Milk Test";
    milk.category = InventoryCategory::MILK;
    milk.quantity = 50.0;
    milk.unit = InventoryUnit::LITER;
    milk.reorder_level = 10.0;

    Inventory egg{};
    egg.name = "Egg Test";
    egg.category = InventoryCategory::EGG;
    egg.quantity = 200.0;
    egg.unit = InventoryUnit::PIECE;
    egg.reorder_level = 50.0;

    Inventory other{};
    other.name = "Other Test";
    other.category = InventoryCategory::OTHER;
    other.quantity = 15.0;
    other.unit = InventoryUnit::PACK;
    other.reorder_level = 5.0;

    EXPECT_NO_THROW(inv_service->addItem(medicine));
    EXPECT_NO_THROW(inv_service->addItem(feed));
    EXPECT_NO_THROW(inv_service->addItem(equipment));
    EXPECT_NO_THROW(inv_service->addItem(milk));
    EXPECT_NO_THROW(inv_service->addItem(egg));
    EXPECT_NO_THROW(inv_service->addItem(other));

    auto items = inv_service->getAllItems();
    EXPECT_EQ(items.size(), 6);

    // Verify each category is present
    std::map<InventoryCategory, bool> categoryFound;
    for (const auto& item : items) {
        categoryFound[item.category] = true;
    }

    EXPECT_TRUE(categoryFound[InventoryCategory::MEDICINE]);
    EXPECT_TRUE(categoryFound[InventoryCategory::FEED]);
    EXPECT_TRUE(categoryFound[InventoryCategory::EQUIPMENT]);
    EXPECT_TRUE(categoryFound[InventoryCategory::MILK]);
    EXPECT_TRUE(categoryFound[InventoryCategory::EGG]);
    EXPECT_TRUE(categoryFound[InventoryCategory::OTHER]);
}

TEST_F(InventoryTest, allInventoryUnits)
{
    Inventory kg_item{};
    kg_item.name = "KG Item";
    kg_item.category = InventoryCategory::FEED;
    kg_item.quantity = 100.0;
    kg_item.unit = InventoryUnit::KG;
    kg_item.reorder_level = 20.0;

    Inventory liter_item{};
    liter_item.name = "Liter Item";
    liter_item.category = InventoryCategory::MILK;
    liter_item.quantity = 50.0;
    liter_item.unit = InventoryUnit::LITER;
    liter_item.reorder_level = 10.0;

    Inventory piece_item{};
    piece_item.name = "Piece Item";
    piece_item.category = InventoryCategory::EGG;
    piece_item.quantity = 100.0;
    piece_item.unit = InventoryUnit::PIECE;
    piece_item.reorder_level = 30.0;

    Inventory ton_item{};
    ton_item.name = "Ton Item";
    ton_item.category = InventoryCategory::FEED;
    ton_item.quantity = 2.5;
    ton_item.unit = InventoryUnit::TON;
    ton_item.reorder_level = 0.5;

    Inventory pack_item{};
    pack_item.name = "Pack Item";
    pack_item.category = InventoryCategory::MEDICINE;
    pack_item.quantity = 25.0;
    pack_item.unit = InventoryUnit::PACK;
    pack_item.reorder_level = 5.0;

    EXPECT_NO_THROW(inv_service->addItem(kg_item));
    EXPECT_NO_THROW(inv_service->addItem(liter_item));
    EXPECT_NO_THROW(inv_service->addItem(piece_item));
    EXPECT_NO_THROW(inv_service->addItem(ton_item));
    EXPECT_NO_THROW(inv_service->addItem(pack_item));

    auto items = inv_service->getAllItems();
    EXPECT_EQ(items.size(), 5);

    // Verify each unit is present
    std::map<InventoryUnit, bool> unitFound;
    for (const auto& item : items) {
        unitFound[item.unit] = true;
    }

    EXPECT_TRUE(unitFound[InventoryUnit::KG]);
    EXPECT_TRUE(unitFound[InventoryUnit::LITER]);
    EXPECT_TRUE(unitFound[InventoryUnit::PIECE]);
    EXPECT_TRUE(unitFound[InventoryUnit::TON]);
    EXPECT_TRUE(unitFound[InventoryUnit::PACK]);
}

TEST_F(InventoryTest, edgeCaseNegativeQuantity)
{
    Inventory item{};
    item.name = "Negative Test";
    item.category = InventoryCategory::OTHER;
    item.quantity = -10.0;  // Negative quantity
    item.unit = InventoryUnit::PIECE;
    item.reorder_level = 5.0;

    EXPECT_NO_THROW(inv_service->addItem(item));

    auto items = inv_service->getAllItems();
    ASSERT_EQ(items.size(), 1);
    
    // Negative quantity should be treated as OUT_OF_STOCK
    EXPECT_EQ(items[0].status, InventoryStatus::OUT_OF_STOCK);
}

TEST_F(InventoryTest, zeroReorderLevel)
{
    Inventory item{};
    item.name = "Zero Reorder";
    item.category = InventoryCategory::EQUIPMENT;
    item.quantity = 10.0;
    item.unit = InventoryUnit::PIECE;
    item.reorder_level = 0.0;  // Zero reorder level

    EXPECT_NO_THROW(inv_service->addItem(item));

    auto items = inv_service->getAllItems();
    ASSERT_EQ(items.size(), 1);
    
    // With quantity > 0 and reorder_level = 0, should be AVAILABLE
    EXPECT_EQ(items[0].status, InventoryStatus::AVAILABLE);
}

TEST_F(InventoryTest, exactReorderLevelThreshold)
{
    Inventory item{};
    item.name = "Threshold Test";
    item.category = InventoryCategory::FEED;
    item.quantity = 20.0;  // Exactly at reorder level
    item.unit = InventoryUnit::KG;
    item.reorder_level = 20.0;

    EXPECT_NO_THROW(inv_service->addItem(item));

    auto items = inv_service->getAllItems();
    ASSERT_EQ(items.size(), 1);
    
    // At exact reorder level should be LOW_STOCK (quantity <= reorder_level)
    EXPECT_EQ(items[0].status, InventoryStatus::LOW_STOCK);
}

TEST_F(InventoryTest, multipleQuantityUpdates)
{
    Inventory item{};
    item.name = "Multi Update";
    item.category = InventoryCategory::FEED;
    item.quantity = 100.0;
    item.unit = InventoryUnit::KG;
    item.reorder_level = 30.0;

    EXPECT_NO_THROW(inv_service->addItem(item));

    auto items = inv_service->getAllItems();
    ASSERT_EQ(items.size(), 1);
    i64 itemId = items[0].item_id;

    // First update
    EXPECT_NO_THROW(inv_service->updateQuantity(itemId, 80.0));
    Inventory updated1 = inv_service->getItemById(itemId);
    EXPECT_EQ(updated1.quantity, 80.0);

    // Second update
    EXPECT_NO_THROW(inv_service->updateQuantity(itemId, 45.0));
    Inventory updated2 = inv_service->getItemById(itemId);
    EXPECT_EQ(updated2.quantity, 45.0);

    // Third update
    EXPECT_NO_THROW(inv_service->updateQuantity(itemId, 150.0));
    Inventory updated3 = inv_service->getItemById(itemId);
    EXPECT_EQ(updated3.quantity, 150.0);
}

TEST_F(InventoryTest, emptyInventoryList)
{
    // Don't add any items
    auto items = inv_service->getAllItems();
    EXPECT_EQ(items.size(), 0);
    EXPECT_TRUE(items.empty());
}