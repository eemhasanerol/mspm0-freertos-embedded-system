# ⚙️ STM32F4 Bare-Metal Drivers

Bare-metal peripheral drivers developed for the STM32F407VG in Embedded C, without using HAL or LL libraries.

The project focuses on understanding STM32 peripherals at register level and building modular, reusable driver APIs.

---

## 🧩 Implemented Drivers

- **RCC** – Clock configuration and peripheral clock control
- **GPIO** – GPIO configuration and alternate function support
- **SysTick** – Millisecond tick and delay functions
- **EXTI** – External interrupt configuration and handling
- **I2C** – Master communication and interrupt handling
- **SPI** – Master communication
- **USART** – Serial communication

---

## 🔧 Driver Design

- Handle-based peripheral configuration
- Enum-based configuration options
- Status and error handling through return values
- Configurable timeout for blocking operations
- Interrupt support for GPIO/EXTI and I2C

---

## 🧠 Example Projects

The `/Examples` directory contains simple applications used to test the drivers on real hardware.

| Example | Description |
|---------|-------------|
| `GPIO_LED_Toggle` | Toggles LEDs using a SysTick-based delay. |
| `EXTI_Button_LED` | Controls an LED using an external button interrupt. |
| `I2C_DeviceID_Read` | Reads a device ID register over I2C. |
| `SPI_DeviceID_Read` | Reads a device register over SPI. |

---

## 🗂 Project Structure

```text
stm32f4-baremetal-drivers/
│
├── Core/                 # Startup code and SysTick
├── Drivers/              # Peripheral drivers
├── Examples/             # Example applications
└── README.md
```

---

## 🔨 Build Info

- **MCU:** STM32F407VG
- **Core:** ARM Cortex-M4
- **Language:** Embedded C
- **IDE:** STM32CubeIDE
- **Programming:** Bare-Metal / Register-Level
- **HAL/LL:** Not used

---

## 👤 Author

**Hasan Erol**  
Embedded Software / Firmware

STM32 • ARM Cortex-M • Bare-Metal • Embedded C

📧 **eem.hasanerol@gmail.com**  
🔗 [github.com/eemhasanerol](https://github.com/eemhasanerol)
