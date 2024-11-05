#ifndef MOTIONADDON_H
#define MOTIONADDON_H

#include "lsm6dso.h"
#include "mpu6886.h"

#include "gpaddon.h"

#include "GamepadEnums.h"
#include "peripheralmanager.h"

#include <map>

#ifndef MOTION_ENABLED
#define MOTION_ENABLED 0
#endif

// IO Module Name
#define MotionAddonName "Motion"

class MotionAddon : public GPAddon {
public:
	virtual bool available();
	virtual void setup();
	virtual void preprocess() {}
	virtual void process();
    virtual std::string name() { return MotionAddonName; }
private:
    MPU6886* mpu = nullptr;
    LSM6DSO* lsm = nullptr;
};

#endif  // _I2CAnalog_H_
