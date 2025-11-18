// TFT_eSPI display
#define ST7796_DRIVER 1
#ifndef USER_SETUP_LOADED
#define USER_SETUP_LOADED
#endif

#define TFT_WIDTH 320
#define TFT_HEIGHT 480

#define TFT_MISO 12
#define TFT_MOSI 13
#define TFT_SCLK 14
#define TFT_CS 15
#define TFT_DC 2
#define TFT_RST -1
#define TFT_BL 27

#define TOUCH_CS 33
#define TOUCH_DRIVER 2046
#define TOUCH_ORIENTATION 0
#define TOUCH_IRQ 36

// Define SS pin for SD card
#define SS 5

#ifndef SMOOTH_FONT
#define SMOOTH_FONT
#endif

#define SPI_FREQUENCY 27000000
#define SPI_READ_FREQUENCY 20000000
#define SPI_TOUCH_FREQUENCY 2500000

// SD card
#define SDCARD_CS 5
#define SDCARD_SCK 18
#define SDCARD_MISO 19
#define SDCARD_MOSI 23

// I2C
#define GROVE_SDA 27
#define GROVE_SCL 22

// SPI general
#define SPI_SCK_PIN 18
#define SPI_MOSI_PIN 23
#define SPI_MISO_PIN 19
#define SPI_SS_PIN 27

// Battery
#define BAT_PIN 34

// RGB LED
#define HAS_RGB_LED 1
#define RGB_LED_R 4
#define RGB_LED_G 16
#define RGB_LED_B 17
#define LED_TYPE WS2812B
#define LED_ORDER GRB
#define LED_COUNT 1
#define LED_COLOR_STEP 15

// Speaker
#define BCLK 26
#define WCLK 25
#define DOUT 32

// Serial GPS
#define SERIAL_TX 3
#define SERIAL_RX 1

// IR/RF pins
#define LED 22

// CC1101
#define CC1101_GDO0_PIN 22
#define CC1101_SS_PIN 27

// NRF24
#define NRF24_CE_PIN 22
#define NRF24_SS_PIN 27

// W5500
#define W5500_SS_PIN 27
#define W5500_INT_PIN 22

// FM Radio
#define FM_RSTPIN 40

// I2C pins for Wire library
#define SDA 21
#define SCL 22

// SPI pins for SPI library
#define SCK 18
#define MISO 19
#define MOSI 23
