// LSM6DSO Library
// category=Signal Input/Output

// Written by:
//   Mike Parks <mikepparks@gmail.com>

#ifndef _LSM6DSO_H_
#define _LSM6DSO_H_

#include <vector>

#include "i2cdevicebase.h"

#define LSM6DS33_ID         0x69
#define LSM6DSO_ID          0x6C

class LSM6DSO : public I2CDeviceBase {
    public:
        typedef enum {
            FUNC_CFG_ACCESS = 0x01,
            PIN_CTRL = 0x02,
            FIFO_CTRL1 = 0x07,
            FIFO_CTRL2 = 0x08,
            FIFO_CTRL3 = 0x09,
            FIFO_CTRL4 = 0x0A,
            COUNTER_BDR_REG1 = 0x0B,
            COUNTER_BDR_REG2 = 0x0C,
            INT1_CTRL = 0x0D,
            INT2_CTRL = 0x0E,
            WHO_AM_I = 0x0F,
            CTRL1_XL = 0x10,
            CTRL2_G = 0x11,
            CTRL3_C = 0x12,
            CTRL4_C = 0x13,
            CTRL5_C = 0x14,
            CTRL6_C = 0x15,
            CTRL7_G = 0x16,
            CTRL8_XL = 0x17,
            CTRL9_XL = 0x18,
            CTRL10_C = 0x19,
            ALL_INT_SRC = 0x1A,
            WAKE_UP_SRC = 0x1B,
            TAP_SRC = 0x1C,
            D6D_SRC = 0x1D,
            STATUS_REG = 0x1E,
            OUT_TEMP_L = 0x20,
            OUT_TEMP_H = 0x21,
            OUTX_L_G = 0x22,
            OUTX_H_G = 0x23,
            OUTY_L_G = 0x24,
            OUTY_H_G = 0x25,
            OUTZ_L_G = 0x26,
            OUTZ_H_G = 0x27,
            OUTX_L_A = 0x28,
            OUTX_H_A = 0x29,
            OUTY_L_A = 0x2A,
            OUTY_H_A = 0x2B,
            OUTZ_L_A = 0x2C,
            OUTZ_H_A = 0x2D,
            EMB_FUNC_STATUS_MAINPAGE = 0x35,
            FSM_STATUS_A_MAINPAGE = 0x36,
            FSM_STATUS_B_MAINPAGE = 0x37,
            STATUS_MASTER_MAINPAGE = 0x39,
            FIFO_STATUS1 = 0x3A,
            FIFO_STATUS2 = 0x3B,
            TIMESTAMP0 = 0x40,
            TIMESTAMP1 = 0x41,
            TIMESTAMP2 = 0x42,
            TIMESTAMP3 = 0x43,
            TAP_CFG0 = 0x56,
            TAP_CFG1 = 0x57,
            TAP_CFG2 = 0x58,
            TAP_THS_6D = 0x59,
            INT_DUR2 = 0x5A,
            WAKE_UP_THS = 0x5B,
            WAKE_UP_DUR = 0x5C,
            FREE_FALL = 0x5D,
            MD1_CFG = 0x5E,
            MD2_CFG = 0x5F,
            I3C_BUS_AVB = 0x62,
            INTERNAL_FREQ_FINE = 0x63,
            INT_OIS = 0x6F,
            CTRL1_OIS = 0x70,
            CTRL2_OIS = 0x71,
            CTRL3_OIS = 0x72,
            X_OFS_USR = 0x73,
            Y_OFS_USR = 0x74,
            Z_OFS_USR = 0x75,
            FIFO_DATA_OUT_TAG = 0x78,
            FIFO_DATA_OUT_X_L = 0x79,
            FIFO_DATA_OUT_X_H = 0x7A,
            FIFO_DATA_OUT_Y_L = 0x7B,
            FIFO_DATA_OUT_Y_H = 0x7C,
            FIFO_DATA_OUT_Z_L = 0x7D,
            FIFO_DATA_OUT_Z_H = 0x7E
        } LSM6DSO_REGISTERS;

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

        void setI2C(PeripheralI2C *i2cController) { this->i2c = i2cController; }
        void setAddress(uint8_t addr) { this->address = addr; }

        uint8_t read(uint8_t reg);
        void write(uint8_t reg, uint8_t val);

        uint8_t getID();
        void accelerometer(float* x, float* y, float* z);
        void gyroscope(float* x, float* y, float* z);
    private:

    protected:
        PeripheralI2C* i2c = nullptr;
        uint8_t address = 0;

        bool identified = false;
};

#endif
