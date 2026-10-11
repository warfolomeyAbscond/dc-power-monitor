
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <math.h>

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <Adafruit_INA3221.h>

// =====================================================
// DC POWER MONITOR
// ESP32 DevKit-32 + INA3221 + ST7789 240x240
// =====================================================

// --------------------- ST7789 ------------------------

#define TFT_CS    -1
#define TFT_DC    17
#define TFT_RST   16
#define TFT_SCLK  18
#define TFT_MOSI  23

// --------------------- INA3221 -----------------------

#define I2C_SDA 21
#define I2C_SCL 22
#define INA_ADDRESS 0x40

// Onboard current-sense resistors marked R100 = 0.1 ohm
const float SHUNT_RESISTANCE_OHMS = 0.1f;

// --------------------- OBJECTS -----------------------

Adafruit_ST7789 tft(TFT_CS, TFT_DC, TFT_RST);
Adafruit_INA3221 ina3221;

// --------------------- STATE -------------------------

bool sensorReady = false;

const uint32_t UPDATE_INTERVAL_MS = 1000;
uint32_t lastUpdate = 0;

struct ChannelData {
    float busVoltage;
    float shuntVoltage;
    float current;
    float power;
    bool valid;
};

ChannelData channels[3];

// --------------------- COLORS ------------------------

const uint16_t C_BG     = 0x1082;
const uint16_t C_PANEL  = 0x18E3;
const uint16_t C_CYAN   = 0x07FF;
const uint16_t C_WHITE  = ST77XX_WHITE;
const uint16_t C_GREEN  = ST77XX_GREEN;
const uint16_t C_RED    = ST77XX_RED;
const uint16_t C_YELLOW = ST77XX_YELLOW;
const uint16_t C_GRAY   = 0x8410;

// =====================================================
// SENSOR INITIALIZATION
// =====================================================

void initializeSensor() {
    Wire.begin(I2C_SDA, I2C_SCL);
    Wire.setClock(100000);

    sensorReady = ina3221.begin(INA_ADDRESS, &Wire);

    if (!sensorReady) {
        Serial.println("[ERROR] INA3221 not found at 0x40");
        return;
    }

    // Configure each channel independently.
    // R100 means 0.1 ohm.
    for (uint8_t ch = 0; ch < 3; ch++) {
        ina3221.setShuntResistance(
            ch,
            SHUNT_RESISTANCE_OHMS
        );
    }

    Serial.println("[OK] INA3221 connected at 0x40");
    Serial.println("[OK] Shunt resistors: 0.100 ohm");
    Serial.println("[OK] Channels: 3");
}

// =====================================================
// DISPLAY HELPERS
// =====================================================

void centerText(
    const char *text,
    int y,
    uint8_t textSize,
    uint16_t color
) {
    tft.setTextSize(textSize);
    tft.setTextColor(color);

    int16_t x1, y1;
    uint16_t w, h;

    tft.getTextBounds(
        text,
        0, y,
        &x1, &y1,
        &w, &h
    );

    tft.setCursor((240 - w) / 2, y);
    tft.print(text);
}

// =====================================================
// DRAW STATIC DISPLAY
// =====================================================

void drawInterface() {
    tft.fillScreen(C_BG);
    tft.setTextWrap(false);

    centerText("DC POWER MONITOR", 12, 2, C_CYAN);

    tft.drawFastHLine(10, 37, 220, C_CYAN);

    for (uint8_t ch = 0; ch < 3; ch++) {
        int y = 45 + ch * 55;

        tft.fillRoundRect(
            8, y,
            224, 51,
            6, C_PANEL
        );

        tft.setTextColor(C_CYAN);
        tft.setTextSize(2);

        tft.setCursor(17, y + 17);
        tft.print("CH");
        tft.print(ch + 1);
    }

    tft.setTextSize(1);
    tft.setCursor(12, 218);

    if (sensorReady) {
        tft.setTextColor(C_GREEN);
        tft.print("INA3221: CONNECTED");
    } else {
        tft.setTextColor(C_RED);
        tft.print("INA3221: NOT FOUND");
    }
}

// =====================================================
// READ ALL THREE CHANNELS
// =====================================================

void readMeasurements() {
    for (uint8_t ch = 0; ch < 3; ch++) {
        ChannelData &data = channels[ch];

        data.busVoltage   = NAN;
        data.shuntVoltage = NAN;
        data.current      = NAN;
        data.power        = NAN;
        data.valid        = false;

        if (!sensorReady) {
            continue;
        }

        // Each call uses its own channel index: 0, 1, 2.
        float bus = ina3221.getBusVoltage(ch);
        float shunt = ina3221.getShuntVoltage(ch);
        float current = ina3221.getCurrentAmps(ch);

        if (
            !isfinite(bus) ||
            !isfinite(shunt) ||
            !isfinite(current)
        ) {
            continue;
        }

        data.busVoltage = bus;
        data.shuntVoltage = shunt;
        data.current = current;
        data.power = bus * current;
        data.valid = true;
    }
}

// =====================================================
// UPDATE DISPLAY
// =====================================================

void updateDisplay() {
    for (uint8_t ch = 0; ch < 3; ch++) {
        int y = 45 + ch * 55;

        // Clear only dynamic values
        tft.fillRect(
            77,
            y + 4,
            150,
            44,
            C_PANEL
        );

        tft.setTextSize(1);

        if (!channels[ch].valid) {
            tft.setTextColor(C_RED);
            tft.setCursor(86, y + 20);
            tft.print("NO DATA");
            continue;
        }

        const ChannelData &data = channels[ch];

        tft.setTextColor(C_WHITE);

        // Voltage
        tft.setCursor(84, y + 6);
        tft.printf(
            "%.3f V",
            data.busVoltage
        );

        // Current
        tft.setCursor(84, y + 20);
        tft.printf(
            "%.2f mA",
            data.current * 1000.0f
        );

        // Power
        tft.setCursor(84, y + 34);
        tft.printf(
            "%.3f W",
            data.power
        );
    }
}

// =====================================================
// SERIAL DIAGNOSTICS
// =====================================================

void printDiagnostics() {
    Serial.println();
    Serial.println(
        "========== INA3221 MEASUREMENTS =========="
    );

    if (!sensorReady) {
        Serial.println("INA3221 NOT CONNECTED");
        return;
    }

    for (uint8_t ch = 0; ch < 3; ch++) {
        const ChannelData &data = channels[ch];

        Serial.printf(
            "CH%d | ",
            ch + 1
        );

        if (!data.valid) {
            Serial.println("INVALID READING");
            continue;
        }

        Serial.printf(
            "BUS = %.4f V | ",
            data.busVoltage
        );

        Serial.printf(
            "SHUNT = %.4f mV | ",
            data.shuntVoltage * 1000.0f
        );

        Serial.printf(
            "CURRENT = %.3f mA | ",
            data.current * 1000.0f
        );

        Serial.printf(
            "POWER = %.5f W\n",
            data.power
        );
    }

    Serial.println(
        "=========================================="
    );
}

// =====================================================
// SETUP
// =====================================================

void setup() {
    Serial.begin(115200);
    delay(500);

    Serial.println();
    Serial.println("DC Power Monitor starting...");

    // Initialize sensor
    initializeSensor();

    // Initialize display using the verified casino setup.
    SPI.begin(
        TFT_SCLK,
        -1,
        TFT_MOSI,
        -1
    );

    // IMPORTANT: this display requires SPI_MODE3.
    tft.init(
        240,
        240,
        SPI_MODE3
    );

    tft.setRotation(0);

    drawInterface();

    // Initial measurement and display update
    readMeasurements();
    updateDisplay();
    printDiagnostics();

    lastUpdate = millis();

    Serial.println("[OK] System initialized");
}

// =====================================================
// LOOP
// =====================================================

void loop() {
    uint32_t now = millis();

    if (now - lastUpdate >= UPDATE_INTERVAL_MS) {
        lastUpdate = now;

        readMeasurements();
        updateDisplay();
        printDiagnostics();
    }
}
