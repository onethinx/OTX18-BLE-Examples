# OTX18-BLE-Examples

This repository provides two example projects demonstrating **Bluetooth Low Energy (BLE)** functionality using the **Onethinx OTX-18** module:

1. **BLE Scanner** - Scans and lists available BLE advertisements.
2. **BLE GATT Communication** - Establishes communication with a BLE peripheral using GATT.

## Requirements

- **Hardware**
  - Onethinx OTX-18 module
  - Debugger (e.g., MiniProg4)
  - BLE-enabled devices for testing

- **Software**
  - [Visual Studio Code](https://code.visualstudio.com/)
  - [OTX-Maestro Extension](https://github.com/onethinx/OTX-Maestro)
  - [PSoC Creator (optional)](https://www.infineon.com/cms/en/design-support/tools/sdk/psoc-creator/) for chip configuration

---

# 1️⃣ BLE Scanner Example

## Overview

This example demonstrates how to use the **Onethinx OTX-18** module as a **BLE scanner**, continuously scanning for advertisements from nearby BLE devices.

### Features:
- Initiates BLE scanning.
- Detects advertising packets from surrounding BLE devices.
- Displays **device name**, **MAC address**, **advertisement type**, and **signal strength (RSSI)**.

### How It Works:
1. The OTX-18 module initializes BLE scanning.
2. It continuously listens for BLE advertisements.
3. Upon detecting a BLE device, it extracts **advertisement data**.
4. The data is printed via UART.

### Output Example:

```
NEW DEVICE FOUND 
R: eventType = 3 
R: peerAddrType = 1 
R: peerBdAddr = C7:9F:2B:1B:14:63 
Advertising Data: 
  Length = 7, AD Type = 0xFF 
  Manufacturer: Apple, Inc. (Company ID: 0x004C) 
  Data: 12 02 00 00 B3  
R: RSSI = -77 
```

### Running the Example:
1. **Open the BLE Scanner project** in **VS Code**.
2. **Compile and flash the firmware** onto the OTX-18 module.
3. **Monitor the serial output** to view detected devices.

---

# 2️⃣ BLE GATT Communication Example

## Overview

This example demonstrates how the **Onethinx OTX-18** module acts as a **BLE central device**, connecting to a **BLE peripheral** using the **GATT (Generic Attribute Profile)** protocol.

### Features:
- Scans for a specific **BLE peripheral**.
- Establishes a connection with the target device.
- Reads and writes **GATT characteristics**.

### How It Works:
1. The OTX-18 module starts scanning for a target BLE peripheral (e.g., a sensor or another Onethinx module).
2. Once found, it establishes a BLE connection.
3. It reads data from a **GATT characteristic** (e.g., temperature sensor value).
4. Optionally, it writes data to a characteristic (e.g., turning on an LED).

### Example: Reading a Temperature Sensor

```
Connected to BLE Device: Temp_Sensor_001
Reading GATT Characteristic UUID: 0x2A6E (Temperature)
Received Value: 23.5°C
```

### Example: Writing to an LED Characteristic

```
Connected to BLE Device: LED_Controller
Writing GATT Characteristic UUID: 0x2A56 (LED Control)
Value Sent: ON
```

### Running the Example:
1. **Open the BLE GATT project** in **VS Code**.
2. **Modify the target device name or MAC address** in the source code.
3. **Compile and flash the firmware** onto the OTX-18 module.
4. **Monitor the serial output** for connection status and data exchange.

---

## ⚙️ Setup Instructions (Common for Both Examples)

### 1. Install Required Tools
Follow these steps to set up your development environment:
- Install **Visual Studio Code**.
- Install the **OTX-Maestro extension**.
- (Optional) Install **PSoC Creator** if chip reconfiguration is needed.

### 2. Configure and Build in VS Code
- Open the project folder (`BLE_Scanner` or `BLE_GATT`).
- Click **"Clean-Reconfigure"** in the status bar.
- Click **"Build-And-Launch"** to flash and debug.

### 3. Monitor Output
Use a **serial terminal** (e.g., PuTTY, Termite) to view the BLE scan results or GATT communication logs.

---

## 🐝 License

This project is licensed under the **MIT License**.

---

## 🔗 Useful Resources
- [Onethinx Documentation](https://github.com/onethinx/)
- [BLE Protocol Overview](https://www.bluetooth.com/specifications/gatt/)
- [PSoC Creator & BLE Middleware](https://www.infineon.com/cms/en/design-support/tools/sdk/psoc-creator/)

---

This documentation follows the structure of **Onethinx OTX18-Project-Examples** to ensure consistency. Let me know if you need any modifications! 🚀

