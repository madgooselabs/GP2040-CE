#include "mpu6886.h"

#include <cstring>

void MPU6886::begin() {
    reset();
}

void MPU6886::reset() {
    identified = false;

    if (getID() == MPU6886_ID) {
        identified = true;

        // Init
        write(MPU6886_REGISTERS::PWR_MGMT_1, 0x00);

        // Reset device
        write(MPU6886_REGISTERS::PWR_MGMT_1, 1 << 7);

        // Select clock
        write(MPU6886_REGISTERS::PWR_MGMT_1, 1 << 0);

        // Accelerometer scale to +-8G
        write(MPU6886_REGISTERS::ACCEL_CONFIG_1, 0x10);
        write(MPU6886_REGISTERS::ACCEL_CONFIG_2, 0x00);

        // Gyroscope scale to +-2000dps
        write(MPU6886_REGISTERS::GYRO_CONFIG, 0x18);

        write(MPU6886_REGISTERS::CONFIG, 0x01);

        write(MPU6886_REGISTERS::SMPLRT_DIV, 0x05);
        write(MPU6886_REGISTERS::INT_ENABLE, 0x00);

        write(MPU6886_REGISTERS::USER_CTRL, 0x00);

        write(MPU6886_REGISTERS::FIFO_EN, 0x00);

        write(MPU6886_REGISTERS::INT_PIN_CFG, 0x22);
        write(MPU6886_REGISTERS::INT_ENABLE, 0x01);
    }
}

uint8_t MPU6886::getID() {
    return read(MPU6886_REGISTERS::WHOAMI);
}

void MPU6886::accelerometer(float* x, float* y, float* z) {
    if (identified) {
        float accelRes = MPU6886_ACCEL_RES / 32768.0;
        *x = (int16_t)((read(MPU6886_REGISTERS::ACCEL_XOUT_H) << 8) | read(MPU6886_REGISTERS::ACCEL_XOUT_L)) * accelRes;
        *y = (int16_t)((read(MPU6886_REGISTERS::ACCEL_YOUT_H) << 8) | read(MPU6886_REGISTERS::ACCEL_YOUT_L)) * accelRes;
        *z = (int16_t)((read(MPU6886_REGISTERS::ACCEL_ZOUT_H) << 8) | read(MPU6886_REGISTERS::ACCEL_ZOUT_L)) * accelRes;
    } else {
        *x = 0;
        *y = 0;
        *z = 0;
    }
}

void MPU6886::gyroscope(float* x, float* y, float* z) {
    if (identified) {
        float gyroRes = MPU6886_GYRO_RES / 32768.0;
        *x = (int16_t)((read(MPU6886_REGISTERS::GYRO_XOUT_H) << 8) | read(MPU6886_REGISTERS::GYRO_XOUT_L)) * gyroRes;
        *y = (int16_t)((read(MPU6886_REGISTERS::GYRO_YOUT_H) << 8) | read(MPU6886_REGISTERS::GYRO_YOUT_L)) * gyroRes;
        *z = (int16_t)((read(MPU6886_REGISTERS::GYRO_ZOUT_H) << 8) | read(MPU6886_REGISTERS::GYRO_ZOUT_L)) * gyroRes;
    } else {
        *x = 0;
        *y = 0;
        *z = 0;
    }
}

void MPU6886::temperature(float* t) {
    *t = (identified ? MPU6886_ROOM_TEMP + ((read(MPU6886_REGISTERS::TEMP_OUT_H) << 8) | read(MPU6886_REGISTERS::TEMP_OUT_L)) / MPU6886_TEMP_SENS : 0);
}

uint8_t MPU6886::read(uint8_t reg) {
    uint8_t val;
    i2c->write(address, &reg, 1);
    i2c->read(address, &val, 1);
    return val;
}

void MPU6886::write(uint8_t reg, uint8_t val) {
    uint8_t writeBytes[2] = {reg, val};
    i2c->write(address, writeBytes, 2);
}