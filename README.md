# 🚀 OTX18-BLE-Examples 🚀

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

| Project | Description                                                                                     |
|------|-------------------------------------------------------------------------------------------------|
| 1    | [This example demonstrates how to use the Onethinx OTX-18 module as a BLE scanner, continuously scanning for advertisements from nearby BLE devices.](BLE-scan-example)                                    |
| 2    | [This example demonstrates how the Onethinx OTX-18 module acts as a BLE central device, connecting to a BLE peripheral using the GATT (Generic Attribute Profile) protocol.](BLE-example)  |


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

