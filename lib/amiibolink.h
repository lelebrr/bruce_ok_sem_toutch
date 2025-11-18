// Stub header for amiibolink library
// Add the real library to platformio.ini lib_deps if needed
// For now, providing empty definitions to allow compilation

#ifndef __AMIIBOLINK_H__
#define __AMIIBOLINK_H__

#include <stdint.h>

class AmiiboLink {
public:
    AmiiboLink();
    ~AmiiboLink();
    bool init();
    bool detect();
    bool writeAmiibo(const uint8_t *data);
    bool readAmiibo(uint8_t *buffer);
};

#endif
