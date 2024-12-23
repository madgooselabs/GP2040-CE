#include "seesaw_dev.h"

#include <cstring>
#include <cstdio>

void SeesawDevice::begin() {
    isReady = false;
    reset();
}

void SeesawDevice::reset(){
    isReady = false;

    for (int retryCtr = 0; retryCtr < SEESAW_RETRIES; ++retryCtr) {
        writeByte(SeesawCommands::SEESAW_STATUS_BASE, SeesawCommands::SEESAW_STATUS_SWRST, 0xFF);

        controllerID = getControllerID();

        if ((isReady == false) && ((controllerID == SeesawDevices::SAMD09_HW_ID_CODE) || (controllerID == SeesawDevices::ATTINY8X7_HW_ID_CODE))) {
            isReady = true;
        }

        if (isReady) break;
    }

    if (isReady) {
        //printf("Ready\n");
        //uint32_t options = getOptions();
        //printf("Controller ID: %x\n", controllerID);
        //printf("Options: %x\n", options);
        //
        //printf("Capabilities:\n");
        //printf("Timer: %1d\n",    (options & (1UL << SeesawModules::SEESAW_TIMER_BASE)) > 0);
        //printf("ADC: %1d\n",      (options & (1UL << SeesawModules::SEESAW_ADC_BASE)) > 0);
        //printf("DAC: %1d\n",      (options & (1UL << SeesawModules::SEESAW_DAC_BASE)) > 0);
        //printf("IRQ: %1d\n",      (options & (1UL << SeesawModules::SEESAW_INTERRUPT_BASE)) > 0);
        //printf("DAP: %1d\n",      (options & (1UL << SeesawModules::SEESAW_DAP_BASE)) > 0);
        //printf("EEPROM: %1d\n",   (options & (1UL << SeesawModules::SEESAW_EEPROM_BASE)) > 0);
        //printf("NeoPixel: %1d\n", (options & (1UL << SeesawModules::SEESAW_NEOPIXEL_BASE)) > 0);
        //printf("Touch: %1d\n",    (options & (1UL << SeesawModules::SEESAW_TOUCH_BASE)) > 0);
        //printf("Keypad: %1d\n",   (options & (1UL << SeesawModules::SEESAW_KEYPAD_BASE)) > 0);
        //printf("Encoder: %1d\n",  (options & (1UL << SeesawModules::SEESAW_ENCODER_BASE)) > 0);
        //printf("Spectrum: %1d\n", (options & (1UL << SeesawModules::SEESAW_SPECTRUM_BASE)) > 0);
    }
}

void SeesawDevice::start() {

}

void SeesawDevice::poll() {
    uint8_t keypadCount = this->readByte(SEESAW_KEYPAD_BASE, SEESAW_KEYPAD_COUNT);
    waitUntil_us(500);

    if (keypadCount > 0) {
        //keypadCount += 2;
        keyEventRaw buf[keypadCount];
        this->read(SEESAW_KEYPAD_BASE, SEESAW_KEYPAD_FIFO, (uint8_t *)buf, keypadCount);
        waitUntil_us(1000);
        for (uint16_t i = 0; i < keypadCount; i++) {
            if (buf[i].bit.NUM < NEO_TRELLIS_NUM_KEYS) {
                keyEvent evt = {buf[i].bit.EDGE, buf[i].bit.NUM};
                printf("Key Num: %d\n", evt.bit.NUM);
            }
        }
    }
//    keyEventRaw keys;
//    if (this->read(SEESAW_KEYPAD_BASE, SEESAW_KEYPAD_FIFO, (uint8_t *)keys, 1, 1000)) {
//
//    }
}

void SeesawDevice::waitUntil_us(uint64_t us) {
    SeesawDevice_alarmFired = false;

    // Enable the interrupt for our alarm (the timer outputs 4 alarm irqs)
    hw_set_bits(&timer_hw->inte, 1u << SEESAW_ALARM_NUM);
    // Set irq handler for alarm irq
    irq_set_exclusive_handler(SEESAW_ALARM_IRQ, alarmIRQ);
    // Enable the alarm irq
    irq_set_enabled(SEESAW_ALARM_IRQ, true);
    // Enable interrupt in block and at processor

    // Alarm is only 32 bits so if trying to delay more
    // than that need to be careful and keep track of the upper
    // bits
    uint64_t target = timer_hw->timerawl + us;

    // Write the lower 32 bits of the target time to the alarm which
    // will arm it
    timer_hw->alarm[SEESAW_ALARM_NUM] = (uint32_t) target;

    while (!SeesawDevice_alarmFired);
}

void SeesawDevice::alarmIRQ() {
    // Clear the alarm irq
    hw_clear_bits(&timer_hw->intr, 1u << SEESAW_ALARM_NUM);

    // Assume alarm 0 has fired
    SeesawDevice_alarmFired = true;
}

int SeesawDevice::doI2CWrite(uint8_t *data, int len) {
    int result = i2c->write(address, data, len);
    waitUntil_us(SEESAW_DELAY);
    return result;
}

int SeesawDevice::doI2CRead(uint8_t *data, int len) {
    int result = i2c->read(address, data, len);
    waitUntil_us(SEESAW_DELAY);
    return result;
}

void SeesawDevice::read(uint8_t reg, uint8_t offset, uint8_t *data, uint8_t len) {
    int result = 0;
    write(reg, offset);

    result = doI2CRead(data, len);

#ifdef SEESAW_I2C_DEBUG
    printf("SeesawDevice:read ");
    for (int i = 0; i < len; ++i) {
        printf("%x ", data[i]);
    }

    printf(" result=%d\n", result);
#endif
}

void SeesawDevice::write(uint8_t reg, uint8_t offset, uint8_t *data, int len) {
    int result = 0;
    uint8_t dataLen = len+2;
    uint8_t dataToWrite[dataLen];
    dataToWrite[0] = reg;
    dataToWrite[1] = offset;
    if (len > 0) memcpy(&dataToWrite[2], data, len);

#ifdef SEESAW_I2C_DEBUG
    printf("SeesawDevice:write ");
    for (int i = 0; i < dataLen; ++i) {
        printf("%x ", dataToWrite[i]);
    }
#endif

    result = doI2CWrite(dataToWrite, dataLen);

#ifdef SEESAW_I2C_DEBUG
    printf(" result=%d\n", result);
#endif
}

uint8_t SeesawDevice::readByte(uint8_t regBase, uint8_t reg) {
    uint8_t dataToRead[1];
    read(regBase, reg, dataToRead, 1);
    return dataToRead[0];
}

void SeesawDevice::writeByte(uint8_t regBase, uint8_t reg, uint8_t value) {
    uint8_t dataToWrite[1] = {value};
    write(regBase, reg, dataToWrite, 1);
}

uint16_t SeesawDevice::readShort(uint8_t regBase, uint8_t reg) {
    uint8_t dataToRead[2];
    read(regBase, reg, dataToRead, 2);
    return ((uint16_t)dataToRead[0]) | ((uint16_t)dataToRead[1] << 8);
}

void SeesawDevice::writeShort(uint8_t regBase, uint8_t reg, uint16_t value) {
    uint8_t dataToWrite[2] = {(uint8_t)(value),(uint8_t)((value>>8))};
    write(regBase, reg, dataToWrite, 2);
}

uint32_t SeesawDevice::readLong(uint8_t regBase, uint8_t reg) {
    uint8_t dataToRead[4];
    read(regBase, reg, dataToRead, 4);
    return ((uint32_t)dataToRead[0]) | ((uint32_t)dataToRead[1] << 8) | ((uint32_t)dataToRead[2] << 16) | ((uint32_t)dataToRead[3] << 24);
}

void SeesawDevice::pinMode(uint8_t pin, uint8_t mode) {
    if (pin >= 32) {
        pinModeBulk(0, 1ul << (pin - 32), mode);
    } else {
        pinModeBulk(1ul << pin, mode);
    }
}

void SeesawDevice::pinModeBulk(uint32_t pins, uint8_t mode) {
    uint8_t pinCommand[] = {(uint8_t)(pins >> 24), (uint8_t)(pins >> 16),(uint8_t)(pins >> 8), (uint8_t)pins};

    switch (mode) {
        case SeesawPinMode::SEESAW_INPUT:
            write(SeesawModules::SEESAW_GPIO_BASE, SeesawInputRegisters::SEESAW_GPIO_DIRCLR_BULK, pinCommand, 4);
            write(SeesawModules::SEESAW_GPIO_BASE, SeesawInputRegisters::SEESAW_GPIO_PULLENCLR, pinCommand, 4);
            break;
        case SeesawPinMode::SEESAW_OUTPUT:
            write(SeesawModules::SEESAW_GPIO_BASE, SeesawInputRegisters::SEESAW_GPIO_DIRSET_BULK, pinCommand, 4);
            break;
        case SeesawPinMode::SEESAW_INPUT_PULLUP:
            write(SeesawModules::SEESAW_GPIO_BASE, SeesawInputRegisters::SEESAW_GPIO_DIRCLR_BULK, pinCommand, 4);
            write(SeesawModules::SEESAW_GPIO_BASE, SeesawInputRegisters::SEESAW_GPIO_PULLENSET, pinCommand, 4);
            write(SeesawModules::SEESAW_GPIO_BASE, SeesawInputRegisters::SEESAW_GPIO_BULK_SET, pinCommand, 4);
            break;
        case SeesawPinMode::SEESAW_INPUT_PULLDOWN:
            write(SeesawModules::SEESAW_GPIO_BASE, SeesawInputRegisters::SEESAW_GPIO_DIRCLR_BULK, pinCommand, 4);
            write(SeesawModules::SEESAW_GPIO_BASE, SeesawInputRegisters::SEESAW_GPIO_PULLENSET, pinCommand, 4);
            write(SeesawModules::SEESAW_GPIO_BASE, SeesawInputRegisters::SEESAW_GPIO_BULK_CLR, pinCommand, 4);
            break;
    }
}

void SeesawDevice::pinModeBulk(uint32_t pinsa, uint32_t pinsb, uint8_t mode) {
    uint8_t pinCommand[] = {(uint8_t)(pinsa >> 24), (uint8_t)(pinsa >> 16), (uint8_t)(pinsa >> 8), (uint8_t)pinsa, (uint8_t)(pinsb >> 24), (uint8_t)(pinsb >> 16), (uint8_t)(pinsb >> 8), (uint8_t)pinsb};
    switch (mode) {
        case SeesawPinMode::SEESAW_INPUT:
            this->write(SeesawModules::SEESAW_GPIO_BASE, SeesawInputRegisters::SEESAW_GPIO_DIRCLR_BULK, pinCommand, 8);
            break;
        case SeesawPinMode::SEESAW_OUTPUT:
            this->write(SeesawModules::SEESAW_GPIO_BASE, SeesawInputRegisters::SEESAW_GPIO_DIRSET_BULK, pinCommand, 8);
            break;
        case SeesawPinMode::SEESAW_INPUT_PULLUP:
            this->write(SeesawModules::SEESAW_GPIO_BASE, SeesawInputRegisters::SEESAW_GPIO_DIRCLR_BULK, pinCommand, 8);
            this->write(SeesawModules::SEESAW_GPIO_BASE, SeesawInputRegisters::SEESAW_GPIO_PULLENSET, pinCommand, 8);
            this->write(SeesawModules::SEESAW_GPIO_BASE, SeesawInputRegisters::SEESAW_GPIO_BULK_SET, pinCommand, 8);
            break;
        case SeesawPinMode::SEESAW_INPUT_PULLDOWN:
            this->write(SeesawModules::SEESAW_GPIO_BASE, SeesawInputRegisters::SEESAW_GPIO_DIRCLR_BULK, pinCommand, 8);
            this->write(SeesawModules::SEESAW_GPIO_BASE, SeesawInputRegisters::SEESAW_GPIO_PULLENSET, pinCommand, 8);
            this->write(SeesawModules::SEESAW_GPIO_BASE, SeesawInputRegisters::SEESAW_GPIO_BULK_CLR, pinCommand, 8);
            break;
    }
}

uint32_t SeesawDevice::digitalReadBulk(uint32_t pins) {
    uint8_t pinData[4];
    read(SeesawModules::SEESAW_GPIO_BASE, SEESAW_GPIO_BULK, pinData, 4);
    uint32_t result = ((uint32_t)pinData[0] << 24) | ((uint32_t)pinData[1] << 16) | ((uint32_t)pinData[2] << 8) | (uint32_t)pinData[3];
    return result & pins;
}

uint32_t SeesawDevice::digitalReadBulkB(uint32_t pins) {
    uint8_t pinData[8];
    read(SeesawModules::SEESAW_GPIO_BASE, SEESAW_GPIO_BULK, pinData, 8);
    uint32_t result = ((uint32_t)pinData[4] << 24) | ((uint32_t)pinData[5] << 16) | ((uint32_t)pinData[6] << 8) | (uint32_t)pinData[7];
    return result & pins;
}

bool SeesawDevice::digitalRead(uint8_t pin) {
    if (pin >= 32) {
        return digitalReadBulkB((1ul << (pin - 32))) != 0;
    } else {
        return digitalReadBulk((1ul << pin)) != 0;
    }
}

uint16_t SeesawDevice::analogRead(uint8_t pin) {
    uint8_t pinData[2];
    uint8_t p = 0;

    if (controllerID == SeesawDevices::SAMD09_HW_ID_CODE) {
        switch (pin) {
            case SEESAW_ADC_INPUT_0_PIN:
                p = 0;
                break;
            case SEESAW_ADC_INPUT_1_PIN:
                p = 1;
                break;
            case SEESAW_ADC_INPUT_2_PIN:
                p = 2;
                break;
            case SEESAW_ADC_INPUT_3_PIN:
                p = 3;
                break;
            default:
                return 0;
        }
    } else if (controllerID == SeesawDevices::ATTINY8X7_HW_ID_CODE) {
        p = pin;
    } else {
        return 0;
    }

    read(SeesawModules::SEESAW_ADC_BASE, SeesawADCFunctions::SEESAW_ADC_CHANNEL_OFFSET + p, pinData, 2);
    uint16_t result = ((uint16_t)pinData[0] << 8) | pinData[1];
    return result;
}