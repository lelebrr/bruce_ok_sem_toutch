#include "ConfigMenu.h"
#include "core/display.h"
#include "core/display_functions.h"
#include "core/utils.h"
#include <globals.h>

void ConfigMenu::optionsMenu() {
    options.clear();
    options.emplace_back("Display", []() {
        // TODO: Implement display settings
    });
    options.emplace_back("Network", []() {
        // TODO: Implement network settings
    });
    options.emplace_back("System", []() {
        // TODO: Implement system settings
    });
    addOptionToMainMenu();

    loopOptions(options, MENU_TYPE_SUBMENU, "Configuration");
}

void ConfigMenu::drawIconImg() {
    drawImgFromFS(
        *bruceConfig.themeFS(),
        bruceConfig.getThemeItemImg(bruceConfig.theme.paths.config),
        0,
        imgCenterY,
        true
    );
}

void ConfigMenu::drawIcon(float scale) {
    clearIconArea();
    int iconSize = scale * 40;

    // Gear icon
    int centerX = iconCenterX;
    int centerY = iconCenterY;

    // Draw gear teeth
    for (int i = 0; i < 8; i++) {
        float angle = i * PI / 4;
        float x1 = centerX + cos(angle) * iconSize / 2;
        float y1 = centerY + sin(angle) * iconSize / 2;
        float x2 = centerX + cos(angle) * iconSize / 3;
        float y2 = centerY + sin(angle) * iconSize / 3;

        tft.fillTriangle(x1 - 2, y1 - 2, x1 + 2, y1 + 2, x2, y2, bruceConfig.priColor);
    }

    // Draw gear center
    tft.fillCircle(centerX, centerY, iconSize / 3, bruceConfig.priColor);
    tft.fillCircle(centerX, centerY, iconSize / 6, bruceConfig.bgColor);
}
