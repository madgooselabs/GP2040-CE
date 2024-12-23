#include "hardware/pwm.h"
#include "addons/seesaw.h"
#include "storagemanager.h"
#include "usbdriver.h"
#include "helper.h"
#include "config.pb.h"

bool SeesawAddon::available() {
    stdio_init_all();

    printf("Seesaw Available\n");

    const DisplayOptions& displayOptions = Storage::getInstance().getDisplayOptions();
    //const PCF8575Options& options = Storage::getInstance().getAddonOptions().pcf8575Options;
    //if (options.enabled) {
        seesaw = new SeesawDevice();
        PeripheralI2CScanResult result = PeripheralManager::getInstance().scanForI2CDevice(seesaw->getDeviceAddresses());
        if (result.address > -1) {
            seesaw->setAddress(result.address);
            seesaw->setI2C(PeripheralManager::getInstance().getI2C(result.block));
            return true;
        } else {
            delete seesaw;
        }
    //}
    return false;
}

void SeesawAddon::setup() {
    printf("Seesaw Setup\n");
    seesaw->begin();
    seesaw->start();
}

void SeesawAddon::process() {
    seesaw->poll();
}
