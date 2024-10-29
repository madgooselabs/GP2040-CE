#include "lsm6dso.h"

#include <cstring>

void LSM6DSO::begin() {
    reset();
}

void LSM6DSO::reset(){
    identified = false;

    if (getID() == LSM6DSO_ID) {
        identified = true;

        // Accelerometer full scale +-8G
        write(LSM6DSO_REGISTERS::CTRL1_XL, 0x8C);

        // Gyro full scale 2000 degrees per second
        write(LSM6DSO_REGISTERS::CTRL2_G, 0x8C);

        // No increment register address
        write(LSM6DSO_REGISTERS::CTRL3_C, 0x00);
    }
}

uint8_t LSM6DSO::getID() {
    return read(LSM6DSO_REGISTERS::WHO_AM_I);
}

void LSM6DSO::accelerometer(float* x, float* y, float* z) {
    if (identified) {
        float accelRes = 2.0 / 32768.0;
        *x = (int16_t)((read(LSM6DSO_REGISTERS::OUTX_H_A) << 8) | read(LSM6DSO_REGISTERS::OUTX_L_A)) * accelRes;
        *y = (int16_t)((read(LSM6DSO_REGISTERS::OUTY_H_A) << 8) | read(LSM6DSO_REGISTERS::OUTY_L_A)) * accelRes;
        *z = (int16_t)((read(LSM6DSO_REGISTERS::OUTZ_H_A) << 8) | read(LSM6DSO_REGISTERS::OUTZ_L_A)) * accelRes;
    } else {
        *x = 0;
        *y = 0;
        *z = 0;
    }
}

void LSM6DSO::gyroscope(float* x, float* y, float* z) {
    if (identified) {
        float gyroRes = 250.0 / 32768.0;
        *x = (int16_t)((read(LSM6DSO_REGISTERS::OUTX_H_G) << 8) | read(LSM6DSO_REGISTERS::OUTX_L_G)) * gyroRes;
        *y = (int16_t)((read(LSM6DSO_REGISTERS::OUTY_H_G) << 8) | read(LSM6DSO_REGISTERS::OUTY_L_G)) * gyroRes;
        *z = (int16_t)((read(LSM6DSO_REGISTERS::OUTZ_H_G) << 8) | read(LSM6DSO_REGISTERS::OUTZ_L_G)) * gyroRes;
    } else {
        *x = 0;
        *y = 0;
        *z = 0;
    }
}

uint8_t LSM6DSO::read(uint8_t reg) {
    uint8_t val;
    i2c->write(address, &reg, 1);
    i2c->read(address, &val, 1);
    return val;
}

void LSM6DSO::write(uint8_t reg, uint8_t val) {
    uint8_t writeBytes[2] = {reg, val};
    i2c->write(address, writeBytes, 2);
}