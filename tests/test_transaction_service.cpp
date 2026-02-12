#include <gtest/gtest.h>
#include "core/transaction_service.h"
#include <sqlite3.h>
#include <memory>

class TransactionTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        ASSERT_EQ(sqlite3_open(":memory:", &db), SQLITE_OK);
        tx_service = std::make_unique<TransactionService>(db);
        tx_service->initTable();
    }

    void TearDown() override
    {
        tx_service.reset();
        sqlite3_close(db);
        db = nullptr;
    }

    sqlite3* db = nullptr;
    std::unique_ptr<TransactionService> tx_service;
};

// ==================== BASIC CRUD OPERATIONS ====================

TEST_F(TransactionTest, insertsTransaction)
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

    EXPECT_NO_THROW(tx_service->addTransaction(tx));

    sqlite3_stmt* stmt = nullptr;
    ASSERT_EQ(sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM transactions;", -1, &stmt, nullptr), SQLITE_OK);
    ASSERT_EQ(sqlite3_step(stmt), SQLITE_ROW);
    int count = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    EXPECT_EQ(count, 1);
}

TEST_F(TransactionTest, getAllTransactions)
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

    EXPECT_NO_THROW(tx_service->addTransaction(tx1));
    EXPECT_NO_THROW(tx_service->addTransaction(tx2));

    std::vector<Transaction> transactions;
    EXPECT_NO_THROW(transactions = tx_service->getAllTransactions());

    EXPECT_EQ(transactions.size(), 2);

    EXPECT_EQ(transactions[0].type, TransactionType::BUY);
    EXPECT_EQ(transactions[0].amount, 50.0);
    EXPECT_EQ(transactions[0].entity_type, TransactionEntityType::GOODS);

    EXPECT_EQ(transactions[1].type, TransactionType::SELL);
    EXPECT_EQ(transactions[1].amount, 120.0);
    EXPECT_EQ(transactions[1].entity_type, TransactionEntityType::ANIMAL);
}

TEST_F(TransactionTest, getTransactionById)
{
    Transaction tx{};
    tx.type = TransactionType::SALARY;
    tx.direction = TransactionDirection::OUT;
    tx.amount = 1000.0;
    tx.entity_type = TransactionEntityType::EMPLOYEE;
    tx.entity_id = "1";
    tx.description = "Employee salary";
    tx.date = "2026-02-06";
    tx.status = TransactionStatus::COMPLETED;

    EXPECT_NO_THROW(tx_service->addTransaction(tx));

    std::vector<Transaction> allTx = tx_service->getAllTransactions();
    ASSERT_EQ(allTx.size(), 1);
    i64 txId = allTx[0].tid;

    Transaction fetchedTx;
    EXPECT_NO_THROW(fetchedTx = tx_service->getTransactionById(txId));

    EXPECT_EQ(fetchedTx.tid, txId);
    EXPECT_EQ(fetchedTx.type, TransactionType::SALARY);
    EXPECT_EQ(fetchedTx.amount, 1000.0);
    EXPECT_EQ(fetchedTx.entity_type, TransactionEntityType::EMPLOYEE);
    EXPECT_EQ(fetchedTx.description, "Employee salary");
}

TEST_F(TransactionTest, updateTransactionStatus)
{
    Transaction tx{};
    tx.type = TransactionType::BILLS;
    tx.direction = TransactionDirection::OUT;
    tx.amount = 200.0;
    tx.entity_type = TransactionEntityType::GOODS;
    tx.entity_id = "1";
    tx.description = "Fertilizer bill";
    tx.date = "2026-02-06";
    tx.status = TransactionStatus::PENDING;

    EXPECT_NO_THROW(tx_service->addTransaction(tx));

    auto allTx = tx_service->getAllTransactions();
    ASSERT_EQ(allTx.size(), 1);

    i64 txId = allTx[0].tid;
    EXPECT_EQ(allTx[0].status, TransactionStatus::PENDING);

    EXPECT_NO_THROW(tx_service->updateStatus(txId, TransactionStatus::COMPLETED));

    Transaction updatedTx = tx_service->getTransactionById(txId);
    EXPECT_EQ(updatedTx.status, TransactionStatus::COMPLETED);
}

TEST_F(TransactionTest, emptyTransactionList)
{
    auto transactions = tx_service->getAllTransactions();
    EXPECT_EQ(transactions.size(), 0);
    EXPECT_TRUE(transactions.empty());
}

// ==================== ERROR HANDLING ====================

TEST_F(TransactionTest, getTransactionByIdInvalid)
{
    i64 invalidId = 999;
    EXPECT_THROW(tx_service->getTransactionById(invalidId), std::runtime_error);
}

TEST_F(TransactionTest, updateStatusInvalidId)
{
    i64 invalidId = 67;
    EXPECT_THROW(tx_service->updateStatus(invalidId, TransactionStatus::COMPLETED), std::runtime_error);
}

// ==================== ALL TRANSACTION TYPES ====================

TEST_F(TransactionTest, allTransactionTypes)
{
    Transaction buy_tx{};
    buy_tx.type = TransactionType::BUY;
    buy_tx.direction = TransactionDirection::OUT;
    buy_tx.amount = 100.0;
    buy_tx.entity_type = TransactionEntityType::GOODS;
    buy_tx.entity_id = "1";
    buy_tx.description = "Buy transaction";
    buy_tx.date = "2026-02-01";
    buy_tx.status = TransactionStatus::COMPLETED;

    Transaction sell_tx{};
    sell_tx.type = TransactionType::SELL;
    sell_tx.direction = TransactionDirection::IN;
    sell_tx.amount = 200.0;
    sell_tx.entity_type = TransactionEntityType::ANIMAL;
    sell_tx.entity_id = "2";
    sell_tx.description = "Sell transaction";
    sell_tx.date = "2026-02-02";
    sell_tx.status = TransactionStatus::COMPLETED;

    Transaction salary_tx{};
    salary_tx.type = TransactionType::SALARY;
    salary_tx.direction = TransactionDirection::OUT;
    salary_tx.amount = 1500.0;
    salary_tx.entity_type = TransactionEntityType::EMPLOYEE;
    salary_tx.entity_id = "3";
    salary_tx.description = "Salary transaction";
    salary_tx.date = "2026-02-03";
    salary_tx.status = TransactionStatus::COMPLETED;

    Transaction bills_tx{};
    bills_tx.type = TransactionType::BILLS;
    bills_tx.direction = TransactionDirection::OUT;
    bills_tx.amount = 300.0;
    bills_tx.entity_type = TransactionEntityType::GOODS;
    bills_tx.entity_id = "4";
    bills_tx.description = "Bills transaction";
    bills_tx.date = "2026-02-04";
    bills_tx.status = TransactionStatus::COMPLETED;

    EXPECT_NO_THROW(tx_service->addTransaction(buy_tx));
    EXPECT_NO_THROW(tx_service->addTransaction(sell_tx));
    EXPECT_NO_THROW(tx_service->addTransaction(salary_tx));
    EXPECT_NO_THROW(tx_service->addTransaction(bills_tx));

    auto transactions = tx_service->getAllTransactions();
    EXPECT_EQ(transactions.size(), 4);

    // Verify all types are present
    std::map<TransactionType, bool> typeFound;
    for (const auto& tx : transactions) {
        typeFound[tx.type] = true;
    }

    EXPECT_TRUE(typeFound[TransactionType::BUY]);
    EXPECT_TRUE(typeFound[TransactionType::SELL]);
    EXPECT_TRUE(typeFound[TransactionType::SALARY]);
    EXPECT_TRUE(typeFound[TransactionType::BILLS]);
}

// ==================== ALL TRANSACTION STATUSES ====================

TEST_F(TransactionTest, allTransactionStatuses)
{
    Transaction completed_tx{};
    completed_tx.type = TransactionType::BUY;
    completed_tx.direction = TransactionDirection::OUT;
    completed_tx.amount = 100.0;
    completed_tx.entity_type = TransactionEntityType::GOODS;
    completed_tx.entity_id = "1";
    completed_tx.description = "Completed transaction";
    completed_tx.date = "2026-02-01";
    completed_tx.status = TransactionStatus::COMPLETED;

    Transaction pending_tx{};
    pending_tx.type = TransactionType::SELL;
    pending_tx.direction = TransactionDirection::IN;
    pending_tx.amount = 200.0;
    pending_tx.entity_type = TransactionEntityType::ANIMAL;
    pending_tx.entity_id = "2";
    pending_tx.description = "Pending transaction";
    pending_tx.date = "2026-02-02";
    pending_tx.status = TransactionStatus::PENDING;

    Transaction cancelled_tx{};
    cancelled_tx.type = TransactionType::SALARY;
    cancelled_tx.direction = TransactionDirection::OUT;
    cancelled_tx.amount = 1500.0;
    cancelled_tx.entity_type = TransactionEntityType::EMPLOYEE;
    cancelled_tx.entity_id = "3";
    cancelled_tx.description = "Cancelled transaction";
    cancelled_tx.date = "2026-02-03";
    cancelled_tx.status = TransactionStatus::CANCELLED;

    EXPECT_NO_THROW(tx_service->addTransaction(completed_tx));
    EXPECT_NO_THROW(tx_service->addTransaction(pending_tx));
    EXPECT_NO_THROW(tx_service->addTransaction(cancelled_tx));

    auto transactions = tx_service->getAllTransactions();
    EXPECT_EQ(transactions.size(), 3);

    // Verify all statuses are present
    std::map<TransactionStatus, bool> statusFound;
    for (const auto& tx : transactions) {
        statusFound[tx.status] = true;
    }

    EXPECT_TRUE(statusFound[TransactionStatus::COMPLETED]);
    EXPECT_TRUE(statusFound[TransactionStatus::PENDING]);
    EXPECT_TRUE(statusFound[TransactionStatus::CANCELLED]);
}

// ==================== ALL ENTITY TYPES ====================

TEST_F(TransactionTest, allEntityTypes)
{
    Transaction animal_tx{};
    animal_tx.type = TransactionType::SELL;
    animal_tx.direction = TransactionDirection::IN;
    animal_tx.amount = 500.0;
    animal_tx.entity_type = TransactionEntityType::ANIMAL;
    animal_tx.entity_id = "COW-001";
    animal_tx.description = "Animal transaction";
    animal_tx.date = "2026-02-01";
    animal_tx.status = TransactionStatus::COMPLETED;

    Transaction employee_tx{};
    employee_tx.type = TransactionType::SALARY;
    employee_tx.direction = TransactionDirection::OUT;
    employee_tx.amount = 2000.0;
    employee_tx.entity_type = TransactionEntityType::EMPLOYEE;
    employee_tx.entity_id = "EMP-001";
    employee_tx.description = "Employee transaction";
    employee_tx.date = "2026-02-02";
    employee_tx.status = TransactionStatus::COMPLETED;

    Transaction goods_tx{};
    goods_tx.type = TransactionType::BUY;
    goods_tx.direction = TransactionDirection::OUT;
    goods_tx.amount = 150.0;
    goods_tx.entity_type = TransactionEntityType::GOODS;
    goods_tx.entity_id = "GOODS-001";
    goods_tx.description = "Goods transaction";
    goods_tx.date = "2026-02-03";
    goods_tx.status = TransactionStatus::COMPLETED;

    EXPECT_NO_THROW(tx_service->addTransaction(animal_tx));
    EXPECT_NO_THROW(tx_service->addTransaction(employee_tx));
    EXPECT_NO_THROW(tx_service->addTransaction(goods_tx));

    auto transactions = tx_service->getAllTransactions();
    EXPECT_EQ(transactions.size(), 3);

    // Verify all entity types are present
    std::map<TransactionEntityType, bool> entityFound;
    for (const auto& tx : transactions) {
        entityFound[tx.entity_type] = true;
    }

    EXPECT_TRUE(entityFound[TransactionEntityType::ANIMAL]);
    EXPECT_TRUE(entityFound[TransactionEntityType::EMPLOYEE]);
    EXPECT_TRUE(entityFound[TransactionEntityType::GOODS]);
}

// ==================== DIRECTION VALIDATION ====================

TEST_F(TransactionTest, transactionDirections)
{
    Transaction in_tx{};
    in_tx.type = TransactionType::SELL;
    in_tx.direction = TransactionDirection::IN;
    in_tx.amount = 300.0;
    in_tx.entity_type = TransactionEntityType::ANIMAL;
    in_tx.entity_id = "1";
    in_tx.description = "Money coming in";
    in_tx.date = "2026-02-01";
    in_tx.status = TransactionStatus::COMPLETED;

    Transaction out_tx{};
    out_tx.type = TransactionType::BUY;
    out_tx.direction = TransactionDirection::OUT;
    out_tx.amount = 150.0;
    out_tx.entity_type = TransactionEntityType::GOODS;
    out_tx.entity_id = "2";
    out_tx.description = "Money going out";
    out_tx.date = "2026-02-02";
    out_tx.status = TransactionStatus::COMPLETED;

    EXPECT_NO_THROW(tx_service->addTransaction(in_tx));
    EXPECT_NO_THROW(tx_service->addTransaction(out_tx));

    auto transactions = tx_service->getAllTransactions();
    EXPECT_EQ(transactions.size(), 2);

    EXPECT_EQ(transactions[0].direction, TransactionDirection::IN);
    EXPECT_EQ(transactions[1].direction, TransactionDirection::OUT);
}

// ==================== STATUS TRANSITIONS ====================

TEST_F(TransactionTest, statusTransitionPendingToCompleted)
{
    Transaction tx{};
    tx.type = TransactionType::BUY;
    tx.direction = TransactionDirection::OUT;
    tx.amount = 100.0;
    tx.entity_type = TransactionEntityType::GOODS;
    tx.entity_id = "1";
    tx.description = "Test transaction";
    tx.date = "2026-02-01";
    tx.status = TransactionStatus::PENDING;

    EXPECT_NO_THROW(tx_service->addTransaction(tx));

    auto transactions = tx_service->getAllTransactions();
    i64 txId = transactions[0].tid;

    EXPECT_EQ(transactions[0].status, TransactionStatus::PENDING);

    EXPECT_NO_THROW(tx_service->updateStatus(txId, TransactionStatus::COMPLETED));

    Transaction updated = tx_service->getTransactionById(txId);
    EXPECT_EQ(updated.status, TransactionStatus::COMPLETED);
}

TEST_F(TransactionTest, statusTransitionPendingToCancelled)
{
    Transaction tx{};
    tx.type = TransactionType::SELL;
    tx.direction = TransactionDirection::IN;
    tx.amount = 200.0;
    tx.entity_type = TransactionEntityType::ANIMAL;
    tx.entity_id = "1";
    tx.description = "Test transaction";
    tx.date = "2026-02-01";
    tx.status = TransactionStatus::PENDING;

    EXPECT_NO_THROW(tx_service->addTransaction(tx));

    auto transactions = tx_service->getAllTransactions();
    i64 txId = transactions[0].tid;

    EXPECT_EQ(transactions[0].status, TransactionStatus::PENDING);

    EXPECT_NO_THROW(tx_service->updateStatus(txId, TransactionStatus::CANCELLED));

    Transaction updated = tx_service->getTransactionById(txId);
    EXPECT_EQ(updated.status, TransactionStatus::CANCELLED);
}

TEST_F(TransactionTest, statusTransitionCompletedToCompleted)
{
    Transaction tx{};
    tx.type = TransactionType::SALARY;
    tx.direction = TransactionDirection::OUT;
    tx.amount = 1500.0;
    tx.entity_type = TransactionEntityType::EMPLOYEE;
    tx.entity_id = "1";
    tx.description = "Test transaction";
    tx.date = "2026-02-01";
    tx.status = TransactionStatus::COMPLETED;

    EXPECT_NO_THROW(tx_service->addTransaction(tx));

    auto transactions = tx_service->getAllTransactions();
    i64 txId = transactions[0].tid;

    // Updating completed to completed should work
    EXPECT_NO_THROW(tx_service->updateStatus(txId, TransactionStatus::COMPLETED));

    Transaction updated = tx_service->getTransactionById(txId);
    EXPECT_EQ(updated.status, TransactionStatus::COMPLETED);
}

// ==================== AMOUNT EDGE CASES ====================

TEST_F(TransactionTest, zeroAmountTransaction)
{
    Transaction tx{};
    tx.type = TransactionType::BUY;
    tx.direction = TransactionDirection::OUT;
    tx.amount = 0.0;  // Zero amount
    tx.entity_type = TransactionEntityType::GOODS;
    tx.entity_id = "1";
    tx.description = "Zero amount test";
    tx.date = "2026-02-01";
    tx.status = TransactionStatus::COMPLETED;

    EXPECT_NO_THROW(tx_service->addTransaction(tx));

    auto transactions = tx_service->getAllTransactions();
    ASSERT_EQ(transactions.size(), 1);
    EXPECT_EQ(transactions[0].amount, 0.0);
}

TEST_F(TransactionTest, largeAmountTransaction)
{
    Transaction tx{};
    tx.type = TransactionType::SELL;
    tx.direction = TransactionDirection::IN;
    tx.amount = 999999999.99;  // Large amount
    tx.entity_type = TransactionEntityType::ANIMAL;
    tx.entity_id = "1";
    tx.description = "Large amount test";
    tx.date = "2026-02-01";
    tx.status = TransactionStatus::COMPLETED;

    EXPECT_NO_THROW(tx_service->addTransaction(tx));

    auto transactions = tx_service->getAllTransactions();
    ASSERT_EQ(transactions.size(), 1);
    EXPECT_DOUBLE_EQ(transactions[0].amount, 999999999.99);
}

TEST_F(TransactionTest, decimalAmountTransaction)
{
    Transaction tx{};
    tx.type = TransactionType::BILLS;
    tx.direction = TransactionDirection::OUT;
    tx.amount = 123.45;  // Decimal amount
    tx.entity_type = TransactionEntityType::GOODS;
    tx.entity_id = "1";
    tx.description = "Decimal amount test";
    tx.date = "2026-02-01";
    tx.status = TransactionStatus::COMPLETED;

    EXPECT_NO_THROW(tx_service->addTransaction(tx));

    auto transactions = tx_service->getAllTransactions();
    ASSERT_EQ(transactions.size(), 1);
    EXPECT_DOUBLE_EQ(transactions[0].amount, 123.45);
}

// ==================== DATE HANDLING ====================

TEST_F(TransactionTest, multipleDatesTransactions)
{
    Transaction tx1{};
    tx1.type = TransactionType::BUY;
    tx1.direction = TransactionDirection::OUT;
    tx1.amount = 100.0;
    tx1.entity_type = TransactionEntityType::GOODS;
    tx1.entity_id = "1";
    tx1.description = "January transaction";
    tx1.date = "2026-01-15";
    tx1.status = TransactionStatus::COMPLETED;

    Transaction tx2{};
    tx2.type = TransactionType::SELL;
    tx2.direction = TransactionDirection::IN;
    tx2.amount = 200.0;
    tx2.entity_type = TransactionEntityType::ANIMAL;
    tx2.entity_id = "2";
    tx2.description = "February transaction";
    tx2.date = "2026-02-15";
    tx2.status = TransactionStatus::COMPLETED;

    Transaction tx3{};
    tx3.type = TransactionType::SALARY;
    tx3.direction = TransactionDirection::OUT;
    tx3.amount = 1500.0;
    tx3.entity_type = TransactionEntityType::EMPLOYEE;
    tx3.entity_id = "3";
    tx3.description = "March transaction";
    tx3.date = "2026-03-15";
    tx3.status = TransactionStatus::COMPLETED;

    EXPECT_NO_THROW(tx_service->addTransaction(tx1));
    EXPECT_NO_THROW(tx_service->addTransaction(tx2));
    EXPECT_NO_THROW(tx_service->addTransaction(tx3));

    auto transactions = tx_service->getAllTransactions();
    EXPECT_EQ(transactions.size(), 3);

    EXPECT_EQ(transactions[0].date, "2026-01-15");
    EXPECT_EQ(transactions[1].date, "2026-02-15");
    EXPECT_EQ(transactions[2].date, "2026-03-15");
}

// ==================== DESCRIPTION HANDLING ====================

TEST_F(TransactionTest, emptyDescriptionTransaction)
{
    Transaction tx{};
    tx.type = TransactionType::BUY;
    tx.direction = TransactionDirection::OUT;
    tx.amount = 100.0;
    tx.entity_type = TransactionEntityType::GOODS;
    tx.entity_id = "1";
    tx.description = "";  // Empty description
    tx.date = "2026-02-01";
    tx.status = TransactionStatus::COMPLETED;

    EXPECT_NO_THROW(tx_service->addTransaction(tx));

    auto transactions = tx_service->getAllTransactions();
    ASSERT_EQ(transactions.size(), 1);
    EXPECT_EQ(transactions[0].description, "");
}

TEST_F(TransactionTest, longDescriptionTransaction)
{
    Transaction tx{};
    tx.type = TransactionType::SELL;
    tx.direction = TransactionDirection::IN;
    tx.amount = 200.0;
    tx.entity_type = TransactionEntityType::ANIMAL;
    tx.entity_id = "1";
    tx.description = "This is a very long description that contains a lot of information about the transaction including details about what was sold, to whom, and for what purpose. It might span multiple lines in some displays.";
    tx.date = "2026-02-01";
    tx.status = TransactionStatus::COMPLETED;

    EXPECT_NO_THROW(tx_service->addTransaction(tx));

    auto transactions = tx_service->getAllTransactions();
    ASSERT_EQ(transactions.size(), 1);
    EXPECT_EQ(transactions[0].description.length(), tx.description.length());
}

// ==================== ENTITY ID VARIATIONS ====================

TEST_F(TransactionTest, numericEntityId)
{
    Transaction tx{};
    tx.type = TransactionType::BUY;
    tx.direction = TransactionDirection::OUT;
    tx.amount = 100.0;
    tx.entity_type = TransactionEntityType::GOODS;
    tx.entity_id = "12345";  // Numeric string
    tx.description = "Numeric entity ID";
    tx.date = "2026-02-01";
    tx.status = TransactionStatus::COMPLETED;

    EXPECT_NO_THROW(tx_service->addTransaction(tx));

    auto transactions = tx_service->getAllTransactions();
    ASSERT_EQ(transactions.size(), 1);
    EXPECT_EQ(transactions[0].entity_id, "12345");
}

TEST_F(TransactionTest, alphanumericEntityId)
{
    Transaction tx{};
    tx.type = TransactionType::SELL;
    tx.direction = TransactionDirection::IN;
    tx.amount = 200.0;
    tx.entity_type = TransactionEntityType::ANIMAL;
    tx.entity_id = "COW-ABC-123";  // Alphanumeric
    tx.description = "Alphanumeric entity ID";
    tx.date = "2026-02-01";
    tx.status = TransactionStatus::COMPLETED;

    EXPECT_NO_THROW(tx_service->addTransaction(tx));

    auto transactions = tx_service->getAllTransactions();
    ASSERT_EQ(transactions.size(), 1);
    EXPECT_EQ(transactions[0].entity_id, "COW-ABC-123");
}

// ==================== MULTIPLE OPERATIONS ====================

TEST_F(TransactionTest, multipleStatusUpdates)
{
    Transaction tx{};
    tx.type = TransactionType::BUY;
    tx.direction = TransactionDirection::OUT;
    tx.amount = 100.0;
    tx.entity_type = TransactionEntityType::GOODS;
    tx.entity_id = "1";
    tx.description = "Multiple updates test";
    tx.date = "2026-02-01";
    tx.status = TransactionStatus::PENDING;

    EXPECT_NO_THROW(tx_service->addTransaction(tx));

    auto transactions = tx_service->getAllTransactions();
    i64 txId = transactions[0].tid;

    // First update
    EXPECT_NO_THROW(tx_service->updateStatus(txId, TransactionStatus::COMPLETED));
    Transaction updated1 = tx_service->getTransactionById(txId);
    EXPECT_EQ(updated1.status, TransactionStatus::COMPLETED);

    // Second update
    EXPECT_NO_THROW(tx_service->updateStatus(txId, TransactionStatus::CANCELLED));
    Transaction updated2 = tx_service->getTransactionById(txId);
    EXPECT_EQ(updated2.status, TransactionStatus::CANCELLED);

    // Third update back to pending
    EXPECT_NO_THROW(tx_service->updateStatus(txId, TransactionStatus::PENDING));
    Transaction updated3 = tx_service->getTransactionById(txId);
    EXPECT_EQ(updated3.status, TransactionStatus::PENDING);
}

TEST_F(TransactionTest, manyTransactions)
{
    // Add 100 transactions
    for (int i = 0; i < 100; ++i) {
        Transaction tx{};
        tx.type = (i % 2 == 0) ? TransactionType::BUY : TransactionType::SELL;
        tx.direction = (i % 2 == 0) ? TransactionDirection::OUT : TransactionDirection::IN;
        tx.amount = 100.0 + i;
        tx.entity_type = TransactionEntityType::GOODS;
        tx.entity_id = std::to_string(i);
        tx.description = "Transaction " + std::to_string(i);
        tx.date = "2026-02-01";
        tx.status = TransactionStatus::COMPLETED;

        EXPECT_NO_THROW(tx_service->addTransaction(tx));
    }

    auto transactions = tx_service->getAllTransactions();
    EXPECT_EQ(transactions.size(), 100);
}

// ==================== REALISTIC SCENARIOS ====================

TEST_F(TransactionTest, mixedTransactionScenario)
{
    // Purchase feed
    Transaction feed_purchase{};
    feed_purchase.type = TransactionType::BUY;
    feed_purchase.direction = TransactionDirection::OUT;
    feed_purchase.amount = 350.75;
    feed_purchase.entity_type = TransactionEntityType::GOODS;
    feed_purchase.entity_id = "FEED-001";
    feed_purchase.description = "Cattle feed purchase";
    feed_purchase.date = "2026-02-01";
    feed_purchase.status = TransactionStatus::COMPLETED;

    // Sell livestock
    Transaction livestock_sale{};
    livestock_sale.type = TransactionType::SELL;
    livestock_sale.direction = TransactionDirection::IN;
    livestock_sale.amount = 1250.00;
    livestock_sale.entity_type = TransactionEntityType::ANIMAL;
    livestock_sale.entity_id = "COW-042";
    livestock_sale.description = "Sold cow to market";
    livestock_sale.date = "2026-02-05";
    livestock_sale.status = TransactionStatus::COMPLETED;

    // Pay employee salary
    Transaction salary_payment{};
    salary_payment.type = TransactionType::SALARY;
    salary_payment.direction = TransactionDirection::OUT;
    salary_payment.amount = 2500.00;
    salary_payment.entity_type = TransactionEntityType::EMPLOYEE;
    salary_payment.entity_id = "EMP-005";
    salary_payment.description = "Monthly salary - John Doe";
    salary_payment.date = "2026-02-10";
    salary_payment.status = TransactionStatus::COMPLETED;

    // Electricity bill (pending)
    Transaction electricity_bill{};
    electricity_bill.type = TransactionType::BILLS;
    electricity_bill.direction = TransactionDirection::OUT;
    electricity_bill.amount = 450.30;
    electricity_bill.entity_type = TransactionEntityType::GOODS;
    electricity_bill.entity_id = "UTIL-001";
    electricity_bill.description = "Monthly electricity bill";
    electricity_bill.date = "2026-02-12";
    electricity_bill.status = TransactionStatus::PENDING;

    EXPECT_NO_THROW(tx_service->addTransaction(feed_purchase));
    EXPECT_NO_THROW(tx_service->addTransaction(livestock_sale));
    EXPECT_NO_THROW(tx_service->addTransaction(salary_payment));
    EXPECT_NO_THROW(tx_service->addTransaction(electricity_bill));

    auto transactions = tx_service->getAllTransactions();
    EXPECT_EQ(transactions.size(), 4);

    // Calculate balance (IN - OUT)
    double balance = 0.0;
    for (const auto& tx : transactions) {
        if (tx.status == TransactionStatus::COMPLETED) {
            if (tx.direction == TransactionDirection::IN) {
                balance += tx.amount;
            } else {
                balance -= tx.amount;
            }
        }
    }

    // 1250.00 (livestock) - 350.75 (feed) - 2500.00 (salary) = -1600.75
    EXPECT_DOUBLE_EQ(balance, -1600.75);
}