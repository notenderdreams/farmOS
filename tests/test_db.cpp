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

TEST_F(DbTest, getAllTransactions)
{
    Transaction tx1{};
    tx1.type = TransactionType::BUY;
    tx1.direction = TransactionDirection::OUT;
    tx1.amount = 50.0;
    tx1.entity_type = TransactionEntityType::GOODS;
    tx1.entity_id = "101";
    tx1.description = "Seeds";
    tx1.date = "2026-02-06";
    tx1.status = TransactionStatus::COMPLETED;

    Transaction tx2{};
    tx2.type = TransactionType::SELL;
    tx2.direction = TransactionDirection::IN;
    tx2.amount = 120.0;
    tx2.entity_type = TransactionEntityType::ANIMAL;
    tx2.entity_id = "202";
    tx2.description = "Sheep sold";
    tx2.date = "2026-02-06";
    tx2.status = TransactionStatus::COMPLETED;

    EXPECT_NO_THROW(service->addTransaction(tx1));
    EXPECT_NO_THROW(service->addTransaction(tx2));

    std::vector<Transaction> transactions;
    EXPECT_NO_THROW(transactions = service->getAllTransactions());

    EXPECT_EQ(transactions.size(), 2);

    EXPECT_EQ(transactions[0].type, TransactionType::BUY);
    EXPECT_EQ(transactions[0].amount, 50.0);
    EXPECT_EQ(transactions[0].entity_type, TransactionEntityType::GOODS);

    EXPECT_EQ(transactions[1].type, TransactionType::SELL);
    EXPECT_EQ(transactions[1].amount, 120.0);
    EXPECT_EQ(transactions[1].entity_type, TransactionEntityType::ANIMAL);
}

TEST_F(DbTest, getTransactionById)
{
    Transaction tx{};
    tx.type = TransactionType::SALARY;
    tx.direction = TransactionDirection::OUT;
    tx.amount = 1000.0;
    tx.entity_type = TransactionEntityType::EMPLOYEE;
    tx.entity_id = 1;
    tx.description = "Employee salary";
    tx.date = "2026-02-06";
    tx.status = TransactionStatus::COMPLETED;

    EXPECT_NO_THROW(service->addTransaction(tx));

    std::vector<Transaction> allTx = service->getAllTransactions();
    ASSERT_EQ(allTx.size(), 1);
    i64 txId = allTx[0].tid;

    Transaction fetchedTx;
    EXPECT_NO_THROW(fetchedTx = service->getTransactionById(txId));

    EXPECT_EQ(fetchedTx.tid, txId);
    EXPECT_EQ(fetchedTx.type, TransactionType::SALARY);
    EXPECT_EQ(fetchedTx.amount, 1000.0);
    EXPECT_EQ(fetchedTx.entity_type, TransactionEntityType::EMPLOYEE);
    EXPECT_EQ(fetchedTx.description, "Employee salary");
}

TEST_F(DbTest, updateTransactionStatus) {
    Transaction tx{};
    tx.type = TransactionType::BILLS;
    tx.direction = TransactionDirection::OUT;
    tx.amount = 200.0;
    tx.entity_type = TransactionEntityType::GOODS;
    tx.entity_id = "1";
    tx.description = "Fertilizer bill";
    tx.date = "2026-02-06";
    tx.status = TransactionStatus::PENDING;

    EXPECT_NO_THROW(service->addTransaction(tx));

    auto allTx = service->getAllTransactions();
    ASSERT_EQ(allTx.size(), 1);

    i64 txId = allTx[0].tid;
    EXPECT_EQ(allTx[0].status, TransactionStatus::PENDING);

    EXPECT_NO_THROW(service->updateStatus(txId, TransactionStatus::COMPLETED));

    Transaction updatedTx = service->getTransactionById(txId);
    EXPECT_EQ(updatedTx.status, TransactionStatus::COMPLETED);
}

TEST_F(DbTest, updateStatusInvalidId) {
    i64 invalidId = 67; 
    EXPECT_THROW(service->updateStatus(invalidId, TransactionStatus::COMPLETED), std::runtime_error);
}
