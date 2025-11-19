#ifndef __SERIAL_HELPERS_H__
#define __SERIAL_HELPERS_H__

// Ensure critical macros are defined
#ifndef SAFE_STACK_BUFFER_SIZE
#define SAFE_STACK_BUFFER_SIZE 4096
#endif

#include <Arduino.h>
#include <LittleFS.h>
extern FS &LittleFS;
FS &PSRamFS = LittleFS;
// #include <PSRamFS.h>

bool _setupPsramFs();
char *_readFileFromSerial(size_t fileSizeChar = SAFE_STACK_BUFFER_SIZE);

#endif
