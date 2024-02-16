#ifndef _PCF8575EXPANDER_H
#define _PCF8575EXPANDER_H

#include <PCF8575.h>

#include "gpaddon.h"

#include "GamepadEnums.h"
#include "peripheralmanager.h"

//#ifndef I2C_ANALOG1219_ENABLED
//#define I2C_ANALOG1219_ENABLED 0
//#endif
//
//#ifndef I2C_ANALOG1219_SDA_PIN
//#define I2C_ANALOG1219_SDA_PIN    -1
//#endif
//
//#ifndef I2C_ANALOG1219_SCL_PIN
//#define I2C_ANALOG1219_SCL_PIN    -1
//#endif
//
//#ifndef I2C_ANALOG1219_BLOCK
//#define I2C_ANALOG1219_BLOCK i2c0
//#endif
//
//#ifndef I2C_ANALOG1219_SPEED
//#define I2C_ANALOG1219_SPEED 400000
//#endif
//
//#ifndef I2C_ANALOG1219_ADDRESS
//#define I2C_ANALOG1219_ADDRESS 0x40
//#endif

// Analog Module Name
#define PCF8575ExpanderName "PCF8575"

class PCF8575Expander : public GPAddon {
public:
	virtual bool available();
	virtual void setup();
	virtual void preprocess() {}
	virtual void process();
    virtual std::string name() { return PCF8575ExpanderName; }
private:
    PCF8575* pcf;
	bool isSet = false;
};

#endif  // _I2CAnalog_H_
