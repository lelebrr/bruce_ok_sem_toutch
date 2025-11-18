#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <string>

void backToMenu();
void addOptionToMainMenu();
void updateClockTimezone();
void updateTimeStr(struct tm timeInfo);
void showDeviceInfo();

#if defined(HAS_TOUCH)
void touchHeatMap(struct TouchPoint t);
#endif

String getOptionsJSON();

#endif // UTILS_H
