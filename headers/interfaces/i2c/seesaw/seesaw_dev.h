// Seesaw Library
// category=Signal Input/Output

// Written by:
//   Mike Parks <mikepparks@gmail.com>

#ifndef _SEESAW_DEVICE_H_
#define _SEESAW_DEVICE_H_

#include <vector>

#include "i2cdevicebase.h"

#ifndef SEESAW_DELAY
#define SEESAW_DELAY 1000
#endif

//#define SEESAW_I2C_DEBUG

typedef enum {
    SEESAW_STATUS_BASE = 0x00,
    SEESAW_STATUS_HW_ID = 0x01,
    SEESAW_STATUS_VERSION = 0x02,
    SEESAW_STATUS_OPTIONS = 0x03,
    SEESAW_STATUS_TEMP = 0x04,
    SEESAW_STATUS_SWRST = 0x7F,
} SeesawCommands;

typedef enum {
    CRICKIT_PID          = 9999,
    ROBOHATMM1_PID       = 9998,
    SAMD09_HW_ID_CODE    = 0x55,
    ATTINY8X7_HW_ID_CODE = 0x87
} SeesawDevices;

typedef enum {
    SEESAW_GPIO_BASE = 0x01,
    SEESAW_SERCOM0_BASE = 0x02,

    SEESAW_TIMER_BASE = 0x08,
    SEESAW_ADC_BASE = 0x09,
    SEESAW_DAC_BASE = 0x0A,
    SEESAW_INTERRUPT_BASE = 0x0B,
    SEESAW_DAP_BASE = 0x0C,
    SEESAW_EEPROM_BASE = 0x0D,
    SEESAW_NEOPIXEL_BASE = 0x0E,
    SEESAW_TOUCH_BASE = 0x0F,
    SEESAW_KEYPAD_BASE = 0x10,
    SEESAW_ENCODER_BASE = 0x11,
    SEESAW_SPECTRUM_BASE = 0x12,
} SeesawModules;

typedef enum {
    SEESAW_GPIO_DIRSET_BULK = 0x02,
    SEESAW_GPIO_DIRCLR_BULK = 0x03,
    SEESAW_GPIO_BULK = 0x04,
    SEESAW_GPIO_BULK_SET = 0x05,
    SEESAW_GPIO_BULK_CLR = 0x06,
    SEESAW_GPIO_BULK_TOGGLE = 0x07,
    SEESAW_GPIO_INTENSET = 0x08,
    SEESAW_GPIO_INTENCLR = 0x09,
    SEESAW_GPIO_INTFLAG = 0x0A,
    SEESAW_GPIO_PULLENSET = 0x0B,
    SEESAW_GPIO_PULLENCLR = 0x0C,
} SeesawInputRegisters;

typedef enum {
    SEESAW_INPUT,
    SEESAW_OUTPUT,
    SEESAW_INPUT_PULLUP,
    SEESAW_INPUT_PULLDOWN
} SeesawPinMode;

typedef enum {
    SEESAW_ADC_STATUS = 0x00,
    SEESAW_ADC_INTEN = 0x02,
    SEESAW_ADC_INTENCLR = 0x03,
    SEESAW_ADC_WINMODE = 0x04,
    SEESAW_ADC_WINTHRESH = 0x05,
    SEESAW_ADC_CHANNEL_OFFSET = 0x07,
} SeesawADCFunctions;

enum {
    SEESAW_KEYPAD_STATUS = 0x00,
    SEESAW_KEYPAD_EVENT = 0x01,
    SEESAW_KEYPAD_INTENSET = 0x02,
    SEESAW_KEYPAD_INTENCLR = 0x03,
    SEESAW_KEYPAD_COUNT = 0x04,
    SEESAW_KEYPAD_FIFO = 0x10,
};

#define SEESAW_ALARM_NUM 0
#define SEESAW_ALARM_IRQ TIMER_IRQ_0

#define SEESAW_RETRIES 10

#define SEESAW_ADC_INPUT_0_PIN 2 ///< default ADC input pin
#define SEESAW_ADC_INPUT_1_PIN 3 ///< default ADC input pin
#define SEESAW_ADC_INPUT_2_PIN 4 ///< default ADC input pin
#define SEESAW_ADC_INPUT_3_PIN 5 ///< default ADC input pin

#define SEESAW_PWM_0_PIN 4 ///< default PWM output pin
#define SEESAW_PWM_1_PIN 5 ///< default PWM output pin
#define SEESAW_PWM_2_PIN 6 ///< default PWM output pin
#define SEESAW_PWM_3_PIN 7 ///< default PWM output pin

#define NEO_TRELLIS_NUM_ROWS 4
#define NEO_TRELLIS_NUM_COLS 4
#define NEO_TRELLIS_NUM_KEYS (NEO_TRELLIS_NUM_ROWS * NEO_TRELLIS_NUM_COLS)

static volatile bool SeesawDevice_alarmFired;

class SeesawDevice : public I2CDeviceBase {
    public:
        // Constructor
        SeesawDevice() {}
        SeesawDevice(PeripheralI2C *i2cController, uint8_t addr = 0x2E) {
            this->i2c = i2cController;
            this->address = addr;
        }

        std::vector<uint8_t> getDeviceAddresses() const override {
            return {0x2E};
        }

        //// Methods
        void begin();
        void reset();
        void start();
        void poll();

        int doI2CWrite(uint8_t *data, int len);
        int doI2CRead(uint8_t *data, int len);

        union keyEvent {
            struct {
                uint8_t EDGE : 2;  ///< the edge that was triggered
                uint16_t NUM : 14; ///< the event number
            } bit;               ///< bitfield format
            uint16_t reg;        ///< register format
        };

        union keyEventRaw {
            struct {
                uint8_t EDGE : 2; ///< the edge that was triggered
                uint8_t NUM : 6;  ///< the event number
            } bit;              ///< bitfield format
            uint8_t reg;        ///< register format
        };

        void waitUntil_us(uint64_t us);
        static void alarmIRQ();

        uint32_t getControllerID() { return readByte(SeesawCommands::SEESAW_STATUS_BASE, SeesawCommands::SEESAW_STATUS_HW_ID); };
        uint32_t getOptions() { return readLong(SeesawCommands::SEESAW_STATUS_BASE, SeesawCommands::SEESAW_STATUS_OPTIONS); };
        uint32_t getVersion() { return readLong(SeesawCommands::SEESAW_STATUS_BASE, SeesawCommands::SEESAW_STATUS_VERSION); };
        uint32_t getTemp() { return readLong(SeesawCommands::SEESAW_STATUS_BASE, SeesawCommands::SEESAW_STATUS_TEMP); };

        void pinModeBulk(uint32_t pins, uint8_t mode);
        void pinModeBulk(uint32_t pinsa, uint32_t pinsb, uint8_t mode);
        void pinMode(uint8_t pin, uint8_t mode);

        uint32_t digitalReadBulk(uint32_t pins);
        uint32_t digitalReadBulkB(uint32_t pins);
        bool digitalRead(uint8_t pin);
        uint16_t analogRead(uint8_t pin);

        void setI2C(PeripheralI2C *i2cController) { this->i2c = i2cController; }
        void setAddress(uint8_t addr) { this->address = addr; }
    private:
        bool isReady = false;
        uint32_t controllerID = 0;

        void read(uint8_t reg, uint8_t offset, uint8_t *data, uint8_t len);
        void write(uint8_t reg, uint8_t offset, uint8_t *data = NULL, int len = 0);

        uint8_t readByte(uint8_t regBase, uint8_t reg);
        void writeByte(uint8_t regBase, uint8_t reg, uint8_t value);

        uint16_t readShort(uint8_t regBase, uint8_t reg);
        void writeShort(uint8_t regBase, uint8_t reg, uint16_t value);

        uint32_t readLong(uint8_t regBase, uint8_t reg);
        void writeLong(uint8_t regBase, uint8_t reg, uint32_t value);
    protected:
        PeripheralI2C* i2c = nullptr;
        uint8_t address = 0;
};

#endif
