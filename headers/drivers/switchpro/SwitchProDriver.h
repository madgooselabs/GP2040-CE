/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: Copyright (c) 2024 OpenStickCommunity (gp2040-ce.info)
 */

#ifndef _SWITCH_PRO_DRIVER_H_
#define _SWITCH_PRO_DRIVER_H_

#include "gpdriver.h"
#include "drivers/switchpro/SwitchProDescriptors.h"

class SwitchProDriver : public GPDriver {
public:
    virtual void initialize();
    virtual void process(Gamepad * gamepad, uint8_t * outBuffer);
    virtual void initializeAux() {}
    virtual void processAux() {}
    virtual uint16_t get_report(uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen);
    virtual void set_report(uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize);
    virtual bool vendor_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const *request);
    virtual const uint16_t * get_descriptor_string_cb(uint8_t index, uint16_t langid);
    virtual const uint8_t * get_descriptor_device_cb();
    virtual const uint8_t * get_hid_descriptor_report_cb(uint8_t itf) ;
    virtual const uint8_t * get_descriptor_configuration_cb(uint8_t index);
    virtual const uint8_t * get_descriptor_device_qualifier_cb();
    virtual uint16_t GetJoystickMidValue();
    virtual USBListener * get_usb_auth_listener() { return nullptr; }

    void set_controller_rumble(bool rumble);
private:
    uint8_t report[SWITCH_PRO_ENDPOINT_SIZE] = { };
    uint8_t last_report[SWITCH_PRO_ENDPOINT_SIZE] = { };
    SwitchProReport switchReport;
    uint8_t last_report_counter;
    uint32_t last_report_timer;
    bool isReady = false;

    uint8_t handshakeCounter = 0;

    const uint8_t colorProfiles[12] = {
        0x1B,0x1B,0x1D,
        0xEC,0x00,0x8C,
        0xEC,0x00,0x8C,
        0xEC,0x00,0x8C,
    };

    SwitchDeviceInfo deviceInfo;
    uint8_t playerID = 0;
    uint8_t inputMode = 0;
    bool isIMUEnabled = false;
    bool isVibrationEnabled = false;
};

#endif // _SWITCH_PRO_DRIVER_H_
