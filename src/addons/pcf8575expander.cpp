#include "addons/pcf8575expander.h"
#include "storagemanager.h"
#include "helper.h"
#include "config.pb.h"

bool PCF8575Expander::available() {
    //const AnalogADS1219Options& options = Storage::getInstance().getAddonOptions().analogADS1219Options;
    //return (options.enabled && PeripheralManager::getInstance().isI2CEnabled(options.i2cBlock));
    return PeripheralManager::getInstance().isI2CEnabled(1);
}

void PCF8575Expander::setup() {
    //const AnalogADS1219Options& options = Storage::getInstance().getAddonOptions().analogADS1219Options;
    PeripheralI2C* i2c = PeripheralManager::getInstance().getI2C(1);

    stdio_init_all();

    pcf = new PCF8575(i2c);
    pcf->begin();

//    memset(&pins, 0, sizeof(ADS_PINS));
//    channelHop = 0;
//
//    uIntervalMS = 1;
//    nextTimer = getMillis();
//
//    // Init our ADS1219 library
//    ads = new ADS1219(i2c, options.i2cAddress);
//    ads->begin();                               // setup I2C and chip start
//    ads->setChannel(0);                         // Start on Channel 0
//    ads->setConversionMode(CONTINUOUS);         // Read analog continuously
//    ads->setGain(ONE);                          // Set gain to 1
//    ads->setDataRate(1000);                     // 1mhz (1.1ms delay)
//    ads->setVoltageReference(REF_INTERNAL);     // Use internal VREF for now
//    ads->start();                               // START/SYNC command
}

void PCF8575Expander::process()
{
    pcf->setPin(1, isSet);
    printf("%01x\n", pcf->getPin(1));
    isSet = !isSet;
//    if (nextTimer < getMillis()) {
//        float result;
//        uint32_t readValue;
//        if ( ads->readRegister(STATUS) & REGISTER_STATUS_DRDY ) {
//            readValue = ads->readConversionResult();
//            result = readValue / float(ADS_MAX); // gives us 0.0f to 1.0f (actual voltage is times voltage)
//            pins.A[channelHop] = result;
//            channelHop = (channelHop+1) % 4; // Loop 0-3
//            ads->setChannel(channelHop);
//            nextTimer = getMillis() + uIntervalMS; // interval for read (we can't be too fast)
//        }
//    }
//
//    Gamepad * gamepad = Storage::getInstance().GetGamepad();
//    gamepad->state.lx = (uint16_t)(65535.f*pins.A[0]);
//    gamepad->state.ly = (uint16_t)(65535.f*pins.A[1]);
//    gamepad->state.rx = (uint16_t)(65535.f*pins.A[2]);
//    gamepad->state.ry = (uint16_t)(65535.f*pins.A[3]);

}
