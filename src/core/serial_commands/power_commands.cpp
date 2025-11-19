#include "power_commands.h"
#include "core/settings.h"
#include <globals.h>

void poweroffCallback(cmd *c) {
    powerOff();
    esp_deep_sleep_start(); // only wake up via hardware reset
}

void rebootCallback(cmd *c) { ESP.restart(); }

void sleepCallback(cmd *c) { setSleepMode(); }

void createPowerCommands(SimpleCLI *cli) {
    Command cmdOff = cli->addCmd("power_off", poweroffCallback);
    Command cmdReboot = cli->addCmd("power_reboot", rebootCallback);
    Command cmdSleep = cli->addCmd("power_sleep", sleepCallback);

    Command cmdOff2 = cli->addCmd("poweroff", poweroffCallback);
    Command cmdReboot2 = cli->addCmd("reboot", rebootCallback);
    Command cmdSleep2 = cli->addCmd("sleep", sleepCallback);
}
