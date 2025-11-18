// Stub header for ELECHOUSE_CC1101_SRC_DRV library
// Add the real library to platformio.ini lib_deps if needed
// For now, providing empty definitions to allow compilation

#ifndef __ELECHOUSE_CC1101_SRC_DRV_H__
#define __ELECHOUSE_CC1101_SRC_DRV_H__

#include <stdint.h>

class ELECHOUSE_CC1101_SRC_DRV {
public:
    static void init();
    static void setDevAddress(uint8_t addr);
    static void sendData(uint8_t *data, int len);
    static int receiveData(uint8_t *buffer);
};

#endif
