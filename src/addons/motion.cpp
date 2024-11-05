#include "addons/motion.h"
#include "storagemanager.h"
#include "helper.h"
#include "config.pb.h"

bool MotionAddon::available() {
    stdio_init_all();
    bool result = false;

    const DisplayOptions& displayOptions = Storage::getInstance().getDisplayOptions();
    const MotionOptions& options = Storage::getInstance().getAddonOptions().motionOptions;
    //if (options.enabled) {
        mpu = new MPU6886();
        lsm = new LSM6DSO();

        PeripheralI2CScanResult resultMPU = PeripheralManager::getInstance().scanForI2CDevice(mpu->getDeviceAddresses());
        PeripheralI2CScanResult resultLSM = PeripheralManager::getInstance().scanForI2CDevice(lsm->getDeviceAddresses());

        if (resultMPU.address > -1) {
            mpu->setAddress(resultMPU.address);
            mpu->setI2C(PeripheralManager::getInstance().getI2C(resultMPU.block));
            result = true;
        } else {
            delete mpu;
            mpu = nullptr;
        }

        if (resultLSM.address > -1) {
            lsm->setAddress(resultLSM.address);
            lsm->setI2C(PeripheralManager::getInstance().getI2C(resultLSM.block));
            result = true;
        } else {
            delete lsm;
            lsm = nullptr;
        }
    //}
    return result;
}

void MotionAddon::setup() {
    const MotionOptions& options = Storage::getInstance().getAddonOptions().motionOptions;

    if (mpu != nullptr) mpu->begin();
    if (lsm != nullptr) lsm->begin();
}

void MotionAddon::process()
{
    float ax;
    float ay;
    float az;
    float gx;
    float gy;
    float gz;

    Gamepad * gamepad = Storage::getInstance().GetGamepad();

    if (mpu != nullptr) {
        mpu->accelerometer(&ax, &ay, &az);
        mpu->gyroscope(&gx, &gy, &gz);
        printf("[MPU6886] %f,%f,%f,%f,%f,%f\n", ax, ay, az, gx, gy, gz);
    }

    if (lsm != nullptr) {
        lsm->accelerometer(&ax, &ay, &az);
        lsm->gyroscope(&gx, &gy, &gz);
        printf("[LSM6DSO] %f,%f,%f,%f,%f,%f\n", ax, ay, az, gx, gy, gz);
    }
}
