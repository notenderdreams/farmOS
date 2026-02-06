#include <gtest/gtest.h>
#include "core/transaction_service.h"
#include <sqlite3.h>
#include <memory>

class DbTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        ASSERT_EQ(sqlite3_open(":memory:", &db), SQLITE_OK);
        service = std::make_unique<TransactionService>(db);
        service->initTable();
    }

    void TearDown() override
    {
        service.reset();
        sqlite3_close(db);
        db = nullptr;
    }

    sqlite3* db = nullptr;
    std::unique_ptr<TransactionService> service;
};

TEST_F(DbTest, insertsTransaction)
{
    Transaction tx{};
    tx.type = TransactionType::BUY;
    tx.direction = TransactionDirection::OUT;
    tx.amount = 67.9;
    tx.entity_type = TransactionEntityType::GOODS;
    tx.entity_id = "123";
    tx.description = "Kitten purchase";
    tx.date = "2026-02-06";
    tx.status = TransactionStatus::COMPLETED;

    EXPECT_NO_THROW(service->addTransaction(tx));

    sqlite3_stmt* stmt = nullptr;
    ASSERT_EQ(sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM transactions;", -1, &stmt, nullptr), SQLITE_OK);
    ASSERT_EQ(sqlite3_step(stmt), SQLITE_ROW);
    int count = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    EXPECT_EQ(count, 1);
}