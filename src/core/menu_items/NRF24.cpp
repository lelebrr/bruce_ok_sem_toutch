#include "NRF24.h"
#include "core/display.h"
#include "core/display_functions.h"
#include "core/utils.h"
#include <globals.h>

void NRF24Menu::optionsMenu() {
    options.clear();
    options.emplace_back("RF24 Status", []() {
        // TODO: Implement RF24 status
    });
    options.emplace_back("RF24 Settings", []() {
        // TODO: Implement RF24 settings
    });
    options.emplace_back("RF24 Scan", []() {
        // TODO: Implement RF24 scanning
    });
    addOptionToMainMenu();

    loopOptions(options, MENU_TYPE_SUBMENU, "RF24");
}

void NRF24Menu::drawIconImg() {
    drawImgFromFS(
        *bruceConfig.themeFS(), bruceConfig.getThemeItemImg(bruceConfig.theme.paths.nrf), 0, imgCenterY, true
    );
}

void NRF24Menu::drawIcon(float scale) {
    clearIconArea();
    int iconSize = scale * 40;

    // RF24 module representation
    int centerX = iconCenterX;
    int centerY = iconCenterY;

    // Module body
    tft.fillRoundRect(
        centerX - iconSize / 3,
        centerY - iconSize / 4,
        iconSize * 2 / 3,
        iconSize / 2,
        iconSize / 8,
        bruceConfig.priColor
    );

    // Module pins
    for (int i = 0; i < 8; i++) {
        float angle = i * PI / 4;
        float x = centerX + cos(angle) * iconSize / 2;
        float y = centerY + sin(angle) * iconSize / 2;
        tft.fillCircle(x, y, 2, bruceConfig.priColor);
    }

    // Antenna
    tft.drawLine(centerX, centerY - iconSize / 2, centerX, centerY - iconSize, bruceConfig.priColor);

    // Signal waves
    for (int i = 0; i < 2; i++) {
        tft.drawArc(
            centerX,
            centerY - iconSize / 2 - i * 5,
            3 + i * 2,
            2 + i,
            45,
            135,
            bruceConfig.priColor,
            bruceConfig.bgColor
        );
    }
}
