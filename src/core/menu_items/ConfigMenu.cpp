#include "ConfigMenu.h"
#include "core/display.h"
#include "core/display_functions.h"
#include "core/i2c_finder.h"
#include "core/settings.h"
#include "core/wifi/wifi_common.h"
#ifdef HAS_RGB_LED
#include "core/led_control.h"
#endif

void ConfigMenu::optionsMenu() {
    options.clear();
    options.emplace_back("Brightness", setBrightnessMenu);
    options.emplace_back("Dim Time", setDimmerTimeMenu);
    options.emplace_back("Orientation", lambdaHelper(gsetRotation, '1'));
    options.emplace_back("UI Color", setUIColor);
    options.emplace_back("UI Theme", setTheme);
    options.emplace_back(String("InstaBoot: " + String(bruceConfig.instantBoot ? "ON" : "OFF")), [=]() {
        bruceConfig.instantBoot = !bruceConfig.instantBoot;
        bruceConfig.saveFile();
    });
#ifndef HAS_RGB_LED
    // LED options removed for boards without RGB LED
#endif
    options.emplace_back("Sound On/Off", setSoundConfig);
#if defined(HAS_NS4168_SPKR)
    options.emplace_back("Sound Volume", setSoundVolume);
#endif
    options.emplace_back("Startup WiFi", setWifiStartupConfig);
    options.emplace_back("Startup App", setStartupApp);
    options.emplace_back("Hide/Show Apps", []() { mainMenu.hideAppsMenu(); });
    options.emplace_back("Network Creds", setNetworkCredsMenu);
    options.emplace_back("Clock", setClock);
    options.emplace_back("Sleep", setSleepMode);
    options.emplace_back("Factory Reset", [=]() { bruceConfig.factoryReset(); });
    options.emplace_back("Restart", [=]() { ESP.restart(); });

    options.emplace_back("Turn-off", powerOff);
    options.emplace_back("Deep Sleep", goToDeepSleep);

    if (bruceConfig.devMode) options.emplace_back("Device Pin setting", [=]() { devMenu(); });

    options.emplace_back("About", showDeviceInfo);
    addOptionToMainMenu();

    loopOptions(options, MENU_TYPE_SUBMENU, "Config");
}

void ConfigMenu::devMenu() {
    options.clear();
    options.emplace_back("I2C Finder", find_i2c_addresses);
    options.emplace_back("CC1101 Pins", [=]() { setSPIPinsMenu(bruceConfigPins.CC1101_bus); });
    options.emplace_back("NRF24  Pins", [=]() { setSPIPinsMenu(bruceConfigPins.NRF24_bus); });
    options.emplace_back("SDCard Pins", [=]() { setSPIPinsMenu(bruceConfigPins.SDCARD_bus); });
    //{"SYSI2C Pins", [=]() { setI2CPinsMenu(bruceConfigPins.sys_i2c); }   },
    options.emplace_back("I2C Pins", [=]() { setI2CPinsMenu(bruceConfigPins.i2c_bus); });
    options.emplace_back("UART Pins", [=]() { setUARTPinsMenu(bruceConfigPins.uart_bus); });
    options.emplace_back("GPS Pins", [=]() { setUARTPinsMenu(bruceConfigPins.gps_bus); });
    options.emplace_back("Back", [=]() { optionsMenu(); });

    loopOptions(options, MENU_TYPE_SUBMENU, "Dev Mode");
}
void ConfigMenu::drawIconImg() {
    drawImg(
        *bruceConfig.themeFS(),
        bruceConfig.getThemeItemImg(bruceConfig.theme.paths.config),
        0,
        imgCenterY,
        true
    );
}
void ConfigMenu::drawIcon(float scale) {
    clearIconArea();
    int radius = scale * 9;

    int i = 0;
    for (i = 0; i < 6; i++) {
        tft.drawArc(
            iconCenterX,
            iconCenterY,
            3.5 * radius,
            2 * radius,
            15 + 60 * i,
            45 + 60 * i,
            bruceConfig.priColor,
            bruceConfig.bgColor,
            true
        );
    }

    tft.drawArc(
        iconCenterX,
        iconCenterY,
        2.5 * radius,
        radius,
        0,
        360,
        bruceConfig.priColor,
        bruceConfig.bgColor,
        false
    );
}
