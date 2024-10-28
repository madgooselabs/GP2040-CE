// MPU6886 Library
// category=Signal Input/Output

// Written by:
//   Mike Parks <mikepparks@gmail.com>

#ifndef _MPU6886_H_
#define _MPU6886_H_

#include <vector>

#include "i2cdevicebase.h"

#define MPU6886_ID          0x19
#define MPU6886_ACCEL_RES    8.0 // +-8G
#define MPU6886_GYRO_RES  2000.0 // +-2000 degrees/sec
#define MPU6886_ROOM_TEMP   25.0 // 25°C
#define MPU6886_TEMP_SENS  326.8 // sensor temp sensitivity

class MPU6886 : public I2CDeviceBase {
    public:
        typedef enum {
            SMPLRT_DIV = 0x19,
            CONFIG = 0x1A,
            GYRO_CONFIG = 0x1B,
            ACCEL_CONFIG_1 = 0x1C,
            ACCEL_CONFIG_2 = 0x1D,
            FIFO_EN = 0x23,
            INT_PIN_CFG = 0x37,
            INT_ENABLE = 0x38,

            ACCEL_XOUT_H = 0x3B,
            ACCEL_XOUT_L = 0x3C,
            ACCEL_YOUT_H = 0x3D,
            ACCEL_YOUT_L = 0x3E,
            ACCEL_ZOUT_H = 0x3F,
            ACCEL_ZOUT_L = 0x40,

            TEMP_OUT_H = 0x41,
            TEMP_OUT_L = 0x42,

            GYRO_XOUT_H = 0x43,
            GYRO_XOUT_L = 0x44,
            GYRO_YOUT_H = 0x45,
            GYRO_YOUT_L = 0x46,
            GYRO_ZOUT_H = 0x47,
            GYRO_ZOUT_L = 0x48,

            USER_CTRL = 0x6A,
            PWR_MGMT_1 = 0x6B,
            PWR_MGMT_2 = 0x6C,
            WHOAMI = 0x75
        } MPU6886_REGISTERS;

        // Constructor
        MPU6886() {}
        MPU6886(PeripheralI2C *i2cController, uint8_t addr = 0x68) {
            this->i2c = i2cController;
            this->address = addr;
        }

        std::vector<uint8_t> getDeviceAddresses() const override {
            return {0x68, 0x69};
        }

        //// Methods
        void begin();
        void reset();
        //void start();

        void setI2C(PeripheralI2C *i2cController) { this->i2c = i2cController; }
        void setAddress(uint8_t addr) { this->address = addr; }

        uint8_t read(uint8_t reg);
        void write(uint8_t reg, uint8_t val);

        uint8_t getID();
        void accelerometer(float* x, float* y, float* z);
        void gyroscope(float* x, float* y, float* z);
        void temperature(float* t);
    private:
    protected:
        PeripheralI2C* i2c = nullptr;
        uint8_t address = 0;

        bool identified = false;
};

#endif
