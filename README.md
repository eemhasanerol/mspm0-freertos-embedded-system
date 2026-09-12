# ⚙️ MSPM0G3507 FreeRTOS-Based Embedded System

A real-time embedded system developed on the TI MSPM0G3507 using FreeRTOS and Embedded C.

The system integrates sensors, Wi-Fi communication, an RTC, a magnetometer, and a TFT display. It operates in both online and offline modes depending on Wi-Fi availability.

---

## 🧩 System Features

- **FreeRTOS** – Task-based system architecture and task management
- **BME280** – Temperature, humidity, and pressure measurement
- **DS1307** – Real-time clock and date information
- **QMC5883L** – Compass and heading measurement
- **ESP8266** – Wi-Fi connectivity and HTTP communication
- **ST7789** – TFT display interface
- **Flash Memory** – Persistent storage of Wi-Fi credentials
- **Watchdog** – System recovery in case of a lockup

---

## ⚙️ FreeRTOS Design

The application is divided into separate tasks for sensors, display, Wi-Fi, buttons, and system monitoring.

- Task priority management
- Stack size configuration
- Task Notifications for task synchronization
- Hardware watchdog for system monitoring

---

## 🔌 Peripheral Communication

| Device | Interface | Purpose |
|--------|-----------|---------|
| BME280 | I2C | Temperature, humidity, and pressure |
| DS1307 | I2C | Date and time |
| QMC5883L | I2C | Compass heading |
| ESP8266 | UART | Wi-Fi and HTTP communication |
| ST7789 | SPI | TFT display |
| Buttons | GPIO | User input |

Drivers for the BME280, DS1307, and QMC5883L were developed using their datasheets. Callback functions are used to keep the sensor drivers independent of MCU-specific I2C code.

---

## 🌐 Online / Offline Operation

### Offline Mode

When Wi-Fi is unavailable, the system continues to provide:

- Temperature, humidity, and pressure data
- Date and time
- Compass heading

### Online Mode

When Wi-Fi is available, the system also retrieves:

- Weather information
- Financial data

The ESP8266 communicates with the MSPM0G3507 over UART using AT commands. HTTP responses are received and parsed on the MCU before the data is displayed on the ST7789 TFT.

---

## 🌍 Web Service

A Python-based web service was developed to collect weather and financial data from different APIs.

The data is combined and served through PythonAnywhere, allowing the embedded system to retrieve the required information with a single HTTP request.

---

## 💾 Flash Storage

Wi-Fi SSID and password information are stored in Flash memory.

The stored credentials are loaded after a system restart, so the Wi-Fi information does not need to be entered again.

---

## 🔧 Debugging and Development

During development:

- FreeRTOS task stack usage was monitored and stack sizes were adjusted where necessary.
- I2C communication problems were analyzed using a logic analyzer.
- QMC5883L hard-iron offsets were determined and applied for compass calibration.
- Small heading changes were filtered to reduce movement of the compass indicator.

---

## 🔨 Build Info

- **MCU:** TI MSPM0G3507
- **Core:** ARM Cortex-M0+
- **Language:** Embedded C
- **RTOS:** FreeRTOS
- **IDE:** Code Composer Studio
- **Wi-Fi Module:** ESP8266
- **Display:** ST7789 TFT

---

## 👤 Author

**Hasan Erol**  
Embedded Software / Firmware

MSPM0 • FreeRTOS • Embedded C • I2C • SPI • UART

📧 **eem.hasanerol@gmail.com**  
🔗 [github.com/eemhasanerol](https://github.com/eemhasanerol)
