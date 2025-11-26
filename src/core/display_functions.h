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
void displayError(String msg, bool wait = false);
void displaySuccess(String msg, bool wait = false);
void displayInfo(String msg);

void progressHandler(float current, float total, String text = "");

void drawImg(String path, int x = 0, int y = 0, bool center = false, int ms = 0);
void drawImgFromFS(fs::FS &fs, String path, int x = 0, int y = 0, bool center = false, int ms = 0);

int loopOptions(
    std::vector<Option> &options, int menuType = MENU_TYPE_MAIN, String title = "", int defaultOption = 0
);

// Additional missing functions
void displayRedStripe(String msg);
void displayTextLine(String msg);
void displayScrollingText(String text, Opt_Coord &coord);
void printTitle(String title);
void padprintf(const char *format, ...);

// Function for listing files in file system
Opt_Coord listFiles(int index, std::vector<FileList> &fileList);
