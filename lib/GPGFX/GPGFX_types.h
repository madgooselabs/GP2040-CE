#ifndef _GPGFX_TYPES_H_
#define _GPGFX_TYPES_H_

#include "peripheral_i2c.h"
#include "peripheral_spi.h"

// to retain legacy display sizes
typedef enum {
    SIZE_128x128 = 1,
    SIZE_128x32,
    SIZE_128x64,
    SIZE_132x64,
    SIZE_64x128,
} GPGFX_DisplaySize;

typedef enum {
    TYPE_NONE,
    TYPE_SSD1306
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