#include "GpsMenu.h"
#include "core/display.h"
#include "core/display_functions.h"
#include "core/utils.h"
#include <globals.h>

void GpsMenu::optionsMenu() {
    options.clear();
    options.emplace_back("GPS Status", []() {
        // TODO: Implement GPS status
    });
    options.emplace_back("GPS Settings", []() {
        // TODO: Implement GPS settings
    });
    options.emplace_back("GPS Maps", []() {
        // TODO: Implement GPS maps
    });
    addOptionToMainMenu();

    loopOptions(options, MENU_TYPE_SUBMENU, "GPS");
}

void GpsMenu::drawIconImg() {
    drawImgFromFS(
        *bruceConfig.themeFS(), bruceConfig.getThemeItemImg(bruceConfig.theme.paths.gps), 0, imgCenterY, true
    );
}

void GpsMenu::drawIcon(float scale) {
    clearIconArea();
    int iconSize = scale * 40;

    // GPS satellite representation
    int centerX = iconCenterX;
    int centerY = iconCenterY;

    // Satellite body
    tft.fillRoundRect(
        centerX - iconSize / 3,
        centerY - iconSize / 6,
        iconSize * 2 / 3,
        iconSize / 3,
        iconSize / 8,
        bruceConfig.priColor
    );

    // Solar panels
    tft.fillRect(
        centerX - iconSize / 2, centerY - iconSize / 8, iconSize / 6, iconSize / 4, bruceConfig.priColor
    );
    tft.fillRect(
        centerX + iconSize / 3, centerY - iconSize / 8, iconSize / 6, iconSize / 4, bruceConfig.priColor
    );

    // Signal waves
    for (int i = 0; i < 3; i++) {
        tft.drawArc(
            centerX,
            centerY,
            iconSize / 2 + i * 5,
            iconSize / 3 + i * 3,
            45,
            135,
            bruceConfig.priColor,
            bruceConfig.bgColor
        );
    }
}
