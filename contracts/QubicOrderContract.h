#pragma once
//std libs - check typedef implementations
#include <string>
#include <cstdint>
#include <queue>
#include <map>
#include <optional>
//Libs for redis pub/sub 
#include <sstream>
#include <winsock2.h> 

#pragma comment(lib, "ws2_32.lib")
#include "../core/src/contracts/qpi.h"

using namespace QPI;

// Order structure
struct BridgeOrder {
    uint64_t orderId;
    std::string qubicSender;
    std::string ethAddress;
    uint64_t amount;
    std::string orderType;
    std::string status;
    std::string ethTxHash;
    bool fromQubicToEthereum;
};

struct OrderReceived {
    std::string orderId;
    std::string originAccount;
    std::string destinationAccount;
    std::string amount;
    std::string memo;
    uint32_t sourceChain;
};

// Contract definition
struct QubicOrderContract : public ContractBase {
    // State definition
    struct State {
        std::map<uint32_t, BridgeOrder> bridgeOrders;
        uint32_t nextOrderId = 1;
        std::queue<uint32_t> orderQueue;
        uint64_t transactionFee = 1000;  // Fee in QUBIC tokens
    };

    // Define state
    State state;

    // Procedure to create a new bridge order
    PUBLIC_PROCEDURE_WITH_LOCALS(createOrder) {
        if (qpi.invocationReward() < state.transactionFee) {
            output.returnCode = "INSUFFICIENT_FEE";
            return;
        }

        qpi.burn(state.transactionFee);  // Burn the fee

        // Create a new bridge order
        BridgeOrder newOrder;
        newOrder.orderId = state.nextOrderId++;
        newOrder.qubicSender = qpi.invocator();
        newOrder.ethAddress = input.ethAddress;
        newOrder.amount = input.amount;
        newOrder.orderType = input.orderType;
        newOrder.status = "Pending";
        newOrder.ethTxHash = "";
        newOrder.fromQubicToEthereum = input.fromQubicToEthereum;

        // Store the order
        state.bridgeOrders[newOrder.orderId] = newOrder;
        state.orderQueue.push(newOrder.orderId);

        output.status = "ORDER_CREATED";
    }

    // Push order to queue
    PUBLIC_PROCEDURE_WITH_LOCALS(pushBridgeOrder) {
        auto it = state.bridgeOrders.find(input.orderId);
        if (it == state.bridgeOrders.end() || it->second.status != "Pending") {
            output.returnCode = "ORDER_NOT_FOUND_OR_INVALID";
            return;
        }

        state.orderQueue.push(it->first);
        output.status = "ORDER_PUSHED";
    }

    // Pull oldest order from queue
    PUBLIC_PROCEDURE_WITH_LOCALS(pullBridgeOrder) {
        if (state.orderQueue.empty()) {
            output.returnCode = "QUEUE_EMPTY";
            return;
        }

        uint32_t orderId = state.orderQueue.front();
        state.orderQueue.pop();

        auto it = state.bridgeOrders.find(orderId);
        if (it == state.bridgeOrders.end()) {
            output.returnCode = "ORDER_NOT_FOUND";
            return;
        }

        const BridgeOrder& order = it->second;
        output.orderId = order.orderId;
        output.qubicSender = order.qubicSender;
        output.ethAddress = order.ethAddress;
        output.amount = order.amount;
        output.status = "ORDER_RETRIEVED";
    }

    // Update order status
    PUBLIC_PROCEDURE(updateBridgeOrder) {
        auto it = state.bridgeOrders.find(input.orderId);
        if (it == state.bridgeOrders.end()) {
            output.returnCode = "ORDER_NOT_FOUND";
            return;
        }

        it->second.status = input.newStatus;
        output.returnCode = "ORDER_UPDATED";
    }

    // Refund order
    PUBLIC_PROCEDURE(refundBridgeOrder) {
        auto it = state.bridgeOrders.find(input.orderId);
        if (it == state.bridgeOrders.end() || it->second.status == "Refunded") {
            output.returnCode = "ORDER_NOT_FOUND_OR_ALREADY_REFUNDED";
            return;
        }

        if (it->second.amount > 0) {
            qpi.transfer(it->second.qubicSender, it->second.amount);
            it->second.status = "Refunded";
        }

        output.returnCode = "REFUND_PROCESSED";
    }

    // Function to retrieve order details
    PUBLIC_FUNCTION_WITH_LOCALS(getBridgeOrder) {
        auto it = state.bridgeOrders.find(input.orderId);
        if (it == state.bridgeOrders.end()) {
            output.returnCode = "ORDER_NOT_FOUND";
            return;
        }

        const BridgeOrder& order = it->second;
        output.orderId = std::to_string(order.orderId);
        output.originAccount = order.qubicSender;
        output.destinationAccount = order.ethAddress;
        output.amount = std::to_string(order.amount);
        output.memo = "Bridge Order";
        output.sourceChain = order.fromQubicToEthereum ? 1 : 0;
    }

    // Procedure to burn tokens after successful transaction
    PUBLIC_PROCEDURE_WITH_LOCALS(burnAmount) {
        auto it = state.bridgeOrders.find(input.orderId);
        if (it == state.bridgeOrders.end() || it->second.status != "Success") {
            output.returnCode = "ORDER_NOT_FOUND_OR_INVALID_STATUS";
            return;
        }

        qpi.burn(it->second.amount);
        it->second.status = "Burned";
        output.returnCode = "AMOUNT_BURNED";
    }

    // Register functions and procedures
    REGISTER_USER_FUNCTIONS_AND_PROCEDURES{
        REGISTER_PROCEDURE(createOrder);
        REGISTER_PROCEDURE(pushBridgeOrder);
        REGISTER_PROCEDURE(pullBridgeOrder);
        REGISTER_PROCEDURE(updateBridgeOrder);
        REGISTER_PROCEDURE(refundBridgeOrder);
        REGISTER_PROCEDURE(burnAmount);
        REGISTER_FUNCTION(getBridgeOrder);
    }
};





