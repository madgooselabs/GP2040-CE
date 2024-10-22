// MPR121 Library
// category=Signal Input/Output

// Written by:
//   Mike Parks <mikepparks@gmail.com>

#ifndef _MPR121_H_
#define _MPR121_H_

#include <vector>

#include "i2cdevicebase.h"

class MPR121 : public I2CDeviceBase {
    public:
        // Constructor
        MPR121() {}
        MPR121(PeripheralI2C *i2cController, uint8_t addr = 0x5A) {
            this->i2c = i2cController;
            this->address = addr;
        }

        std::vector<uint8_t> getDeviceAddresses() const override {
            return {0x5A, 0x5B, 0x5C, 0x5D};
        }

        //// Methods
        void begin();
        void reset();
        void start();

        void setI2C(PeripheralI2C *i2cController) { this->i2c = i2cController; }
        void setAddress(uint8_t addr) { this->address = addr; }
    private:

    protected:
        PeripheralI2C* i2c = nullptr;
        uint8_t address = 0;
};

#endif
