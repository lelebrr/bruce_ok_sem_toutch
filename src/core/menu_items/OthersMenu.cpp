#include "OthersMenu.h"
#include "core/display.h"
#include "core/display_functions.h"
#include "core/utils.h"
#include <globals.h>

void OthersMenu::optionsMenu() {
    options.clear();
    options.emplace_back("Utilities", []() {
        // TODO: Implement utilities menu
    });
    options.emplace_back("Tools", []() {
        // TODO: Implement tools menu
    });
    options.emplace_back("Settings", []() {
        // TODO: Implement settings menu
    });
    addOptionToMainMenu();

    loopOptions(options, MENU_TYPE_SUBMENU, "Others");
}

void OthersMenu::drawIconImg() {
    drawImgFromFS(
        (fs::FS &)(*bruceConfig.themeFS()),
        bruceConfig.getThemeItemImg(bruceConfig.theme.paths.others),
        0,
        imgCenterY,
        true
    );
}

void OthersMenu::drawIcon(float scale) {
    clearIconArea();
    int iconSize = scale * 40;

    // Miscellaneous tools representation
    int centerX = iconCenterX;
    int centerY = iconCenterY;

    // Draw various tools
    // Wrench
    tft.fillRoundRect(
        centerX - iconSize / 2,
        centerY - iconSize / 4,
        iconSize / 3,
        iconSize / 6,
        iconSize / 16,
        bruceConfig.priColor
    );

    // Screwdriver
    tft.fillRect(
        centerX + iconSize / 6, centerY - iconSize / 4, iconSize / 6, iconSize / 3, bruceConfig.priColor
    );

    // Hammer
    tft.fillRect(centerX, centerY + iconSize / 6, iconSize / 4, iconSize / 8, bruceConfig.priColor);
    tft.fillRoundRect(
        centerX - iconSize / 8,
        centerY + iconSize / 3,
        iconSize / 4,
        iconSize / 6,
        iconSize / 8,
        bruceConfig.priColor
    );
}
