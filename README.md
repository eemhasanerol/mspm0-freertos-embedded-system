# FreeRTOS-Based Embedded System

Embedded system developed on the TI MSPM0G3507 using C and FreeRTOS.

The system reads data from multiple sensors, connects to the internet through an ESP8266 Wi-Fi module, and displays both local and online data on an ST7789 TFT display.

## Features

- FreeRTOS-based multi-task application
- BME280 temperature, humidity, and pressure measurement
- DS1307 real-time clock (RTC)
- QMC5883L magnetometer for compass heading
- ESP8266 Wi-Fi communication over UART using AT commands
- Wi-Fi credential storage in Flash memory
- Online and Offline operating modes
- Weather and financial data retrieval from a Python backend over HTTP
- ST7789 TFT display for sensor, internet, RTC, and compass data

## Hardware

| Component | Description |
|-----------|-------------|
| TI MSPM0G3507 | Main microcontroller |
| BME280 | Temperature, humidity, and pressure sensor |
| DS1307 | Real-time clock (RTC) |
| QMC5883L | 3-axis magnetometer |
| ESP8266 | Wi-Fi module |
| ST7789 | TFT display |

## Software Architecture

The project is organized into separate layers for application logic, device drivers, hardware-specific functions, and user interface code.

- `APP/` – Application logic, system initialization, and internet data handling
- `Drivers/` – Device drivers for BME280, DS1307, QMC5883L, ESP8266, and ST7789
- `Platform/` – MCU-specific communication and hardware interface functions
- `UI/` – Display screens and compass interface
## Communication Interfaces

- I2C – Communication with BME280, DS1307, and QMC5883L
- UART – Communication with ESP8266 using AT commands
- SPI – Communication with ST7789 TFT display
## Online and Offline Operation

The system supports both online and offline operating modes.

- Wi-Fi credentials are stored in Flash memory and reused after reset.
- The ESP8266 connects to the configured Wi-Fi network using AT commands.
- In Online mode, weather and financial data are retrieved from a Python backend over HTTP.
- In Offline mode, local sensor, RTC, and compass data remain available without an internet connection.
## FreeRTOS Architecture

The application is divided into separate tasks for sensor processing, user interface, Wi-Fi communication, button handling, and system monitoring.

- FreeRTOS tasks are used to separate the main system functions.
- Task notifications are used for communication between interrupts and tasks.
- Event Groups are used to monitor task activity with a software watchdog mechanism.
- Tasks use RTOS delays instead of blocking delays where periodic execution is required.
## System Architecture
```plaintext
 ┌──────────┐
 │  BME280  │──┐
 └──────────┘  │
               │ I2C
 ┌──────────┐  │       ┌────────────────────────────────┐       ┌─────────────┐
 │  DS1307  │──┼──────►│        TI MSPM0G3507          │──SPI─►│   ST7789    │
 └──────────┘  │       │                                │       │ TFT Display │
               │       │  ┌──────────────────────────┐  │       └─────────────┘
 ┌──────────┐  │       │  │        FreeRTOS          │  │
 │ QMC5883L │──┘       │  │                          │  │       ┌─────────────┐
 └──────────┘          │  │ • Application Tasks      │  │─UART─►│   ESP8266   │
                       │  │ • Task Notifications     │  │       │    Wi-Fi    │
 ┌──────────┐          │  │ • Event Groups           │  │       └──────┬──────┘
 │  Button  │─GPIO/IRQ►│  └────────────┬─────────────┘  │              │
 └──────────┘          │               │ Task Health    │         Wi-Fi / HTTP
                       │               ▼                │              │
                       │  ┌──────────────────────────┐  │              ▼
                       │  │   Watchdog/Supervisor    │  │       ┌───────────────┐
                       │  │          Task            │  │       │ Python Backend│
                       │  └────────────┬─────────────┘  │       │ Weather /     │
                       │               │ Feed           │       │ Financial Data│
                       │               ▼                │       └───────────────┘
                       │  ┌──────────────────────────┐  │
                       │  │   Hardware Watchdog     │  │
                       │  │          WWDT            │  │
                       │  └──────────────────────────┘  │
                       └────────────────────────────────┘q
```
## Project Structure
```text
Full_Proje/
├── APP/
│   ├── app_init.c
│   ├── internet_data.c
│   └── main.c
│
├── Drivers/
│   ├── bme280/
│   ├── ds1307/
│   ├── esp8266/
│   ├── qmc5883l/
│   └── st7789/
│
├── Platform/
│   ├── platform.c
│   └── platform.h
│
├── UI/
│   ├── compass_screen.c
│   ├── offline_screen.c
│   └── wifi_screen.c
│
├── freertos/
├── Full_Proje.syscfg
└── mspm0g3507.cmd
## Development Environment  
- TI Code Composer Studio (CCS)
- TI MSPM0 SDK
- SysConfig
- FreeRTOS
- C
## Build and Run
1. Open the project in TI Code Composer Studio.
2. Make sure the MSPM0 SDK and SysConfig are installed.
3. Connect the LP-MSPM0G3507 LaunchPad.
4. Build the project.
5. Flash and run the application on the target board.
