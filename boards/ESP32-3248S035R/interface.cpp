#include "interface.h"
#include "pins.h"
#include "core/powerSave.h"
#include "input_state.h"
#include "core/utils.h"
#include <XPT2046_Touchscreen.h>
#include <SPI.h>

XPT2046_Touchscreen ts(TOUCH_CS);
SPIClass hspi(HSPI);

void _setup_gpio() {
    hspi.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);
    ts.begin(hspi);
    ts.setRotation(ROTATION);
}

void _post_setup_gpio() {
    // nothing to do
}

void _setBrightness(uint8_t brightval) {
    analogWrite(TFT_BL, brightval);
}

void InputHandler(void) {
    if (ts.touched()) {
        TS_Point p = ts.getPoint();
        touchPoint.x = p.x;
        touchPoint.y = p.y;
        touchHeatMap(touchPoint);
    }
}

int getBattery() {
    return 100;
}

void powerOff() {
    // not implemented
}

void checkReboot() {
    // not implemented
}
