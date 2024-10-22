#ifndef I2C_GPIO_MPR121_H
#define I2C_GPIO_MPR121_H

#include "mpr121.h"

#include "gpaddon.h"

#include "GamepadEnums.h"
#include "peripheralmanager.h"

#include <map>

#ifndef I2C_MPR121_ENABLED
#define I2C_MPR121_ENABLED 0
#endif

#ifndef I2C_MPR121_BLOCK
#define I2C_MPR121_BLOCK i2c0
#endif

#define MPR121_PIN_COUNT 16

// IO pin defaults
#ifndef MPR121_PIN00_DIRECTION
#define MPR121_PIN00_DIRECTION GpioDirection::GPIO_DIRECTION_INPUT
#endif
#ifndef MPR121_PIN00_ACTION
#define MPR121_PIN00_ACTION GpioAction::NONE
#endif

#ifndef MPR121_PIN01_DIRECTION
#define MPR121_PIN01_DIRECTION GpioDirection::GPIO_DIRECTION_INPUT
#endif
#ifndef MPR121_PIN01_ACTION
#define MPR121_PIN01_ACTION GpioAction::NONE
#endif

#ifndef MPR121_PIN02_DIRECTION
#define MPR121_PIN02_DIRECTION GpioDirection::GPIO_DIRECTION_INPUT
#endif
#ifndef MPR121_PIN02_ACTION
#define MPR121_PIN02_ACTION GpioAction::NONE
#endif

#ifndef MPR121_PIN03_DIRECTION
#define MPR121_PIN03_DIRECTION GpioDirection::GPIO_DIRECTION_INPUT
#endif
#ifndef MPR121_PIN03_ACTION
#define MPR121_PIN03_ACTION GpioAction::NONE
#endif

#ifndef MPR121_PIN04_DIRECTION
#define MPR121_PIN04_DIRECTION GpioDirection::GPIO_DIRECTION_INPUT
#endif
#ifndef MPR121_PIN04_ACTION
#define MPR121_PIN04_ACTION GpioAction::NONE
#endif

#ifndef MPR121_PIN05_DIRECTION
#define MPR121_PIN05_DIRECTION GpioDirection::GPIO_DIRECTION_INPUT
#endif
#ifndef MPR121_PIN05_ACTION
#define MPR121_PIN05_ACTION GpioAction::NONE
#endif

#ifndef MPR121_PIN06_DIRECTION
#define MPR121_PIN06_DIRECTION GpioDirection::GPIO_DIRECTION_INPUT
#endif
#ifndef MPR121_PIN06_ACTION
#define MPR121_PIN06_ACTION GpioAction::NONE
#endif

#ifndef MPR121_PIN07_DIRECTION
#define MPR121_PIN07_DIRECTION GpioDirection::GPIO_DIRECTION_INPUT
#endif
#ifndef MPR121_PIN07_ACTION
#define MPR121_PIN07_ACTION GpioAction::NONE
#endif

#ifndef MPR121_PIN08_DIRECTION
#define MPR121_PIN08_DIRECTION GpioDirection::GPIO_DIRECTION_INPUT
#endif
#ifndef MPR121_PIN08_ACTION
#define MPR121_PIN08_ACTION GpioAction::NONE
#endif

#ifndef MPR121_PIN09_DIRECTION
#define MPR121_PIN09_DIRECTION GpioDirection::GPIO_DIRECTION_INPUT
#endif
#ifndef MPR121_PIN09_ACTION
#define MPR121_PIN09_ACTION GpioAction::NONE
#endif

#ifndef MPR121_PIN10_DIRECTION
#define MPR121_PIN10_DIRECTION GpioDirection::GPIO_DIRECTION_INPUT
#endif
#ifndef MPR121_PIN10_ACTION
#define MPR121_PIN10_ACTION GpioAction::NONE
#endif

#ifndef MPR121_PIN11_DIRECTION
#define MPR121_PIN11_DIRECTION GpioDirection::GPIO_DIRECTION_INPUT
#endif
#ifndef MPR121_PIN11_ACTION
#define MPR121_PIN11_ACTION GpioAction::NONE
#endif

#ifndef MPR121_PIN12_DIRECTION
#define MPR121_PIN12_DIRECTION GpioDirection::GPIO_DIRECTION_INPUT
#endif
#ifndef MPR121_PIN12_ACTION
#define MPR121_PIN12_ACTION GpioAction::NONE
#endif

// IO Module Name
#define MPR121AddonName "MPR121"

class MPR121Addon : public GPAddon {
public:
	virtual bool available();
	virtual void setup();
	virtual void preprocess() {}
	virtual void process();
    virtual std::string name() { return MPR121AddonName; }

    std::map<uint8_t, GpioMappingInfo> pinRef;
private:
    MPR121* mpr;

    bool inputButtonUp = false;
    bool inputButtonDown = false;
    bool inputButtonLeft = false;
    bool inputButtonRight = false;
    bool inputButtonB1 = false;
    bool inputButtonB2 = false;
    bool inputButtonB3 = false;
    bool inputButtonB4 = false;
    bool inputButtonL1 = false;
    bool inputButtonR1 = false;
    bool inputButtonL2 = false;
    bool inputButtonR2 = false;
    bool inputButtonS1 = false;
    bool inputButtonS2 = false;
    bool inputButtonL3 = false;
    bool inputButtonR3 = false;
    bool inputButtonA1 = false;
    bool inputButtonA2 = false;
    bool inputButtonFN = false;
};

#endif  // I2C_GPIO_MPR121_H
