#ifndef SEESAW_H_
#define SEESAW_H_

#include "seesaw_dev.h"

#include "gpaddon.h"

#include "GamepadEnums.h"
#include "peripheralmanager.h"

#ifndef SEESAW_ENABLED
#define SEESAW_ENABLED 0
#endif

// Seesaw Module
#define SeesawAddonName "Seesaw"

// Reactive LED
class SeesawAddon : public GPAddon
{
    public:
        virtual bool available();
        virtual void setup();
        virtual void preprocess() {}
        virtual void process();
        virtual std::string name() { return SeesawAddonName; }
    private:
        SeesawDevice* seesaw;
};

#endif