#include "FileMenu.h"
#include "core/display.h"
#include "core/display_functions.h"
#include "core/utils.h"
#include <globals.h>

void FileMenu::optionsMenu() {
    options.clear();
    options.emplace_back("Browse SD", []() {
        // TODO: Implement SD card browsing
    });
    options.emplace_back("Browse LittleFS", []() {
        // TODO: Implement LittleFS browsing
    });
    options.emplace_back("File Operations", []() {
        // TODO: Implement file operations
    });
    addOptionToMainMenu();

    loopOptions(options, MENU_TYPE_SUBMENU, "File Manager");
}

void FileMenu::drawIconImg() {
    if (bruceConfig.theme.fs) {
        drawImgFromFS(SD, bruceConfig.getThemeItemImg(bruceConfig.theme.paths.files), 0, imgCenterY, true);
    } else {
        drawImgFromFS(
            LittleFS, bruceConfig.getThemeItemImg(bruceConfig.theme.paths.files), 0, imgCenterY, true
        );
    }
}

void FileMenu::drawIcon(float scale) {
    clearIconArea();
    int iconSize = scale * 40;

    // Folder icon
    tft.fillRoundRect(
        iconCenterX - iconSize / 2,
        iconCenterY - iconSize / 3,
        iconSize,
        iconSize * 2 / 3,
        iconSize / 8,
        bruceConfig.priColor
    );

    // Folder tab
    tft.fillRect(
        iconCenterX - iconSize / 2,
        iconCenterY - iconSize / 3,
        iconSize * 3 / 4,
        iconSize / 6,
        bruceConfig.priColor
    );

    // Folder lines
    tft.fillRect(
        iconCenterX - iconSize / 3, iconCenterY - iconSize / 6, iconSize * 2 / 3, 2, bruceConfig.bgColor
    );
    tft.fillRect(iconCenterX - iconSize / 3, iconCenterY, iconSize * 2 / 3, 2, bruceConfig.bgColor);
}
