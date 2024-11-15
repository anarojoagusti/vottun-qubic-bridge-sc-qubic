#define NO_UEFI

#include "../include/QubicOrderContract.h"
#include <gtest/gtest.h>

class QubicOrderContractTests : public ::testing::Test {
protected:
    QubicOrderContract contract;
};

// Test for `pushOrder`
TEST_F(QubicOrderContractTests, PushOrder) {
    // Create a new Order with specific details
    Order inputOrder;
    inputOrder.user = "user1";
    inputOrder.amount = 100;
    inputOrder.orderType = "mint";
    inputOrder.status = "Pending";

    // Call pushOrder with inputOrder values
    contract.pushOrder(inputOrder);

    // Check if the order was added correctly
    EXPECT_EQ(contract.orderQueue.size(), 1);
    EXPECT_EQ(contract.orderQueue.front().user, "user1");
    EXPECT_EQ(contract.orderQueue.front().amount, 100);
    EXPECT_EQ(contract.orderQueue.front().status, "Pending");
}

// Test for `pullOrder`
TEST_F(QubicOrderContractTests, PullOrder) {
    // Create and add an order
    Order inputOrder;
    inputOrder.user = "user2";
    inputOrder.amount = 150;
    inputOrder.orderType = "transfer";
    inputOrder.status = "Pending";

    contract.pushOrder(inputOrder);

    // Call pullOrder and check the pulled order values
    Order outputOrder = contract.pullOrder();
    EXPECT_EQ(outputOrder.user, "user2");
    EXPECT_EQ(outputOrder.amount, 150);
    EXPECT_EQ(contract.orderQueue.size(), 0);
}

// Test for `updateOrder`
TEST_F(QubicOrderContractTests, UpdateOrder) {
    // Create and add an order
    Order inputOrder;
    inputOrder.user = "user3";
    inputOrder.amount = 200;
    inputOrder.orderType = "lock";
    inputOrder.status = "Pending";

    contract.pushOrder(inputOrder);

    // Update order status
    uint64_t orderId = contract.orderQueue.front().orderId;
    contract.updateOrder(orderId, "Completed");

    // Check that the status has been updated
    EXPECT_EQ(contract.orderQueue.front().status, "Completed");
}

// Test for `refundOrder`
TEST_F(QubicOrderContractTests, RefundOrder) {
    // Create and add an order
    Order inputOrder;
    inputOrder.user = "user4";
    inputOrder.amount = 300;
    inputOrder.orderType = "lock";
    inputOrder.status = "Pending";

    contract.pushOrder(inputOrder);

    // Refund the order
    uint64_t orderId = contract.orderQueue.front().orderId;
    contract.refundOrder(orderId);

    // Check that the status is "Refunded"
    EXPECT_EQ(contract.orderQueue.front().status, "Refunded");
}

