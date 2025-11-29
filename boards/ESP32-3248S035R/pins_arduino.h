#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <stdint.h>

// Main SPI bus for TFT (HSPI)
#define TFT_MISO 12
#define TFT_MOSI 13
#define TFT_SCLK 14
#define TFT_CS   15
#define TFT_DC   2
#define TFT_RST  4
#define TFT_BL   27

// Resistive touch controller (XPT2046-compatible, shares HSPI)
#define TOUCH_CS  33
#define TOUCH_IRQ 36

// Secondary SPI bus for SD card / RF modules (VSPI)
static const uint8_t SS   = 5;
static const uint8_t MOSI = 23;
static const uint8_t MISO = 19;
static const uint8_t SCK  = 18;

// Default I2C bus (external header)
static const uint8_t SDA = 21;
static const uint8_t SCL = 22;

#endif /* Pins_Arduino_h */
