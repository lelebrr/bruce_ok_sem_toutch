#include "display_functions.h"
#ifdef HAS_SCREEN

// Basic implementations for the missing functions

void drawMainBorder(bool withTitle) {
    if (withTitle) {
        tft.fillRect(5, 5, tftWidth - 10, 20, TFT_BLACK);
        tft.drawRect(5, 5, tftWidth - 10, tftHeight - 10, TFT_WHITE);
    } else {
        tft.drawRect(5, 5, tftWidth - 10, tftHeight - 10, TFT_WHITE);
    }
}

void drawMainBorderWithTitle(String title) {
    drawMainBorder(true);
    tft.setCursor(10, 10);
    tft.println(title);
}

void TouchFooter() {
    // Draw touch footer for buttons
    tft.fillRect(0, tftHeight - 30, tftWidth, 30, TFT_BLACK);
    tft.setCursor(10, tftHeight - 25);
    tft.println("SEL | BACK");
}

void drawStatusBar() {
    // Draw status bar with battery, wifi, etc.
    tft.fillRect(0, 0, tftWidth, 20, TFT_BLACK);
    tft.setCursor(5, 5);
    tft.print("Status: OK");
}

void padprintln(String msg) { tft.println(msg); }

void padprint(String msg) { tft.print(msg); }

void displayError(String msg, bool wait) {
    tft.setTextColor(TFT_RED);
    tft.println("Error: " + msg);
    tft.setTextColor(TFT_WHITE);
    if (wait) {
        while (!check(AnyKeyPress)) { delay(10); }
    }
}

void displaySuccess(String msg, bool wait) {
    tft.setTextColor(TFT_GREEN);
    tft.println("Success: " + msg);
    tft.setTextColor(TFT_WHITE);
    if (wait) {
        while (!check(AnyKeyPress)) { delay(10); }
    }
}

void displayInfo(String msg) { tft.println("Info: " + msg); }

void progressHandler(float current, float total, String text) {
    float percent = current / total * 100;
    tft.printf("%s: %.1f%%\n", text.c_str(), percent);
    tft.drawRect(10, tftHeight - 50, tftWidth - 20, 10, TFT_WHITE);
    tft.fillRect(10, tftHeight - 50, (tftWidth - 20) * (current / total), 10, TFT_BLUE);
}

// Additional missing function implementations
void displayRedStripe(String msg) {
    tft.fillRect(0, tftHeight - 40, tftWidth, 40, TFT_RED);
    tft.setCursor(10, tftHeight - 20);
    tft.setTextColor(TFT_WHITE);
    tft.println(msg);
    tft.setTextColor(TFT_WHITE);
}

void displayScrollingText(String text, Opt_Coord &coord) {
    // Basic implementation for scrolling text
    tft.setCursor(coord.x, coord.y);
    tft.println(text);
}

void printTitle(String title) {
    tft.setCursor(10, 10);
    tft.setTextColor(TFT_YELLOW);
    tft.println(title);
    tft.setTextColor(TFT_WHITE);
}

void padprintf(const char *format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    tft.print(buffer);
}

Opt_Coord listFiles(int index, std::vector<FileList> &fileList) {
    Opt_Coord coord;
    coord.x = 10;
    coord.y = 30;

    int displayHeight = tftHeight - 50; // Space for footer and margins
    int itemsPerPage = displayHeight / (LH * FM);
    int startIndex = std::max(0, index - itemsPerPage / 2);
    int endIndex = std::min((int)fileList.size() - 1, startIndex + itemsPerPage - 1);

    // Adjust startIndex if we're near the end
    if (endIndex - startIndex + 1 < itemsPerPage && startIndex > 0) {
        startIndex = std::max(0, endIndex - itemsPerPage + 1);
    }

    // Display files
    for (int i = startIndex; i <= endIndex && i < (int)fileList.size(); i++) {
        if (i == index) {
            tft.setTextColor(TFT_YELLOW);
            tft.fillRect(coord.x - 5, coord.y - 2, tftWidth - 20, LH * FM, bruceConfig.bgColor);
        } else {
            tft.setTextColor(bruceConfig.priColor);
        }

        tft.setCursor(coord.x, coord.y);
        if (fileList[i].folder) {
            tft.print("📁 ");
        } else {
            tft.print("📄 ");
        }
        tft.println(fileList[i].filename);

        coord.y += LH * FM;
    }

    return coord;
}

void drawImg(String path, int x, int y, bool center, int ms) {
    // Basic implementation
    tft.drawString(path, x, y); // Placeholder
}

void drawImgFromFS(fs::FS &fs, String path, int x, int y, bool center, int ms) {
    // Implementation using tft_logger imageToBin
    uint8_t fs_type = 0; // Default to SD
    if (&fs == &LittleFS) fs_type = 1;
    tft.imageToBin(fs_type, path, x, y, center, ms);
}

int loopOptions(std::vector<Option> &opts, int menuType, String title, int defaultOption) {
    int currentOption = defaultOption;

    if (!opts.empty()) { opts[currentOption].selected = true; }

    while (true) {
        tft.fillScreen(TFT_BLACK);
        drawMainBorderWithTitle(title);

        int y = 30;
        for (size_t i = 0; i < opts.size(); i++) {
            if (i == currentOption) {
                tft.setTextColor(TFT_YELLOW);
            } else {
                tft.setTextColor(TFT_WHITE);
            }
            tft.setCursor(10, y);
            tft.println(opts[i].label);
            y += 20;
        }

        // Wait for input
        if (check(NextPress)) {
            if (currentOption < opts.size() - 1) {
                opts[currentOption].selected = false;
                currentOption++;
                opts[currentOption].selected = true;
            }
        } else if (check(PrevPress)) {
            if (currentOption > 0) {
                opts[currentOption].selected = false;
                currentOption--;
                opts[currentOption].selected = true;
            }
        } else if (check(SelPress)) {
            opts[currentOption].operation();
            return currentOption;
        } else if (check(EscPress)) {
            returnToMenu = true;
            return -1;
        }

        delay(100);
    }

    return currentOption;
}

#endif // HAS_SCREEN

#ifdef NO_SCREEN

// Serial versions for no screen
void drawMainBorder(bool withTitle) { Serial.println("--- Border ---"); }
void drawMainBorderWithTitle(String title) { Serial.println(title); }
void TouchFooter() {}
void drawStatusBar() {}
void padprintln(String msg) { Serial.println(msg); }
void padprint(String msg) { Serial.print(msg); }
void displayError(String msg, bool wait) { Serial.println("Error: " + msg); }
void displaySuccess(String msg, bool wait) { Serial.println("Success: " + msg); }
void displayInfo(String msg) { Serial.println("Info: " + msg); }
void progressHandler(float current, float total, String text) {
    Serial.printf("%s: %.1f%%\n", text.c_str(), current / total * 100);
}
void drawImg(String path, int x, int y, bool center, int ms) {}

void drawImgFromFS(fs::FS &fs, String path, int x, int y, bool center, int ms) {}

int loopOptions(std::vector<Option> &opts, int menuType, String title, int defaultOption) {
    int currentOption = defaultOption;

    Serial.println(title);
    for (size_t i = 0; i < opts.size(); i++) { Serial.printf("%d: %s\n", i + 1, opts[i].label.c_str()); }

    while (true) {
        Serial.println("Enter option number (1-" + String(opts.size()) + "):");
        delay(1000); // Wait for input, in real implementation read from serial

        // Placeholder: select first option
        if (currentOption >= 0 && currentOption < opts.size()) {
            opts[currentOption].operation();
            return currentOption;
        }
    }

    return 0;
}

#endif
