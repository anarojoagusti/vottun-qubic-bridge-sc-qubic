# vottun-qubic-bridge-sc-qubic
Qubic smart contracts for bridge between the Qubic network and Ethereum/Arbitrum allowing for wrapping $Qubic to be bridged to EVM

---

# Project Structure Overview

The project is structured to simplify the organization and configuration of the required components:

1. **`contracts` folder in the project root:**  
   Contains the `QubicOrderContract.h` file, which replicates the smart contract content included in the `core` submodule. This allows for easy localization and readability of the contract if only its content needs to be reviewed.

2. **`core` submodule:**  
   Points to the `votundev/core` fork. Within the `core` folder, the `contracts` directory includes the `EthBridge.h` smart contract.

3. **Configuration parameters:**

   - **`public_settings.h`:**
     - **Line 94:**  
       ```cpp
       #define TESTNET_EPOCH_DURATION 1000
       ```
       This defines the epoch duration. For a testnet, the epoch will change after 1,000 ticks. On a weak machine, you can set a maximum of 5,000 ticks.

     - **Epoch and tick settings:**  
       ```cpp
       #define EPOCH 136
       #define TICK 17420000
       ```
       The epoch and initial tick are defined. Adjustments may be needed depending on the use case.

   - **`qubic.cpp`:**
     - **Port configuration:**  
       ```cpp
       #define PORT 31844
       ```
       Sets the port for starting the node. In this case, the echo script runs on port 31844.

   - **`private_settings.h`:**
     - **Private keys:**  
       ```cpp
       static unsigned char computorSeeds[][55 + 1] = { };
       ```
       Here you need to set the private keys (indices). There are a total of 676 keys in the Qubic network.

     - **Node peers:**  
       ```cpp
       static const unsigned char knownPublicPeers[][4] = {
           {127, 0, 0, 1}, // REMOVE THIS ENTRY AND REPLACE IT WITH YOUR OWN IP ADDRESSES
       };
       ```
       Replace the placeholder IP address with your node's IP address and the echo server's IP address.

4. **Including the contract in `contract_core/contract_def.h`:**  
   Add the following definitions behind `#include "contracts/QVAULT.h"`:
   ```cpp
   #undef CONTRACT_INDEX
   #undef CONTRACT_STATE_TYPE
   #undef CONTRACT_STATE2_TYPE

   #define ETHBRIDGE_CONTRACT_INDEX 11
   #define CONTRACT_INDEX ETHBRIDGE_CONTRACT_INDEX
   #define CONTRACT_STATE_TYPE ETHBRIDGE
   #define CONTRACT_STATE2_TYPE ETHBRIDGE2
   #include "contracts/EthBridge.h"
    ```
5. **Compilation instructions**:
Use Visual Studio in Release mode for compilation. Note that releaseAVX512 will not work in a virtual machine environment.
