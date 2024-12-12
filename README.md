# vottun-qubic-bridge-sc-qubic
Qubic smart contracts for bridge between the Qubic network and Ethereum/Arbitrum allowing for wrapping $Qubic to be bridged to EVM

---

## **Contract Overview**

### **Purpose**
The updated **EthBridge Contract** facilitates secure token bridging between the Qubic network and Ethereum (or Arbitrum) using a **lock-and-mint mechanism**:

1. **From Qubic to Ethereum**: QUBIC tokens are locked in the contract, and equivalent wrapped QUBIC (WQUBIC) tokens are minted on Ethereum.
2. **From Ethereum to Qubic**: WQUBIC tokens are burned on Ethereum, and equivalent QUBIC tokens are unlocked on Qubic.

### **Design Features**
- **Single-contract solution**: Tokens are locked and managed directly in this contract without relying on an external lock contract.
- **Role-based access control**: Admin and Managers ensure secure and decentralized operations.
- **Error Logging**: Implements detailed logging with `EthBridgeLogger` for better visibility into errors during transactions.
- **Enhanced Input Validation**: Prevents invalid operations with robust checks for state and balances.
- **Efficient Stack Usage**: Adheres to Qubic's memory stack restrictions using `*_locals` structures.

---

## **Roles and Permissions**

### **Admin**
- Can set a new admin (`setAdmin`).
- Can add and remove managers (`addManager`, `removeManager`).

### **Managers**
- Authorized to:
  - Confirm bridge orders (`completeOrder`).
  - Refund bridge orders (`refundOrder`).

### **Users**
- Can:
  - Create bridge orders (`createOrder`).
  - Query the status of their orders (`getOrder`).

---

## **Contract State Management**

## **State Variables**

| **Variable**         | **Type**          | **Description**                                                                 |
|-----------------------|-------------------|---------------------------------------------------------------------------------|
| `orders`             | `HashMap`         | Stores all bridge orders using `orderId` as the key.                           |
| `nextOrderId`        | `uint64`          | Tracks the next order ID to assign.                                            |
| `lockedTokens`       | `uint64`          | Total tokens locked in the contract.                                           |
| `transactionFee`     | `uint64`          | Fee required for creating an order.                                            |
| `admin`              | `id`              | Current admin address.                                                         |
| `managers`           | `HashMap`         | Stores manager addresses for access control.                                   |
| `totalReceivedTokens`| `uint64`          | Total tokens received by the contract.                                         |
| `sourceChain`        | `uint32`          | Identifies the source chain (e.g., Qubic).                                |

### **Order Structure**

| **Field**            | **Type**          | **Description**                                                                 |
|-----------------------|-------------------|---------------------------------------------------------------------------------|
| `orderId`            | `uint64`          | Unique ID for the order.                                                       |
| `qubicSender`        | `id`              | Sender's address on the Qubic network.                                         |
| `ethAddress`         | `id`              | Recipient's Ethereum address.                                                  |
| `amount`             | `uint64`          | Amount of tokens to transfer.                                                  |
| `orderType`          | `uint8`           | Type of order (e.g., mint, transfer).                                          |
| `status`             | `uint8`           | Order status (`Created`, `Completed`, or `Refunded`).                          |
| `fromQubicToEthereum`| `bit`             | Direction of the transfer.                                                     |

---

## **Contract Lifecycle**

### **Initialization**
When the contract is deployed, the following values are initialized:
1. **`nextOrderId`**: Set to `0` to start order ID numbering from 0.
2. **`lockedTokens`**: Set to `0`, indicating no tokens are locked initially.
3. **`totalReceivedTokens`**: Set to `0``indicating no tokens are transfered initially to the contract.
3. **`transactionFee`**: Set to `1000` as the initial fee required for creating orders.
4. **`admin`**: Set to the address of the contract invocator (deployer).
5. **`managers`**: Reset to an empty state, allowing admins to add managers as needed.
6. **`sourceChain`**: Set to 0, since QUBIC is no-EVM chain.
---

### **Key Functions and Procedures**

#### 1. `createOrder` (procedure)
- **Purpose**: Creates a new bridge order and locks the specified tokens.
- **Inputs**:
  - `ethAddress`: Destination Ethereum address.
  - `amount`: Amount of tokens to transfer.
  - `fromQubicToEthereum`: Direction of the transfer.
- **Outputs**:
  - `status`: Operation status (`0` = Success, `1` = Invalid amount, `2` = Insufficient transaction fee).
  - **Logging**: Logs success or specific errors (e.g., `invalidAmount`).
- **Backend Returns**:
  - `orderId` for tracking the created order.

#### 2. `getOrder` (Function)
- **Purpose**: Retrieves details of an existing order by `orderId`.
- **Inputs**:
  - `orderId`: ID of the order to retrieve.
- **Outputs**:
  - `status`: Operation status (`0` = Success, `1` = Order not found).
  - `order`: Details of the requested order (`OrderResponse` struct).
  - **Logging**: Logs success or `orderNotFound`.

---

### **Manager Procedures**

#### 3. `completeOrder` (Procedure)
- **Purpose**: Marks an order as completed, releasing tokens to the recipient.
- **Inputs**:
  - `orderId`: ID of the order to complete.
- **Outputs**:
  - `status`: (`0` = Success, `1` = Not found, `2` = Invalid state, `4` = Insufficient tokens).
  - **Logging**: Logs errors like `insufficientLockedTokens`.
- **Logic**:
  - **Qubic to Ethereum**:
    - Verifies sufficient tokens are locked.
    - Updates the locked token balance.
  - **Ethereum to Qubic**:
    - Verifies sufficient tokens are unlocked.
    - Transfers tokens back to the user.
  - Updates the order status to `Completed`.

#### 4. `refundOrder` (Procedure)
- **Purpose**: Refunds an order, returning locked tokens to the sender.
- **Inputs**:
  - `orderId`: ID of the order to refund.
- **Outputs**:
  - `status`: (`0` = Success, `1` = Not found, `2` = Invalid state).
  - **Logging**: Logs `orderNotFound` or successful refund.

---

### **Admin Functions**

#### 5. `setAdmin` (Procedure) 
- **Purpose**: Assigns a new admin to the contract.
- **Inputs**:
  - `address`: New admin's address.
- **Outputs**:
  - `status`: (`0` = Success, `1` = Unauthorized).
  - **Logging**: Logs success or unauthorized access.

#### 6. `addManager` (Procedure)
- **Purpose**: Adds a new manager.
- **Inputs**:
  - `address`: Address of the new manager.
- **Outputs**:
  - `status`: (`0` = Success, `1` = Unauthorized).
  - **Logging**: Logs changes to manager roles.

#### 7. `removeManager` (Procedure)
- **Purpose**: Removes an existing manager.
- **Inputs**:
  - `address`: Address of the manager to remove.
- **Outputs**:
  - `status`: (`0` = Success, `1` = Unauthorized).
  - **Logging**: Logs changes to manager roles.

---

### **Additional Utility Methods**

#### 8. `transferToContract` 
- **Purpose**: Allows a user to transfer tokens to the contract.
- **Inputs**:
  - `amount`: Amount of tokens to transfer.
- **Outputs**:
  - `status`: (`0` = Success, `1` = Invalid amount, `2` = Transfer failed, `3` = Insufficient balance).
  - **Logging**: Logs errors like `transferFailed`.

---

## **Logging System**

### **EthBridgeLogger**
A structured logging mechanism provides detailed error and event visibility. 

| **Field**         | **Type** | **Description**                              |
|--------------------|----------|----------------------------------------------|
| `_contractIndex`  | `uint32` | Index of the contract.                       |
| `_errorCode`      | `uint32` | Specific error code.                         |
| `_orderId`        | `uint64` | ID of the associated order (if applicable).  |
| `_amount`         | `uint64` | Amount involved in the operation.            |
| `_terminator`     | `char`   | Marks the end of logged data.                |

**Example Log Output**:
- **Error**: `onlyManagersCanCompleteOrders` for an unauthorized `completeOrder`.
- **Success**: Logs `orderId` and `amount` upon successful order creation.

---

## **How the Contract Works**

### **1. Initialization**
Upon deployment:
- The `admin` role is assigned to the contract invocator (deployer).
- `nextOrderId` starts at 0.
- `lockedTokens` is initialized to 0.
- `managers` is reset to ensure no initial managers.

### **2. Qubic-to-Ethereum Transfer**
1. **Order Creation**:
   - A user calls `createOrder` with the amount of QUBIC tokens to transfer and the destination Ethereum address.
   - The contract:
     - Validates the input (e.g., amount > 0).
     - Stores the order details in the `orders` map.
   - The method returns a success message along with the order ID.

2. **Order Processing**:
   - The backend middleware detects the new order and processes it:
     - Mints equivalent WQUBIC tokens on Ethereum.
     - Updates the order's state via `transferToContract`+`completeOrder` or `refundOrder`.

3. **Order Completion**:
   - A manager calls `completeOrder` and checks the direction of the transaction:
    - If Ethereum-to-Qubic: releasing the locked tokens.
    - If Qubic-to-Ethereum: checks user has transfered the amount of tokens and lock them.
   - The order's status is updated to `Completed`.

### **3. Refunds**
- If the transfer cannot proceed (e.g., invalid destination), a manager can call `refundOrder` to return the locked tokens to the sender.

---

### **4. Balance and Locked Tokens**

- `totalReceivedTokens`: tracks the total of all tokens sent to the contract, that are not currently in use (locked). It reflects the overall balance the contract has in tokens that are not actively tied to any order.

- `lockedTokens`: represents the amount of the tokens that are actively locked in pending or active orders. It ensures that the contract knows how many tokens are reserved and unavailable for other operations.

**Logic**:

- When a user transfers tokens to the contract (via transferToContract), the `totalReceivedTokens` increases. These tokens are not yet locked until they are associated with an order. This way, the contract keeps track of unused tokens.
- When an order is in the completion process, the required tokens are deducted from the available `totalReceivedTokens` (totalReceivedTokens -= input.amount) and added to lockedTokens (lockedTokens += input.amount).
- If an order is refunded or completed:
    - `lockedTokens` is decreased because the tokens are no longer reserved.
    - `totalReceivedTokens` remains unchanged, as it no longer was considering those 'lockedTokens' in its balance.

---

## **Frontend Requirements**

### **Inputs**
- **User Functions**:
  - `createOrder`: Requires `ethAddress`, `amount`, and `fromQubicToEthereum`.
  - `getOrder`: Requires `orderId`.
- **Admin/Manager Functions**:
  - `setAdmin`: Requires `address`.
  - `addManager`/`removeManager`: Requires `address`.
  - `completeOrder`/`refundOrder`: Requires `orderId`.

### **Outputs**
- All methods return:
  - `status`: Operation result code.
  - **Logging**: Logged events provide error details for debugging.

---

## **Security Considerations**
- **Role Verification**: Ensures only authorized roles (Admin, Managers) perform sensitive operations.
- **Balance Validation**: Verifies user balances and contract token availability before any operation.
- **State Integrity**: Ensures valid state transitions and error handling.
- **Logging**: Captures detailed execution data for traceability.

---

