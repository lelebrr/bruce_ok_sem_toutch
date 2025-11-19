#include "FMMenu.h"
#include "core/display.h"
#include "core/display_functions.h"
#include "core/utils.h"
#include <globals.h>

void FMMenu::optionsMenu() {
    options.clear();
    options.emplace_back("Scan", []() {
        // TODO: Implement FM scanning
    });
    options.emplace_back("Volume", []() {
        // TODO: Implement volume control
    });
    options.emplace_back("Frequency", []() {
        // TODO: Implement frequency setting
    });
    addOptionToMainMenu();

    loopOptions(options, MENU_TYPE_SUBMENU, "FM Radio");
}

void FMMenu::drawIconImg() {
    if (bruceConfig.theme.fs) {
        drawImgFromFS(SD, bruceConfig.getThemeItemImg(bruceConfig.theme.paths.fm), 0, imgCenterY, true);
    } else {
        drawImgFromFS(LittleFS, bruceConfig.getThemeItemImg(bruceConfig.theme.paths.fm), 0, imgCenterY, true);
    }
}

void FMMenu::drawIcon(float scale) {
    clearIconArea();
    int radius = scale * 30;
    int antennaLength = scale * 40;

    // Radio body
    tft.fillRoundRect(
        iconCenterX - radius, iconCenterY - radius / 2, radius * 2, radius, radius / 4, bruceConfig.priColor
    );

    // Antenna
    tft.drawLine(
        iconCenterX + radius,
        iconCenterY - radius / 2,
        iconCenterX + radius,
        iconCenterY - radius / 2 - antennaLength,
        bruceConfig.priColor
    );

    // Speaker grilles
    for (int i = 0; i < 3; i++) {
        tft.fillRect(
            iconCenterX - radius + 5 + i * 15,
            iconCenterY - radius / 2 + 10,
            3,
            radius - 20,
            bruceConfig.bgColor
        );
    }
}
