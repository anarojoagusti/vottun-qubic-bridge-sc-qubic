#ifndef QUBIC_ORDER_CONTRACT_H
#define QUBIC_ORDER_CONTRACT_H

#include <string>
#include <cstdint>
#include <queue>
#include <optional>
using namespace QPI;

// Order structure
struct Order {
    uint64_t orderId;
    std::string user;
    uint64_t amount;
    std::string orderType;  /*[ARojo](mint? transfer? lock?)*/
    std::string status; /*[ARojo]string or enum? need agreement on states: "Pending", "Completed", "Refunded"?*/
};

// QubicOrderContract definition
class QubicOrderContract {
    
private:
    std::queue<Order> orderQueue;
    uint64_t orderCounter = 0; /*[ARojo] Now the orderID is a simple counter. Need consensus on nomenclature*/
    
    // Helper function to find an order by its ID in the queue
    Order* findOrderById(uint64_t orderId);

public:
    // Push Order to the FIFO Queue
    PUBLIC_PROCEDURE_WITH_LOCALS(pushOrder) {
        Order newOrder;
        newOrder.orderId = ++orderCounter;
        newOrder.user = input.user;
        newOrder.amount = input.amount;
        newOrder.orderType = input.orderType;
        newOrder.status = "Pending";

        orderQueue.push(newOrder); // Add order to the queue

        /*[ARojo] Emit some event*/
    }

    // Pull the oldest Order from the FIFO Queue
    PUBLIC_PROCEDURE_WITH_LOCALS(pullOrder) {
        if (orderQueue.empty()) {
            output.returnCode = "EMPTY_QUEUE";
            return;
        }

        Order oldestOrder = orderQueue.front();
        orderQueue.pop();  // Remove order from queue

        output.orderId = oldestOrder.orderId;  // Return data
        output.user = oldestOrder.user;
        output.amount = oldestOrder.amount;
        output.orderType = oldestOrder.orderType;
        output.status = oldestOrder.status;
    }

    // Update Order Status
    PUBLIC_PROCEDURE_WITH_LOCALS(updateOrder) {
        Order* orderToUpdate = findOrderById(input.orderId);
        if (orderToUpdate == nullptr) {
            output.returnCode = "ORDER_NOT_FOUND";
            return;
        }

        orderToUpdate->status = input.newStatus;
        output.returnCode = "STATUS_UPDATED"; 

        /*[ARojo] Emit some event*/
    }

    // Refund Order
    PUBLIC_PROCEDURE_WITH_LOCALS(refundOrder) {
        Order* orderToRefund = findOrderById(input.orderId);
        if (orderToRefund == nullptr) {
            output.returnCode = "ORDER_NOT_FOUND";
            return;
        }

        orderToRefund->status = "Refunded"; 

        // Perform refund transfer
        if (orderToRefund->amount > 0) {
            qpi.transfer(orderToRefund->user, orderToRefund->amount);
        }

        output.returnCode = "REFUND_PROCESSED"; 
        /*[ARojo] Emit some event*/
    }

};

// Implementation of the helper function to find an Order by ID
Order* QubicOrderContract::findOrderById(uint64_t orderId) {
    std::queue<Order> tempQueue = orderQueue;
    while (!tempQueue.empty()) {
        Order& order = tempQueue.front();
        if (order.orderId == orderId) {
            return &order;  // Return order reference
        }
        tempQueue.pop();
    }
    return nullptr;
}

#endif // QUBIC_ORDER_CONTRACT_H
