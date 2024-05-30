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
    uint8_t last_report[CFG_TUD_ENDPOINT0_SIZE] = { };
    SwitchProReport switchReport;
    uint8_t _report[100] = {0x0};
    uint8_t _switchRequestReport[100] = {0x0};
    uint8_t _addr[6] = {0x0};
    bool _vibration_enabled = false;
    uint8_t _vibration_report = 0x00;
    uint8_t _vibration_idx = 0x00;
    bool _imu_enabled = false;
    uint8_t _player_number = 0x00;
    bool _device_info_queried = false;
    uint32_t _timer = 0;
    uint32_t _timestamp = 0;

    uint8_t* generate_usb_report();
    void set_empty_report();
    void set_empty_switch_request_report();
    uint8_t* generate_report();
    void set_subcommand_reply();
    void set_standard_input_report();
    void set_timer();
    void set_full_input_report();
    void set_bt();
    void set_device_info();
    void set_shipment();
    void toggle_imu();
    void imu_sensitivity();
    void set_imu_data();
    void spi_read();
    void set_mode();
    void set_trigger_buttons();
    void enable_vibration();
    void set_player_lights();
    void set_nfc_ir_state();
    void set_nfc_ir_config();
};

#endif // _SWITCH_PRO_DRIVER_H_
