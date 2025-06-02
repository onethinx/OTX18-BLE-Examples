# Onethinx BLE Example

This basic example shows how to use Bluetooth with the OTX-18 module.

## Requirements:
- Visual Studio Code
- [OTX-Maestro >= 1.0.2](https://github.com/onethinx/OTX-Maestro)

## Description

Bluetooth Low Energy (BLE) is used in very low-power network and Internet of Things (IoT) solutions using low-cost battery operated devices that can quickly connect and form simple wireless links. Target applications include HID, remote controls, sports and fitness monitors, portable medical devices and smart phone accessories, among many others.

PSoC 6 BLE Middleware contains a comprehensive API to configure the BLE Stack and the underlying chip hardware. PSoC 6 BLE Middleware incorporates a Bluetooth Core Specification v5.0 compliant protocol stack. You may access the GAP, GATT and L2CAP layers of the stack using the API.

The example uses PSoC Creator to make it easy to configure the PSoC 6 BLE Middleware. The BLE resource supports numerous GATT-based Profiles and Services. Each of these can be configured as either a GATT Client or a GATT Server. It generates all the necessary code for a particular Profile/Service operation, as configured in the PSoC Creator.

This example demonstrates the implementation of a Peripheral GAP role and a GATT Server for exchanging data with a host. Additionally, the Device Information Service is used as an example to demonstrate how to configure the BLE service characteristics in the BLE Component.

The [Windows-based Host is found here](https://github.com/onethinx/BLE-example-host) and works with common BLE dongles and (built-in) adapters.

## UART Connection (for debug or BLE host communication)

To communicate with the module over UART (e.g., for debug or BLE host interaction), connect the OTX-18 module to your PC using a USB-to-Serial adapter:

| OTX-18 Pin | USB-to-Serial Adapter/PC | Description       |
|------------|--------------------------|-------------------|
| P9_1       | RX                       | UART TX from module to PC |
| P9_0       | TX                       | UART RX to module from PC |
| GND        | GND                      | Shared ground     |

**Serial settings:**
- Baud rate: `115200`
- Data bits: `8`
- Stop bits: `1`
- Parity: `None`

You can use any terminal software (e.g. PuTTY, Tera Term, or VS Code Serial Monitor) to monitor or interact with the BLE example over UART.

