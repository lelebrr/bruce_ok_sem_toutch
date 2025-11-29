#include "core/powerSave.h"
#include "core/utils.h"
#include <Arduino.h>
#include <globals.h>
#include <interface.h>

// Fallback brightness parameters if not provided by build flags
#ifndef TFT_BRIGHT_CHANNEL
#define TFT_BRIGHT_CHANNEL 0
#endif

#ifndef TFT_BRIGHT_FREQ
#define TFT_BRIGHT_FREQ 5000
#endif

#ifndef TFT_BRIGHT_Bits
#define TFT_BRIGHT_Bits 8
#endif

/***************************************************************************************
** Function name: _setup_gpio()
** Description:   Initial setup for the Sunton ESP32-3248S035R (Cheap Yellow Display 3.5")
***************************************************************************************/
void _setup_gpio() {
#ifdef TFT_CS
    pinMode(TFT_CS, OUTPUT);
    digitalWrite(TFT_CS, HIGH);
#endif

#ifdef TOUCH_CS
    pinMode(TOUCH_CS, OUTPUT);
    digitalWrite(TOUCH_CS, HIGH);
#endif

#ifdef SDCARD_CS
    if (SDCARD_CS >= 0) {
        pinMode(SDCARD_CS, OUTPUT);
        digitalWrite(SDCARD_CS, HIGH);
    }
#endif
}

/***************************************************************************************
** Function name: _post_setup_gpio()
** Description:   Second stage GPIO setup (touch calibration, brightness)
***************************************************************************************/
void _post_setup_gpio() {
#if defined(USE_TFT_eSPI_TOUCH)
    pinMode(TOUCH_CS, OUTPUT);

    uint16_t calData[5];
    File caldata = LittleFS.open("/calData", "r");

    if (!caldata) {
        // First boot or missing calibration file: run touch calibration
        tft.setRotation(ROTATION);
        tft.calibrateTouch(calData, TFT_WHITE, TFT_BLACK, 10);

        caldata = LittleFS.open("/calData", "w");
        if (caldata) {
            caldata.printf(
                "%d\n%d\n%d\n%d\n%d\n",
                calData[0],
                calData[1],
                calData[2],
                calData[3],
                calData[4]
            );
            caldata.close();
        }
    } else {
        Serial.print("\ntft Calibration data: ");
        for (int i = 0; i < 5; i++) {
            String line = caldata.readStringUntil('\n');
            calData[i] = line.toInt();
            Serial.printf("%d, ", calData[i]);
        }
        Serial.println();
        caldata.close();
    }

    tft.setTouch(calData);
#endif

#ifdef TFT_BL
    // Brightness control must be initialized after TFT setup
    pinMode(TFT_BL, OUTPUT);
    ledcSetup(TFT_BRIGHT_CHANNEL, TFT_BRIGHT_FREQ, TFT_BRIGHT_Bits);
    ledcAttachPin(TFT_BL, TFT_BRIGHT_CHANNEL);
    ledcWrite(TFT_BRIGHT_CHANNEL, 255);
#endif
}

/*********************************************************************
** Function: getBattery
** Description: Sunton ESP32-3248S035R is typically USB powered, no fuel gauge
**********************************************************************/
int getBattery() {
    // Return 0 to indicate "no battery information"
    return 0;
}

/***************************************************************************************
** Function name: isCharging()
** Description:   This board has no battery charging feedback by default
***************************************************************************************/
bool isCharging() {
    return false;
}

/*********************************************************************
** Function: _setBrightness
** Description:   Set backlight brightness (0–100%)
**********************************************************************/
void _setBrightness(uint8_t brightval) {
#ifdef TFT_BL
    int dutyCycle;
    if (brightval == 100) dutyCycle = 255;
    else if (brightval == 75) dutyCycle = 130;
    else if (brightval == 50) dutyCycle = 70;
    else if (brightval == 25) dutyCycle = 20;
    else if (brightval == 0) dutyCycle = 0;
    else dutyCycle = (brightval * 255) / 100;

    ledcWrite(TFT_BRIGHT_CHANNEL, dutyCycle);
#endif
}

/*********************************************************************
** Function: InputHandler
** Description:   Handle resistive touch input and map to global touchPoint
**********************************************************************/
void InputHandler(void) {
    static long d_tmp = 0;
    if (millis() - d_tmp > 200 || LongPress) {
        // Avoid keeping SPI bus busy when not needed
#if defined(USE_TFT_eSPI_TOUCH)
        TouchPoint t;
        checkPowerSaveTime();
        bool _IH_touched = tft.getTouch(&t.x, &t.y);
        if (_IH_touched) {
            NextPress = false;
            PrevPress = false;
            UpPress = false;
            DownPress = false;
            SelPress = false;
            EscPress = false;
            AnyKeyPress = false;
            NextPagePress = false;
            PrevPagePress = false;
            touchPoint.pressed = false;
            _IH_touched = false;

            // Apply rotation corrections similar to CYD boards
            if (bruceConfig.rotation == 3) {
                t.y = (tftHeight + 20) - t.y;
                t.x = tftWidth - t.x;
            }
            if (bruceConfig.rotation == 0) {
                int tmp = t.x;
                t.x = tftWidth - t.y;
                t.y = tmp;
            }
            if (bruceConfig.rotation == 2) {
                int tmp = t.x;
                t.x = t.y;
                t.y = (tftHeight + 20) - tmp;
            }

            if (!wakeUpScreen()) AnyKeyPress = true;
            else goto END;

            // Update global touch point
            touchPoint.x = t.x;
            touchPoint.y = t.y;
            touchPoint.pressed = true;
            touchHeatMap(touchPoint);
        END:
            d_tmp = millis();
        }
#endif
    }
}

/*********************************************************************
** Function: powerOff
** Description:   Put device into deep sleep, wake on BOOT button (GPIO0)
**********************************************************************/
void powerOff() {
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, LOW);
    esp_deep_sleep_start();
}

/*********************************************************************
** Function: goToDeepSleep
** Description:   Alias to powerOff for this board
**********************************************************************/
void goToDeepSleep() {
    powerOff();
}

/*********************************************************************
** Function: checkReboot
** Description:   Optional long-press-to-power-off logic (not used here)
**********************************************************************/
void checkReboot() {}
