# DC Power Monitor v1 — ESP32

A three-channel DC power monitoring prototype built with an ESP32 DevKit-32, an INA3221 sensor module, and a 1.3-inch ST7789 TFT display.

This project is the **first version of a larger power monitoring system**. It focuses on microcontroller-based data acquisition, sensor communication, and real-time visualization.

The next version, **DC Power Monitor v2 — Orange Pi**, is planned to extend the system with wireless data transfer, a Linux-based server, measurement history, and a web dashboard.

## Project Architecture

```text
       INA3221 (I2C)
       3 measurement channels
              |
              v
        ESP32 DevKit-32
              |
              v
        ST7789 TFT (SPI)
        240 x 240 pixels

      USB Serial Diagnostics
```

The ESP32 reads voltage and shunt measurements from the INA3221, calculates current and power, and updates the local display every second.

## Hardware

| Component | Purpose |
|---|---|
| ESP32 DevKit-32 | Main microcontroller |
| INA3221 HW-MCU-3221 | Three-channel voltage and current sensing |
| ST7789 1.3-inch TFT | Local graphical display |
| USB cable | ESP32 power and serial communication |
| MAISHENG MS-305D | Laboratory power supply used during initial experiments |

The INA3221 module features three onboard R100 (0.1 Ω) shunt resistors.

## Wiring

### INA3221 → ESP32

| INA3221 | ESP32 |
|---|---|
| VS | 3V3 |
| GND | GND |
| SDA | GPIO21 |
| SCL | GPIO22 |

I²C device address: `0x40`.

### ST7789 → ESP32

| ST7789 | ESP32 |
|---|---|
| VCC | 3V3 |
| GND | GND |
| SCK | GPIO18 |
| SDA / MOSI | GPIO23 |
| RES | GPIO16 |
| DC | GPIO17 |
| BLK | 3V3 |

**Important:** The display requires `SPI_MODE3` for correct initialization.

## Software

- PlatformIO
- Arduino framework
- Adafruit INA3221 Library
- Adafruit GFX Library
- Adafruit ST7735 and ST7789 Library

## Features

- Three-channel INA3221 software readout
- Voltage display in volts
- Current display in milliamperes
- Calculated power display in watts
- Shunt voltage diagnostics
- TFT graphical interface
- Updates every 1000 ms
- Serial diagnostics at 115200 baud
- Sensor connection status indication
- Configured shunt resistance: 0.1 Ω per channel

## Building and Running

1. Clone this repository.
2. Open the project in Visual Studio Code with PlatformIO.
3. Connect the ESP32 DevKit-32 via USB.
4. Check the serial port configuration in `platformio.ini`.
5. Build and upload the firmware.
6. Open Serial Monitor at 115200 baud.

## Experimental Results

The following observations were obtained during initial testing:

- INA3221 successfully detected at I²C address `0x40`.
- ST7789 successfully initialized using `SPI_MODE3`.
- All three channel readings appeared on the TFT display and Serial Monitor.
- During an experimental 5.04 V input test, the channels reported similar bus voltage readings.
- During voltage decay, channel readings occasionally differed by 8 mV, corresponding to one bus-voltage measurement step.
- Different channel values were observed in later diagnostic readings.

These results confirm functional communication and data display, but **do not yet establish independent current measurement accuracy**.

## Current Limitations

- Measurement terminal connections are not equipped with permanent connectors.
- Three-channel current sensing under known independent loads has not been validated.
- Current accuracy has not been calibrated against a reference instrument.
- Reported values without a properly connected load must not be interpreted as verified load current.

**Project status: Working software prototype. Hardware measurement validation pending.**

## Version Roadmap

### v1 — ESP32

Local embedded monitoring:

- ESP32-based data acquisition
- Three-channel INA3221 interface
- ST7789 graphical display
- Serial diagnostics

### v2 — Orange Pi (Planned)

Distributed monitoring system:

- Retain ESP32 and INA3221 as the measurement controller
- Send readings to Orange Pi over Wi-Fi
- Run a Linux-based data collection service
- Store measurement history in SQLite on microSD
- Provide a browser-based dashboard
- Display historical voltage, current, and power graphs
- Add configurable alerts and logging

Planned repository: `dc-power-monitor-orange-pi`.

## Safety

This is an experimental low-voltage DC monitoring prototype. Verify the exact measurement terminal functions and use an appropriately current-limited circuit with a suitable load before applying external power.

Do not connect mains AC to the INA3221.

## Development Status

Version 1 software and GitHub publication are complete. Further hardware testing and the planned Orange Pi extension are separate development stages.
