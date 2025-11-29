//                            USER DEFINED SETTINGS
//   Set driver type, fonts to be loaded, pins used and SPI control method etc
//
//   See the User_Setup_Select.h file if you wish to be able to select multiple configurations
//
//   See the "Configurations" section of the User Manual for all options available

#define USER_SETUP_ID 99

// Use ST7796 driver on HSPI bus
#define ST7796_DRIVER
#define USE_HSPI_PORT

// Native panel resolution (portrait)
#define TFT_WIDTH 320
#define TFT_HEIGHT 480

// SPI frequencies
#define SPI_FREQUENCY       27000000
#define SPI_READ_FREQUENCY  20000000
#define SPI_TOUCH_FREQUENCY 2500000

// TFT control pins (Sunton / CYD ESP32-3248S035R)
#define TFT_MISO 12
#define TFT_MOSI 13
#define TFT_SCLK 14
#define TFT_CS   15
#define TFT_DC   2
#define TFT_RST  4

// Backlight pin
#define TFT_BL 27

// Resistive touch (XPT2046 compatible)
#define TOUCH_CS  33
#define TOUCH_IRQ 36
#define TOUCH_DRIVER 2046

// Optional interrupt pin used by some helper libraries
#define TOUCH_CONFIG_INT_GPIO_NUM 36

// Fonts
#define LOAD_GLCD   // Font 1. Original 8 pixel font
#define LOAD_FONT2  // Small 16 pixel font
#define LOAD_FONT4  // Medium 26 pixel font
#define LOAD_FONT6  // Large 48 pixel font
#define LOAD_FONT7  // 7 segment 48 font
#define LOAD_FONT8  // Large 75 pixel font
#define LOAD_GFXFF  // FreeFonts. Access to Adafruit_GFX free fonts

#define SMOOTH_FONT
