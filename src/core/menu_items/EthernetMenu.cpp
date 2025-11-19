#include "EthernetMenu.h"
#include "core/display.h"
#include "core/display_functions.h"
#include "core/utils.h"
#include <globals.h>

void EthernetMenu::optionsMenu() {
    options.clear();
    options.emplace_back("Ethernet Status", []() {
        // TODO: Implement Ethernet status
    });
    options.emplace_back("Ethernet Settings", []() {
        // TODO: Implement Ethernet settings
    });
    options.emplace_back("Network Diagnostics", []() {
        // TODO: Implement network diagnostics
    });
    addOptionToMainMenu();

    loopOptions(options, MENU_TYPE_SUBMENU, "Ethernet");
}

void EthernetMenu::drawIconImg() {
    drawImgFromFS(
        *bruceConfig.themeFS(), bruceConfig.getThemeItemImg(bruceConfig.theme.paths.rfid), 0, imgCenterY, true
    );
}

void EthernetMenu::drawIcon(float scale) {
    clearIconArea();
    int iconSize = scale * 40;

    // Ethernet port representation
    int centerX = iconCenterX;
    int centerY = iconCenterY;

    // Port body
    tft.fillRoundRect(
        centerX - iconSize / 3,
        centerY - iconSize / 4,
        iconSize * 2 / 3,
        iconSize / 2,
        iconSize / 8,
        bruceConfig.priColor
    );

    // Port connector
    tft.fillRect(
        centerX - iconSize / 4, centerY - iconSize / 6, iconSize / 2, iconSize / 12, bruceConfig.bgColor
    );

    // Port pins
    for (int i = 0; i < 8; i++) {
        int x = centerX - iconSize / 4 + (i * iconSize / 14);
        tft.fillCircle(x, centerY, 2, bruceConfig.priColor);
    }
}
