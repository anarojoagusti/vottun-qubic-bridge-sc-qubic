# vottun-qubic-bridge-sc-qubic
Qubic smart contracts for bridge between the Qubic network and Ethereum/Arbitrum allowing for wrapping $Qubic to be bridged to EVM

---

## **Contract Design**

### **Purpose**
The Qubic Bridge Smart Contract facilitates the secure transfer of native QUBIC tokens between the Qubic network and Ethereum. The process employs a **lock and mint mechanism** to maintain token equivalence across networks.

### **Key Features**
1. Handles all bridge operations in a single smart contract (no external lock contract required).
2. Manages token locking, refunds, and state transitions internally.
3. Processes both Qubic-to-Ethereum and Ethereum-to-Qubic transfers seamlessly.
4. Maintains a `HashMap`-based order system for efficient lookups and lifecycle management.

---

## **State Management**

### **Data Structures**
- **`BridgeOrder`**:
  Represents the details of a bridge order.
  - `orderId`: A unique identifier for the order.
  - `qubicSender`: Address of the user initiating the order on Qubic.
  - `ethAddress`: Destination Ethereum address.
  - `amount`: Token amount to transfer.
  - `status`: Current status of the order (e.g., `Created`, `Pending`, `Completed`, `Refunded`).
  - `fromQubicToEthereum`: Indicates the direction of the transfer.
  
- **State Variables**:
  - `lockedTokens`: The total number of QUBIC tokens currently locked in the contract.
  - `transactionFee`: The required fee for initiating an order.
  - `orders`: A `HashMap` for storing orders using `orderId` as the key.

---

## **Method Overview**

| **Method**      | **Type**                  | **Description**                                                                 |
|------------------|---------------------------|---------------------------------------------------------------------------------|
| **`createOrder`**   | `PUBLIC_FUNCTION`         | Creates a new bridge order, locking the specified amount of tokens in the contract. |
| **`getOrder`**      | `PUBLIC_FUNCTION`         | Retrieves details of a specific bridge order by `orderId`.                     |
| **`completeOrder`** | `PUBLIC_PROCEDURE`        | Marks an order as completed, releasing the locked tokens to finalize the transfer. |
| **`refundOrder`**   | `PUBLIC_PROCEDURE`        | Refunds a bridge order, returning the locked tokens to the sender.             |

---

## **Order Statuses**

The smart contract tracks the state of each order using the following statuses:

1. **`Created` (0)**:
   - The order has been created, and tokens have been locked in the contract.

2. **`Completed` (1)**:
   - The tokens have been successfully transferred, and the order is finalized.

3. **`Refunded` (2)**:
   - The tokens have been returned to the user due to a failed or canceled transfer.

---

## **How the Contract Works**

### **1. Qubic-to-Ethereum Transfer**
1. The user calls `createOrder` to initiate a bridge order by specifying the amount of QUBIC tokens to transfer and the destination Ethereum address.
2. The contract locks the tokens and emits a log entry indicating the order creation.
3. After the backend confirms the Ethereum transaction, the `completeOrder` method is called to finalize the transfer and release the locked tokens.

### **2. Ethereum-to-Qubic Transfer**
1. The user sends wrapped tokens (WQUBIC) on Ethereum, initiating a bridge process on the backend.
2. The backend calls the Qubic smart contract to release the corresponding QUBIC tokens to the destination address.
3. The contract updates the order's status to `Completed` upon successful completion of the transfer.

---

## **Notable Changes**
1. **Token Locking**: 
   - Tokens are now locked directly in the bridge contract instead of using an external lock contract, simplifying the architecture.
2. **Removed Event Emissions**:
   - The contract no longer emits events for integration with Redis Pub/Sub, relying instead on backend polling.
3. **HashMap Integration**:
   - The order management system now uses a `HashMap` for efficient order lookups and updates.

---

## **Future Enhancements**
- Introduce mechanisms for dynamic fee adjustments based on network conditions.
- Enhance security to prevent edge cases and ensure seamless operation.

