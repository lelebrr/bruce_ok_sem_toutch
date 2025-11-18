//                            USER DEFINED SETTINGS
//   Set driver type, fonts to be loaded, pins used and SPI control method etc
//
//   See the User_Setup_Select.h file if you wish to be able to select multiple configurations
//
//   See the "Configurations" section of the User Manual for all options available

#define USER_SETUP_ID 99

// Define ST7796 Driver
#define ST7796_DRIVER

// Define screen resolution
#define TFT_WIDTH 480
#define TFT_HEIGHT 320

// Define SPI frequency
#define SPI_FREQUENCY 27000000

// Define read frequency
#define SPI_READ_FREQUENCY 20000000

// Define touch frequency
#define SPI_TOUCH_FREQUENCY 2500000

// Define pins
#define TFT_MISO 12
#define TFT_MOSI 13
#define TFT_SCLK 14
#define TFT_CS 15
#define TFT_DC 2
#define TFT_RST -1

// Define backlight pin
#define TFT_BL 27

// Define touch pins
#define TOUCH_CS 33

// Define touch driver
#define TOUCH_DRIVER 2046

// Define touch interrupt pin
#define TOUCH_CONFIG_INT_GPIO_NUM 36

// Define rotation
#define LOAD_GLCD  // Font 1. Original 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2 // Font 2. Small 16 pixel font needs ~3534 bytes in FLASH, needs ~1828 bytes in FLASH
#define LOAD_FONT4 // Font 4. Medium 26 pixel font needs ~5848 bytes in FLASH
#define LOAD_FONT6 // Font 6. Large 48 pixel font needs ~2666 bytes in FLASH, needs ~4548 bytes in FLASH
#define LOAD_FONT7 // Font 7. 7 segment 48 font needs ~2438 bytes in FLASH
#define LOAD_FONT8 // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH
#define LOAD_GFXFF // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

#define SMOOTH_FONT

// #define SPI_TOUCH_CONTROL 1

// #define SPI_FREQUENCY  27000000
// #define SPI_READ_FREQUENCY  20000000
// #define SPI_TOUCH_FREQUENCY  2500000

// #define SUPPORT_TRANSACTIONS
