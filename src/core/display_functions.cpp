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

void displayError(String msg) {
    tft.setTextColor(TFT_RED);
    tft.println("Error: " + msg);
    tft.setTextColor(TFT_WHITE);
}

void displaySuccess(String msg) {
    tft.setTextColor(TFT_GREEN);
    tft.println("Success: " + msg);
    tft.setTextColor(TFT_WHITE);
}

void displayInfo(String msg) { tft.println("Info: " + msg); }

void progressHandler(float current, float total, String text) {
    float percent = current / total * 100;
    tft.printf("%s: %.1f%%\n", text.c_str(), percent);
    tft.drawRect(10, tftHeight - 50, tftWidth - 20, 10, TFT_WHITE);
    tft.fillRect(10, tftHeight - 50, (tftWidth - 20) * (current / total), 10, TFT_BLUE);
}

void drawImg(String path, int x, int y, bool center, int ms) {
    // Basic implementation
    tft.drawString(path, x, y); // Placeholder
}

void drawImg(fs::FS &fs, String path, int x, int y, bool center, int ms) {
    // Placeholder: in real implementation, use TFT_eSPI drawJpgFile or similar
    tft.drawString("drawImg from fs not implemented", x, y);
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
void displayError(String msg) { Serial.println("Error: " + msg); }
void displaySuccess(String msg) { Serial.println("Success: " + msg); }
void displayInfo(String msg) { Serial.println("Info: " + msg); }
void progressHandler(float current, float total, String text) {
    Serial.printf("%s: %.1f%%\n", text.c_str(), current / total * 100);
}
void drawImg(String path, int x, int y, bool center, int ms) {}

void drawImg(fs::FS &fs, String path, int x, int y, bool center, int ms) {}

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
