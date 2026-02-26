# SmartStore Ecosystem

An automated, IoT-based retail solution engineered with **ESP32** nodes, **Azure IoT Hub**, and **FreeRTOS**. This project synchronizes real-time weight-based inventory tracking with an automated checkout kiosk.

## Overview

The SmartStore Ecosystem consists of two specialized embedded nodes communicating via the cloud to eliminate manual scanning and streamline the shopping experience.

* **Inventory Node**: Monitors shelf stock in real-time using load cells and sends telemetry to Azure.
* **PayDesk Node**: Receives the "virtual basket" from the cloud and manages the automated checkout process.

---

## Key Features

- **Real-Time Inventory**: Weight-based sensing with ±1g precision using HX711.
- **Event-Driven Telemetry**: Data is sent to the cloud only when stock changes to optimize power and bandwidth.
- **Multitasking Architecture**: Built on **FreeRTOS** to handle sensor sampling and cloud communication concurrently.
- **Cloud Synchronization**: Secure D2C (Device-to-Cloud) and C2D (Cloud-to-Device) messaging via **Azure IoT Hub** and **MQTT**.
- **User Interface**: Live basket updates on a 2.8" TFT display (ILI9341).

---

## System Architecture

1.  **Sensing**: Load cells on the shelf detect when a product is removed.
2.  **Processing**: The Inventory ESP32 calculates the item count based on predefined unit weights.
3.  **Communication**: The count is sent as telemetry to Azure IoT Hub.
4.  **Action**: The PayDesk ESP32 receives the message, updates the display, and waits for the user to "Pay" (physical button trigger).

---

## Hardware Configuration

### 1. Inventory Node (ESP32)
| Component | Data Pin (DT) | Clock Pin (SCK) | Unit Weight (GR) | Product Name |
| :--- | :--- | :--- | :--- | :--- |
| **Load Cell 1** | GPIO 23 | GPIO 22 | 500g | Dorna |
| **Load Cell 2** | GPIO 21 | GPIO 19 | 500g | Coca Cola |
| **Load Cell 3** | GPIO 18 | GPIO 5 | 1000g | Doritos |
| **Load Cell 4** | GPIO 17 | GPIO 16 | 1500g | Kinder |

### 2. PayDesk Node (ESP32 + TFT)
| Component | Pin | Function |
| :--- | :--- | :--- |
| **TFT CS** | GPIO 5 | Chip Select (SPI) |
| **TFT DC** | GPIO 16 | Data/Command |
| **TFT RST** | GPIO 4 | Reset |
| **Button** | GPIO 17 | Payment Confirmation |
| **SPI MOSI** | GPIO 23 | Master Out Slave In |
| **SPI SCK** | GPIO 18 | Serial Clock |

---

## Firmware Logic

The firmware utilizes **FreeRTOS tasks** to ensure high responsiveness:

### Inventory Tasks:
- `inventoryTask`: Handles high-frequency sampling of the load cells and data filtering.
- `device_to_cloud`: Manages the MQTT connection and pushes updates to Azure when a change is detected.

### PayDesk Tasks:
- `get_virtual_basket`: Listens for C2D messages and parses the incoming product strings.
- `pay_desk`: Monitors the hardware interrupt/button for transaction finalization.

---

## Tech Stack

- **Linguaj:** C/C++ (Arduino Framework)
- **RTOS:** FreeRTOS
- **Cloud:** Azure IoT Hub
- **Protocol:** MQTT over TLS
- **Libraries:**
  - `HX711` (Load cell amplifier)
  - `Adafruit_ILI9341` & `Adafruit_GFX` (Display)
  - `PubSubClient` (Messaging)
  - `mbedtls` (Encryption)

---

## etup & Installation

1.  **Azure Configuration**: 
    - Create an IoT Hub and register two devices.
    - Generate SAS tokens or X.509 certificates for authentication.
2.  **Library Installation**: 
    - Install `HX711`, `Adafruit ILI9341`, and `AzureIoTHub` libraries via PlatformIO or Arduino Library Manager.
3.  **Calibration**:
    - Adjust the `SCALE` constant in the Inventory code to match your specific load cell calibration factor.
4.  **Deployment**:
    - Flash the Inventory code to the shelf ESP32.
    - Flash the PayDesk code to the checkout ESP32.

---

## Author
**[raylaj23]** *Embedded Systems Developer | IoT Enthusiast*
