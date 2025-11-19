#include "ConnectMenu.h"
#include "core/display.h"
#include "core/display_functions.h"
#include "core/utils.h"
#include <globals.h>

void ConnectMenu::optionsMenu() {
    options.clear();
    options.emplace_back("WiFi", []() {
        // TODO: Implement WiFi menu
    });
    options.emplace_back("Bluetooth", []() {
        // TODO: Implement Bluetooth menu
    });
    options.emplace_back("Ethernet", []() {
        // TODO: Implement Ethernet menu
    });
    options.emplace_back("USB", []() {
        // TODO: Implement USB menu
    });
    addOptionToMainMenu();

    loopOptions(options, MENU_TYPE_SUBMENU, "Connections");
}

void ConnectMenu::drawIconImg() {
    drawImgFromFS(
        *bruceConfig.themeFS(),
        bruceConfig.getThemeItemImg(bruceConfig.theme.paths.connect),
        0,
        imgCenterY,
        true
    );
}

void ConnectMenu::drawIcon(float scale) {
    clearIconArea();
    int iconSize = scale * 40;

    // Connection nodes
    tft.fillCircle(
        iconCenterX - iconSize / 3, iconCenterY - iconSize / 4, iconSize / 6, bruceConfig.priColor
    );
    tft.fillCircle(
        iconCenterX + iconSize / 3, iconCenterY - iconSize / 4, iconSize / 6, bruceConfig.priColor
    );
    tft.fillCircle(
        iconCenterX - iconSize / 3, iconCenterY + iconSize / 4, iconSize / 6, bruceConfig.priColor
    );
    tft.fillCircle(
        iconCenterX + iconSize / 3, iconCenterY + iconSize / 4, iconSize / 6, bruceConfig.priColor
    );

    // Connection lines
    tft.drawLine(
        iconCenterX - iconSize / 3,
        iconCenterY - iconSize / 4,
        iconCenterX + iconSize / 3,
        iconCenterY - iconSize / 4,
        bruceConfig.priColor
    );
    tft.drawLine(
        iconCenterX - iconSize / 3,
        iconCenterY + iconSize / 4,
        iconCenterX + iconSize / 3,
        iconCenterY + iconSize / 4,
        bruceConfig.priColor
    );
    tft.drawLine(
        iconCenterX - iconSize / 3,
        iconCenterY - iconSize / 4,
        iconCenterX - iconSize / 3,
        iconCenterY + iconSize / 4,
        bruceConfig.priColor
    );
    tft.drawLine(
        iconCenterX + iconSize / 3,
        iconCenterY - iconSize / 4,
        iconCenterX + iconSize / 3,
        iconCenterY + iconSize / 4,
        bruceConfig.priColor
    );
}
