#ifndef _GPGFX_TYPES_H_
#define _GPGFX_TYPES_H_

#include "peripheral_i2c.h"
#include "peripheral_spi.h"

typedef enum {
    SSD1306
} GPGFX_DisplayType;

typedef struct {
    uint8_t width;
    uint8_t height;
    uint8_t* fontData;
} GPGFX_DisplayFont;

typedef struct {
    GPGFX_DisplayType displayType;
    PeripheralI2C* i2c;
    PeripheralSPI* spi;
    uint8_t size;
    uint16_t address;
    uint8_t orientation;
    bool inverted;
    GPGFX_DisplayFont font;
} GPGFX_DisplayTypeOptions;

#endif