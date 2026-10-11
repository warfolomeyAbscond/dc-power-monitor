# DC Power Monitor

Three-channel DC monitoring prototype using ESP32, INA3221 and a 240x240 ST7789 TFT display.

## Features

- Three independent INA3221 channel reads
- Voltage, current and calculated power display
- Shunt voltage diagnostics in Serial Monitor
- One-second measurement updates
- Sensor detection at I2C address 0x40

## Hardware

- ESP32 DevKit-32
- INA3221 HW-MCU-3221 with R100 (0.1 ohm) shunt resistors
- ST7789 1.3-inch TFT display
- USB cable and jumper wires

## Wiring

### INA3221

| INA3221 | ESP32 |
|---|---|
| VS | 3V3 |
| GND | GND |
| SDA | GPIO21 |
| SCL | GPIO22 |

### ST7789

| Display | ESP32 |
|---|---|
| VCC | 3V3 |
| GND | GND |
| SCK | GPIO18 |
| SDA / MOSI | GPIO23 |
| RES | GPIO16 |
| DC | GPIO17 |
| BLK | 3V3 |

Important: the display requires SPI_MODE3.

## Software

PlatformIO with the Arduino framework.

Dependencies:
- Adafruit INA3221 Library
- Adafruit GFX Library
- Adafruit ST7735 and ST7789 Library

## Usage

1. Open the project in VS Code with PlatformIO.
2. Connect ESP32 using USB.
3. Build and upload the firmware.
4. Open Serial Monitor at 115200 baud.
5. View channel readings on the TFT.

## Project Status

Verified:
- INA3221 I2C communication
- ST7789 display operation with SPI_MODE3
- Three-channel software readout
- Serial diagnostics

Pending verification:
- Independent physical channel operation under known loads
- Current measurement accuracy and calibration

The current measurement connections are not permanently fitted with terminals.

## Safety

For compatible low-voltage DC circuits only. Confirm the module's measurement wiring before applying external power. Do not connect mains voltage.
