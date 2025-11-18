#include "display.h"
#include <globals.h>

void checkPowerSaveTime();
void sleepModeOn();
void sleepModeOff();
void fadeOutScreen(int startValue);
void setBrightness(int value, bool force);
void turnOffDisplay();
void panelSleep(bool sleep);
int getBrightness();
