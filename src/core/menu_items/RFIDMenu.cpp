#include "RFIDMenu.h"
#include "core/display.h"
#include "core/display_functions.h"
#include "core/utils.h"
#include <globals.h>

void RFIDMenu::optionsMenu() {
    options.clear();
    options.emplace_back("Read RFID", []() {
        // TODO: Implement RFID reading
    });
    options.emplace_back("Write RFID", []() {
        // TODO: Implement RFID writing
    });
    options.emplace_back("Clone RFID", []() {
        // TODO: Implement RFID cloning
    });
    addOptionToMainMenu();

    loopOptions(options, MENU_TYPE_SUBMENU, "RFID");
}

void RFIDMenu::drawIconImg() {
    drawImgFromFS(
        (fs::FS &)(*bruceConfig.themeFS()),
        bruceConfig.getThemeItemImg(bruceConfig.theme.paths.rfid),
        0,
        imgCenterY,
        true
    );
}

void RFIDMenu::drawIcon(float scale) {
    clearIconArea();
    int iconSize = scale * 40;

    // RFID card representation
    tft.fillRoundRect(
        iconCenterX - iconSize / 2,
        iconCenterY - iconSize / 3,
        iconSize,
        iconSize * 2 / 3,
        iconSize / 8,
        bruceConfig.priColor
    );

    // RFID chip representation
    tft.fillRoundRect(
        iconCenterX - iconSize / 4,
        iconCenterY - iconSize / 6,
        iconSize / 2,
        iconSize / 3,
        iconSize / 16,
        bruceConfig.bgColor
    );

    // RFID waves
    for (int i = 0; i < 3; i++) {
        tft.drawArc(
            iconCenterX + iconSize / 2 + i * 5,
            iconCenterY,
            5 + i * 3,
            3 + i * 2,
            45,
            135,
            bruceConfig.priColor,
            bruceConfig.bgColor
        );
    }
}
