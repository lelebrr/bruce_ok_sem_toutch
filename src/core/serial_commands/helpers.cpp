#include "helpers.h"

bool _setupPsramFs() {
    // Assume LittleFS is already initialized in main.cpp
    return true;
}

char *_readFileFromSerial(size_t fileSizeChar) {
    char *buf;
    size_t bufSize = 0;
    if (psramFound()) buf = (char *)ps_malloc((fileSizeChar) * sizeof(char));
    else buf = (char *)malloc((fileSizeChar) * sizeof(char));
    if (buf == NULL) {
        Serial.printf("Could not allocate %d\n", fileSizeChar);
        return NULL;
    }
    buf[0] = '\0';

    String currLine = "";
    Serial.println("Reading input data from serial buffer until EOF");
    Serial.flush();
    while (true) {
        if (!Serial.available()) {
            delay(10);
            continue;
        }
        currLine = Serial.readStringUntil('\n');
        if (currLine.startsWith("EOF")) break;
        size_t lineLength = currLine.length();
        if ((bufSize + lineLength + 1) > fileSizeChar) break;

        memcpy(buf + bufSize, currLine.c_str(), lineLength);
        bufSize += lineLength;
        buf[bufSize++] = '\n';
    }
    buf[bufSize] = '\0';
    return buf;
}
