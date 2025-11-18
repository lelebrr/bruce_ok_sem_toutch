#ifndef __UTILS_H__
#define __UTILS_H__

#include "globals.h" // include for globals

// Declare stub functions for missing ones
void setBrightnessMenu();
void setDimmerTimeMenu();
void gsetRotation(char c);
void setUIColor();
void setTheme();
void setSoundConfig();
void setSoundVolume();
void setWifiStartupConfig();
void setStartupApp();
void setNetworkCredsMenu();
void setClock();
void setSleepMode();
void setSPIPinsMenu(BusConfig bus);
void setI2CPinsMenu(BusConfig bus);
void setUARTPinsMenu(BusConfig bus);
void setGpsBaudrateMenu();
void setIrTxPin(char c);
void setIrRxPin(char c);
void setIrTxRepeats();
void gsetIrTxPin(char c);
void gsetIrRxPin(char c);
void addEvilWifiMenu();
void removeEvilWifiMenu();

// Stub functions for mykeyboard
uint32_t getComplementaryColor2(uint32_t color);
void resetTftDisplay();
void wakeUpScreen();
void displayWarning(String msg, bool b = false);

// Other declares
void backToMenu();
void addOptionToMainMenu();
void updateClockTimezone();
void updateTimeStr(struct tm timeInfo);
void runClockLoop();
void showDeviceInfo();

#endif
