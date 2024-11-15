#ifndef QUBIC_ORDER_CONTRACT_H
#define QUBIC_ORDER_CONTRACT_H

#pragma once

#include <string>
#include <cstdint>
#include <queue>
#include <map>
#include <optional>

using namespace QPI;

// Order structure
struct BridgeOrder {
    uint64_t orderId;
    std::string qubicSender;
    std::string ethAddress;  // Destination address on Ethereum
    uint64_t amount;
    std::string orderType;  /*[ARojo](mint? transfer? lock?)*/
    std::string status; /*[ARojo]string or enum? need agreement on states: "Created", "Pending", "Completed", "Refunded"?*/
    std::string ethTxHash;   // Hash for tracking
    bool fromQubicToEthereum;   // [ARojo] Direction of transfer: ETH->QUBIC || QUBIC ->ETH
};

// QubicOrderContract definition
class QubicOrderContract {
public:
    // Mapping to store bridge orders by ID/Hash
    std::map<uint32_t, BridgeOrder> bridgeOrders;
    uint32_t nextOrderId = 1;
    std::queue<BridgeOrder> orderQueue; // FIFO queue for order processing 

    // Register functions and procedures
    REGISTER_USER_FUNCTIONS_AND_PROCEDURES {
        //Procedures
        REGISTER_PROCEDURE(createBridgeOrder);
        REGISTER_PROCEDURE(pushBridgeOrder);
        REGISTER_PROCEDURE(pullBridgeOrder);
        REGISTER_PROCEDURE(updateBridgeOrder);
        REGISTER_PROCEDURE(refundBridgeOrder);
        REGISTER_PROCEDURE(burnAmount);       
        REGISTER_PROCEDURE(initiateTransferToEthereum);
        REGISTER_PROCEDURE(confirmTransferToEthereum);
        REGISTER_PROCEDURE(initiateTransferToQubic);
        REGISTER_PROCEDURE(confirmTransferToQubic);
        //Function
        REGISTER_FUNCTION(getBridgeOrder); 

    // Procedure to create a new bridge order
    PUBLIC_PROCEDURE_WITH_LOCALS(createBridgeOrder) {
        // Create a new bridge order
        BridgeOrder newOrder;
        newOrder.orderId = nextOrderId++;
        newOrder.qubicSender = qpi.invocator(); // Automatically capture the sender address
        newOrder.ethDestination = input.ethDestination;
        newOrder.amount = input.amount;
        newOrder.status = "Pending";
        newOrder.ethTxHash = ""; //[ARojo] must be empty initially - guidelines
        newOrder.fromQubicToEthereum = input.fromQubicToEthereum;

        // Store the order in the mapping
        bridgeOrders[newOrder.orderId] = newOrder;
        orderQueue.push(newOrder.orderId);

        output.status = "ORDER_CREATED";
        /*[ARojo] Emit some event*/
    }

    // Push Order to the FIFO Queue
    PUBLIC_PROCEDURE_WITH_LOCALS(pushBridgeOrder) {
        auto it = bridgeOrders.find(input.orderId);
        if (it == bridgeOrders.end() || it->second.status != "Pending") {
            output.returnCode = "ORDER_NOT_FOUND_OR_INVALID";
            return;
        }

        orderQueue.push(it->second.orderId); // Push the order ID to the queue
        output.status = "ORDER_PUSHED";

        /*[ARojo] Emit some event*/
    }

    // Pull the oldest Order from the FIFO Queue
    PUBLIC_PROCEDURE_WITH_LOCALS(pullBridgeOrder) {
        auto it = bridgeOrders.find(input.orderId);
        if (it == bridgeOrders.end()) {
            output.returnCode = "ORDER_NOT_FOUND";
            return;
        }

        BridgeOrder& order = it->second; // Retrieve the specific order by ID
        output.orderId = order.orderId;
        output.qubicSender = order.qubicSender;
        output.ethDestination = order.ethDestination;
        output.amount = order.amount;
        output.status = "ORDER_RETRIEVED";

        //[ARojo] need to process order info to execute action
        /*[ARojo] Emit some event*/
    }

    // Update Order Status
    PUBLIC_PROCEDURE(updateBridgeOrder) {
        auto it = bridgeOrders.find(input.orderId);
        if (it == bridgeOrders.end()) {
            output.returnCode = "ORDER_NOT_FOUND";
            return;
        }

        it->second.status = input.newStatus;
        output.returnCode = "ORDER_UPDATED";

        /*[ARojo] Emit some event*/
    }

    // Refund Order
    PUBLIC_PROCEDURE(refundBridgeOrder) {
        auto it = bridgeOrders.find(input.orderId);
        if (it == bridgeOrders.end() || it->second.status == "Refunded") {
            output.returnCode = "ORDER_NOT_FOUND_OR_ALREADY_REFUNDED";
            return;
        }

        if(it->second.amount >0){
            qpi.transfer(it->second.qubicSender, it->second.amount); // Perform the refund
            it->second.status = "Refunded";
        }

        output.returnCode = "REFUND_PROCESSED";
        /*[ARojo] Emit some event*/
    }

    // initiate a transfer from Qubic to Ethereum
    PUBLIC_PROCEDURE_WITH_LOCALS(initiateTransferToEthereum) {
        auto it = bridgeOrders.find(input.orderId);
        if (it == bridgeOrders.end() || !it->second.fromQubicToEthereum) {
            output.returnCode = "ORDER_NOT_FOUND_OR_INVALID";
            return;
        }

        it->second.status = "InProgress";
        /*[ARojo] Emit some event*/

        output.returnCode = "TRANSFER_INITIATED";
    }

    // Confirm a transfer from Qubic to Ethereum
    PUBLIC_PROCEDURE_WITH_LOCALS(confirmTransferToEthereum) {
        auto it = bridgeOrders.find(input.orderId);
        if (it == bridgeOrders.end() || it->second.status != "InProgress") {
            output.returnCode = "ORDER_NOT_FOUND_OR_INVALID";
            return;
        }

        it->second.ethTxHash = input.ethTxHash;
        it->second.status = "Success";



        output.returnCode = "TRANSFER_CONFIRMED";
    }

    // Procedure to initiate a transfer from Ethereum to Qubic
    PUBLIC_PROCEDURE_WITH_LOCALS(initiateTransferToQubic) {
        auto it = bridgeOrders.find(input.orderId);
        if (it == bridgeOrders.end() || it->second.fromQubicToEthereum) {
            output.returnCode = "ORDER_NOT_FOUND_OR_INVALID";
            return;
        }

        it->second.status = "InProgress";
        /*[ARojo] Emit some event*/

        output.returnCode = "TRANSFER_INITIATED";
    }

        // Procedure to confirm a transfer from Ethereum to Qubic
    PUBLIC_PROCEDURE_WITH_LOCALS(confirmTransferToQubic) {
        auto it = bridgeOrders.find(input.orderId);
        if (it == bridgeOrders.end() || it->second.status != "InProgress") {
            output.returnCode = "ORDER_NOT_FOUND_OR_INVALID";
            return;
        }

        it->second.status = "Success";
        qpi.transfer(it->second.qubicSender, it->second.amount); // Transfer the tokens to the user's wallet on Qubic

        /*[ARojo] Emit some event*/

        output.returnCode = "TRANSFER_CONFIRMED";
    }

    // Function to get the details of a bridge order
    PUBLIC_FUNCTION_WITH_LOCALS(getBridgeOrder) {
        auto it = bridgeOrders.find(input.orderId);
        if (it == bridgeOrders.end()) {
            output.returnCode = "ORDER_NOT_FOUND";
            return;
        }

        output.orderId = it->second.orderId;
        output.qubicSender = it->second.qubicSender;
        output.ethDestination = it->second.ethDestination;
        output.amount = it->second.amount;
        output.status = it->second.status;
        output.ethTxHash = it->second.ethTxHash;
        output.fromQubicToEthereum = it->second.fromQubicToEthereum;
    }


    // Procedure to burn the tokens after confirmation on Ethereum
    PUBLIC_PROCEDURE_WITH_LOCALS(burnAmount) {
        auto it = bridgeOrders.find(input.orderId);
        if (it == bridgeOrders.end() || it->second.status != "Success") {
            output.returnCode = "ORDER_NOT_FOUND_OR_INVALID_STATUS";
            return;
        }

        // Burn the tokens and update the order status
        it->second.status = "Burned";
        qpi.burn(it->second.amount);

        output.returnCode = "AMOUNT_BURNED";

        // [ARojo] Emit an event indicating tokens are burned
    }

};

#endif // QUBIC_ORDER_CONTRACT_H
