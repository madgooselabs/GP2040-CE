#include "addons/motion.h"
#include "storagemanager.h"
#include "helper.h"
#include "config.pb.h"

bool MotionAddon::available() {
    stdio_init_all();

    const DisplayOptions& displayOptions = Storage::getInstance().getDisplayOptions();
    const MotionOptions& options = Storage::getInstance().getAddonOptions().motionOptions;
    //if (options.enabled) {
        mpu = new MPU6886();
        PeripheralI2CScanResult result = PeripheralManager::getInstance().scanForI2CDevice(mpu->getDeviceAddresses());
        if (result.address > -1) {
            mpu->setAddress(result.address);
            mpu->setI2C(PeripheralManager::getInstance().getI2C(result.block));
            return true;
        } else {
            delete mpu;
        }
    //}
    //return false;
}

void MotionAddon::setup() {
    const MotionOptions& options = Storage::getInstance().getAddonOptions().motionOptions;

    mpu->begin();
}

void MotionAddon::process()
{
    float ax;
    float ay;
    float az;
    float gx;
    float gy;
    float gz;
    float t;

    Gamepad * gamepad = Storage::getInstance().GetGamepad();

    mpu->accelerometer(&ax, &ay, &az);
    mpu->gyroscope(&gx, &gy, &gz);
    t = mpu->temperature();

    printf("%f,%f,%f,%f,%f,%f,%f\n", ax, ay, az, gx, gy, gz, t);

    //sleep_us(100);
}
