// LSM6DSO Library
// category=Signal Input/Output

// Written by:
//   Mike Parks <mikepparks@gmail.com>

#ifndef _LSM6DSO_H_
#define _LSM6DSO_H_

#include <vector>

#include "i2cdevicebase.h"

class LSM6DSO : public I2CDeviceBase {
    public:
        // Constructor
        LSM6DSO() {}
        LSM6DSO(PeripheralI2C *i2cController, uint8_t addr = 0x6A) {
            this->i2c = i2cController;
            this->address = addr;
        }

        std::vector<uint8_t> getDeviceAddresses() const override {
            return {0x6A, 0x6B};
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
