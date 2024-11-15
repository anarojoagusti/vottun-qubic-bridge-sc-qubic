# vottun-qubic-bridge-sc-qubic
Qubic smart contracts for bridge between the Qubic network and Ethereum/Arbitrum allowing for wrapping $Qubic to be bridged to EVM

---

## **Contract Design**

### **Purpose**
The Qubic Bridge Smart Contract provides functionality for:
1. Initiating bridge orders to transfer tokens to Ethereum.
2. Managing order states, such as updating with transaction hashes or refunds.
3. Handling token burns and confirmations during the transfer process.
4. Supporting both Qubic-to-Ethereum and Ethereum-to-Qubic bridging.

---

## **Method Overview**

Each method serves a specific function in the lifecycle of a bridge order. The following table explains their purpose, type, and behavior:

| **Method**                      | **Type**                  | **Description**                                                                                   |
|----------------------------------|---------------------------|---------------------------------------------------------------------------------------------------|
| **`getBridgeOrder`**             | `PUBLIC_FUNCTION`         | Retrieves details of a specific bridge order based on its `orderId`.                             |
| **`createBridgeOrder`**          | `PUBLIC_PROCEDURE_WITH_LOCALS` | Creates a new bridge order, initializing it with the sender's Qubic address, destination Ethereum address, and amount. |
| **`updateBridgeOrder`**          | `PUBLIC_PROCEDURE`        | Updates the status and Ethereum transaction hash of an existing bridge order.                    |
| **`refundBridgeOrder`**          | `PUBLIC_PROCEDURE`        | Marks an order as refunded and returns the retained tokens to the user.                          |
| **`pushBridgeOrder`**            | `PUBLIC_PROCEDURE_WITH_LOCALS` | Adds a new order to the processing queue and prepares it for further action.                      |
| **`pullBridgeOrder`**            | `PUBLIC_PROCEDURE_WITH_LOCALS` | Retrieves an order from the queue by its `orderId` for processing or completion.                 |
| **`initiateTransferToEthereum`** | `PUBLIC_PROCEDURE_WITH_LOCALS` | Starts the process of transferring tokens to Ethereum by locking the Qubic tokens.              |
| **`confirmTransferToEthereum`**  | `PUBLIC_PROCEDURE_WITH_LOCALS` | Finalizes the Qubic-to-Ethereum transfer after the tokens have been successfully pushed to Ethereum. |
| **`initiateTransferToQubic`**    | `PUBLIC_PROCEDURE_WITH_LOCALS` | Begins the process of transferring tokens from Ethereum back to Qubic.                          |
| **`confirmTransferToQubic`**     | `PUBLIC_PROCEDURE_WITH_LOCALS` | Completes the Ethereum-to-Qubic transfer by unlocking the Qubic tokens.                         |
| **`burnAmount`**                 | `PUBLIC_PROCEDURE_WITH_LOCALS` | Burns tokens retained in the contract after a successful transfer to Ethereum.                   |

---

## **State Management**

### **Data Structures**
- **`BridgeOrder`**: Represents an order's details, including:
  - `orderId`: Unique ID for the order. Currently a counter.
  - `qubicSender`: Address of the user initiating the order on Qubic (invocator)
  - `ethDestination`: Ethereum address for token reception.
  - `amount`: Token amount to be transferred.
  - `status`: Current state of the order (e.g., "Pending", "Refunded", "Burned").
  - `ethTxHash`: Ethereum transaction hash (initially empty when created).
  - `fromQubicToEthereum`: Transfer direction;

- **Queue Management**: RN Orders are stored in a queue for processing. Each method interacts with the queue to manage the order lifecycle.

---

## **How the Contract Works**

### **1. Qubic-to-Ethereum Transfer**
1. User calls `createBridgeOrder` to initiate a bridge order.
2. The backend detects the order and triggers `pushBridgeOrder`.
3. Tokens are locked on Qubic via `initiateTransferToEthereum`.
4. After tokens are minted on Ethereum, `confirmTransferToEthereum` is called to finalize the process.
5. Retained tokens are burned using `burnAmount`.

### **2. Ethereum-to-Qubic Transfer**
1. User sends wrapped tokens on Ethereum, initiating `initiateTransferToQubic`.
2. The backend triggers `pushBridgeOrder` to prepare the Qubic transfer.
3. Tokens are unlocked on Qubic via `confirmTransferToQubic`.

---

## **Future Enhancements**
- Implement event emissions for better backend tracking.
- Add security measures to handle edge cases and prevent misuse.
- Optimize queue processing logic for higher throughput.
- Add hash's order processing logic to call procedures instead of using counter-based orderID


