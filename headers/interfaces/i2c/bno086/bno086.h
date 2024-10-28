// BNO086 Library
// category=Signal Input/Output

// Written by:
//   Mike Parks <mikepparks@gmail.com>

#ifndef _BNO086_H_
#define _BNO086_H_

#include <vector>

#include "i2cdevicebase.h"

class BNO086 : public I2CDeviceBase {
    public:
        // Constructor
        BNO086() {}
        BNO086(PeripheralI2C *i2cController, uint8_t addr = 0x4A) {
            this->i2c = i2cController;
            this->address = addr;
        }

        std::vector<uint8_t> getDeviceAddresses() const override {
            return {0x4A, 0x4B};
        }

        //// Methods
        void begin();
        void reset();
        //void start();

        void setI2C(PeripheralI2C *i2cController) { this->i2c = i2cController; }
        void setAddress(uint8_t addr) { this->address = addr; }
    private:

    protected:
        PeripheralI2C* i2c = nullptr;
        uint8_t address = 0;
};

#endif
