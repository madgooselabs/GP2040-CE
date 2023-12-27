#ifndef _GPGFX_H_
#define _GPGFX_H_

#include "OneBitDisplay.h" // to be removed

#include "pico/stdlib.h"
#include "peripheral_i2c.h"
#include "peripheral_spi.h"

class GPGFX {
    public:
        typedef enum {
            SSD1306
        } DisplayType;

        typedef struct {
            PeripheralI2C* i2c;
            PeripheralSPI* spi;
            uint8_t size;
            uint16_t address;
            uint8_t orientation;
            bool inverted;
        } DisplayTypeOptions;

        GPGFX();

        void init(DisplayType display, DisplayTypeOptions* options);
    private:
};

#endif
