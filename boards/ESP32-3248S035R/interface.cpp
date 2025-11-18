#include <SPI.h>
#include <Wire.h>
#include <globals.h>

// TouchScreen
#ifdef HAS_TOUCH
#include <TFT_eSPI.h>
extern tft_logger tft;
#endif

// Include pins
#include "pins_arduino.h"

void _setup_gpio() {
    // TFT Backlight
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

// RGB LED
#ifdef HAS_RGB_LED
    pinMode(RGB_LED_R, OUTPUT);
    pinMode(RGB_LED_G, OUTPUT);
    pinMode(RGB_LED_B, OUTPUT);
    digitalWrite(RGB_LED_R, LOW);
    digitalWrite(RGB_LED_G, LOW);
    digitalWrite(RGB_LED_B, LOW);
#endif

// Speaker
#ifdef HAS_NS4168_SPKR
    pinMode(BCLK, OUTPUT);
    pinMode(WCLK, OUTPUT);
    pinMode(DOUT, OUTPUT);
    digitalWrite(BCLK, LOW);
    digitalWrite(WCLK, LOW);
    digitalWrite(DOUT, LOW);
#endif

// Battery ADC
#ifdef BAT_PIN
    pinMode(BAT_PIN, INPUT);
#endif

    // IR LED
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LOW);

    // SD Card CS
    pinMode(SDCARD_CS, OUTPUT);
    digitalWrite(SDCARD_CS, HIGH);

// Touch CS
#ifdef HAS_TOUCH
    pinMode(TOUCH_CS, OUTPUT);
    digitalWrite(TOUCH_CS, HIGH);
#endif

    // CC1101/NRF24/W5500 CS
    pinMode(CC1101_SS_PIN, OUTPUT);
    digitalWrite(CC1101_SS_PIN, HIGH);
}

void _post_setup_gpio() {
    // TFT Brightness control
    ledcSetup(0, 5000, 8);
    ledcAttachPin(TFT_BL, 0);
    ledcWrite(0, 255);
}

int getBattery() {
#ifdef BAT_PIN
    return analogRead(BAT_PIN);
#else
    return 0;
#endif
}

bool isCharging() {
    // ESP32-3248S035R doesn't have charging detection pin
    return false;
}

void setBrightness(uint8_t brightval, bool save) {
    if (brightval < MINBRIGHT) brightval = MINBRIGHT;
    if (brightval > 100) brightval = 100;

    uint32_t duty = map(brightval, 0, 100, 0, 255);
    ledcWrite(0, duty);

    if (save) {
        bruceConfig.bright = brightval;
        // bruceConfig.saveConfig(); // TODO: check if this method exists
    }
}

uint8_t getBrightness() { return bruceConfig.bright; }

void beginLed() {
#ifdef HAS_RGB_LED
    pinMode(RGB_LED_R, OUTPUT);
    pinMode(RGB_LED_G, OUTPUT);
    pinMode(RGB_LED_B, OUTPUT);
    digitalWrite(RGB_LED_R, LOW);
    digitalWrite(RGB_LED_G, LOW);
    digitalWrite(RGB_LED_B, LOW);
#endif
}

void setLedColor(uint8_t r, uint8_t g, uint8_t b) {
#ifdef HAS_RGB_LED
    digitalWrite(RGB_LED_R, r > 0 ? HIGH : LOW);
    digitalWrite(RGB_LED_G, g > 0 ? HIGH : LOW);
    digitalWrite(RGB_LED_B, b > 0 ? HIGH : LOW);
#endif
}

void setLedColor(uint16_t color) {
    uint8_t r = (color >> 11) & 0x1F;
    uint8_t g = (color >> 5) & 0x3F;
    uint8_t b = color & 0x1F;
    setLedColor(r > 15, g > 31, b > 15);
}

/*********************************************************************
** Function: InputHandler
** Handles the variables PrevPress, NextPress, SelPress, AnyKeyPress and EscPress
**********************************************************************/
void InputHandler(void) {
#ifdef HAS_TOUCH
    // ESP32-3248S035R uses touchscreen, so we need to process touch data
    static bool touchInitialized = false;
    static bool touchCalibrated = false;
    static unsigned long lastTouchTime = 0;
    static uint16_t lastTouchX = 0;
    static uint16_t lastTouchY = 0;
    static uint8_t ghostTouchCount = 0;

    // Ghost touch filtering variables
    static unsigned long touchDebugTime = 0;
    static uint16_t lastValidX = 0;
    static uint16_t lastValidY = 0;
    static bool ignoreGhostTouches = false;

    if (!touchInitialized) {
        // Initialize touch
        Serial.println("Initializing TFT and touch...");
        tft.init();
        tft.setRotation(1);
        touchInitialized = true;
        Serial.println("Touch screen initialized");

        // Debug information
        Serial.print("TFT Width: ");
        Serial.println(tftWidth);
        Serial.print("TFT Height: ");
        Serial.println(tftHeight);
        Serial.print("Touch CS: ");
        Serial.println(TOUCH_CS);
        Serial.print("Touch IRQ: ");
        Serial.println(TOUCH_CONFIG_INT_GPIO_NUM);

        // Test touch with different methods
        Serial.println("Testing touch with different methods...");

        // Method 1: getTouch
        uint16_t touchX, touchY;
        if (tft.getTouch(&touchX, &touchY)) {
            Serial.print("getTouch: X=");
            Serial.print(touchX);
            Serial.print(", Y=");
            Serial.println(touchY);
        } else {
            Serial.println("getTouch: No touch detected");
        }

        // Method 2: getTouchRaw
        uint16_t rawX, rawY;
        if (tft.getTouchRaw(&rawX, &rawY)) {
            Serial.print("getTouchRaw: X=");
            Serial.print(rawX);
            Serial.print(", Y=");
            Serial.println(rawY);
        } else {
            Serial.println("getTouchRaw: No touch detected");
        }

        // Use default calibration values
        Serial.println("Using default calibration values...");
        uint16_t touchCalibration[4] = {300, 300, 3800, 3800};
        tft.setTouch(touchCalibration);
        Serial.println("Touch calibration applied");
    }

    // Calibration mode removed - using automatic calibration

    // Get touch data using simplified method
    uint16_t rawX, rawY;
    bool touchDetected = tft.getTouchRaw(&rawX, &rawY);

    // Enhanced ghost touch detection
    if (touchDetected) {
        // Check for specific ghost touch patterns
        if (rawX == 0 && rawY == 4095) {
            // This is a common ghost touch pattern
            ghostTouchCount++;
            if (ghostTouchCount > 5) {
                touchDetected = false;
                ignoreGhostTouches = true;
                lastTouchTime = millis();
            }
        } else if (rawX > 4000 && rawY > 280 && rawY < 320) {
            // Pattern: high X, Y in button area - likely ghost
            ghostTouchCount++;
            if (ghostTouchCount > 3) {
                touchDetected = false;
                ignoreGhostTouches = true;
                lastTouchTime = millis();
            }
        } else {
            // Reset ghost touch counter for valid touches
            ghostTouchCount = 0;
        }
    }

    // Time-based filtering - reduced threshold
    if (touchDetected && millis() - lastTouchTime < 200) {
        // Check if this is a rapid repeat touch (likely ghost)
        if (abs((int)rawX - (int)lastValidX) < 20 && abs((int)rawY - (int)lastValidY) < 20) {
            touchDetected = false;
            ignoreGhostTouches = true;
            lastTouchTime = millis();
        }
    }

    if (touchDetected && ignoreGhostTouches && millis() - lastTouchTime < 500) {
        // Still in ghost touch ignore period - reduced from 800 to 500
        touchDetected = false;
    }

    if (touchDetected) {
        lastTouchTime = millis();
        ignoreGhostTouches = false;
        lastValidX = rawX;
        lastValidY = rawY;
    }

    // Debug output
    if (millis() - touchDebugTime > 2000) {
        Serial.print("Touch check: ");
        Serial.println(touchDetected ? "DETECTED" : "NOT DETECTED");
        Serial.print("Raw touch data: X=");
        Serial.print(rawX);
        Serial.print(", Y=");
        Serial.println(rawY);
        touchDebugTime = millis();
    }

    if (touchDetected) {
        // Convert raw coordinates to screen coordinates using proper mapping
        // The raw values from XPT2046 are typically 0-4095, but need to be mapped to screen size
        uint16_t touchX = map(rawX, 0, 4095, 0, tftWidth);
        uint16_t touchY = map(rawY, 0, 4095, 0, tftHeight);

        // Debug: show raw and converted coordinates
        Serial.print("Raw: ");
        Serial.print(rawX);
        Serial.print(", ");
        Serial.print(rawY);
        Serial.print(" -> Converted: ");
        Serial.print(touchX);
        Serial.print(", ");
        Serial.println(touchY);

        // Use the mapped coordinates
        touchPoint.x = touchX;
        touchPoint.y = touchY;
        touchPoint.pressed = true;

        // Debug output
        Serial.print("Touch detected: ");
        Serial.print(touchPoint.x);
        Serial.print(", ");
        Serial.println(touchPoint.y);

        // Set button flags based on touch position
        if (touchPoint.y > tftHeight - 50) {
            // Bottom area - navigation buttons
            if (touchPoint.x < tftWidth / 3) {
                PrevPress = true;
                Serial.println("Previous button pressed");
            } else if (touchPoint.x > 2 * tftWidth / 3) {
                NextPress = true;
                Serial.println("Next button pressed");
            } else {
                SelPress = true;
                Serial.println("Select button pressed");
            }
        }

        AnyKeyPress = true;
    } else {
        if (touchPoint.pressed) {
            Serial.println("Touch released");
            touchPoint.pressed = false;
        }
    }
#endif
}
