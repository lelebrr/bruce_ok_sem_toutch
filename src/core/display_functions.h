#pragma once

#include <globals.h>
#include <vector>

// Declaration of missing display and utility functions

void drawMainBorder(bool withTitle = false);
void drawMainBorderWithTitle(String title);
void TouchFooter();
void drawStatusBar();

void padprintln(String msg = "");
void padprint(String msg = "");
void displayError(String msg);
void displaySuccess(String msg);
void displayInfo(String msg);

void progressHandler(float current, float total, String text = "");

void drawImg(String path, int x = 0, int y = 0, bool center = false, int ms = 0);
void drawImg(fs::FS &fs, String path, int x = 0, int y = 0, bool center = false, int ms = 0);

int loopOptions(
    std::vector<Option> &options, int menuType = MENU_TYPE_MAIN, String title = "", int defaultOption = 0
);
