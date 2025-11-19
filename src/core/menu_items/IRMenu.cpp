#include "IRMenu.h"
#include "core/display.h"
#include "core/display_functions.h"
#include "core/utils.h"
#include <globals.h>

void IRMenu::optionsMenu() {
    options.clear();
    options.emplace_back("IR Learning", []() {
        // TODO: Implement IR learning
    });
    options.emplace_back("IR Emission", []() {
        // TODO: Implement IR emission
    });
    options.emplace_back("IR Database", []() {
        // TODO: Implement IR database
    });
    addOptionToMainMenu();

    loopOptions(options, MENU_TYPE_SUBMENU, "Infrared");
}

void IRMenu::drawIconImg() {
    drawImgFromFS(
        *bruceConfig.themeFS(), bruceConfig.getThemeItemImg(bruceConfig.theme.paths.ir), 0, imgCenterY, true
    );
}

void IRMenu::drawIcon(float scale) {
    clearIconArea();
    int iconSize = scale * 40;

    // IR LED representation
    int centerX = iconCenterX;
    int centerY = iconCenterY;

    // LED body
    tft.fillCircle(centerX, centerY, iconSize / 3, bruceConfig.priColor);

    // LED lens
    tft.fillCircle(centerX, centerY, iconSize / 6, bruceConfig.bgColor);

    // IR signal waves
    for (int i = 0; i < 3; i++) {
        tft.drawArc(
            centerX,
            centerY,
            iconSize / 2 + i * 5,
            iconSize / 3 + i * 3,
            0,
            360,
            bruceConfig.priColor,
            bruceConfig.bgColor
        );
    }

    // IR beams
    for (int i = 0; i < 5; i++) {
        float angle = -30 + i * 15;
        float x1 = centerX + cos(angle * PI / 180) * iconSize / 3;
        float y1 = centerY + sin(angle * PI / 180) * iconSize / 3;
        float x2 = centerX + cos(angle * PI / 180) * iconSize;
        float y2 = centerY + sin(angle * PI / 180) * iconSize;

        tft.drawLine(x1, y1, x2, y2, bruceConfig.priColor);
    }
}
