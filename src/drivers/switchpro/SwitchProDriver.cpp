#include "drivers/switchpro/SwitchProDriver.h"
#include "drivers/shared/driverhelper.h"
#include "pico/rand.h"

// force a report to be sent every X ms
#define SWITCH_PRO_KEEPALIVE_TIMER 5

void SwitchProDriver::initialize() {
    //stdio_init_all();

    playerID = 0;
    last_report_counter = 0;
    handshakeCounter = 0;
    isReady = false;

    deviceInfo = {
        .majorVersion = 0x04,
        .minorVersion = 0x20,
        .controllerType = SwitchControllerType::SWITCH_TYPE_PRO_CONTROLLER,
        .unknown00 = 0x02,
        // MAC address in reverse
        .macAddress = {0x7c, 0xbb, 0x8a, (uint8_t)(get_rand_32() % 0xff), (uint8_t)(get_rand_32() % 0xff), (uint8_t)(get_rand_32() % 0xff)},
        .unknown01 = 0x01,
        .storedColors = 0x01,
    };

	switchReport = {
        .reportID = 0x30,
        .timestamp = 0,
        .connection_info = 1,
        .battery_level = 9,

        // byte 00
        .button_y = 0,
        .button_x = 0,
        .button_b = 0,
        .button_a = 0,
        .button_right_sl = 0,
        .button_right_sr = 0,
        .button_r = 0,
        .button_zr = 0,

        // byte 01
        .button_minus = 0,
        .button_plus = 0,
        .button_thumb_r = 0,
        .button_thumb_l = 0,
        .button_home = 0,
        .button_capture = 0,
        .dummy = 0,
        .charging_grip = 0,

        // byte 02
        .dpad_down = 0,
        .dpad_up = 0,
        .dpad_right = 0,
        .dpad_left = 0,
        .button_left_sl = 0,
        .button_left_sr = 0,
        .button_l = 0,
        .button_zl = 0,

        .analog = {0x00},
        .vibrator_input_report = 0,
        .imu_data = {0x00},
        .padding = {0x00}
    };

	class_driver = {
	#if CFG_TUSB_DEBUG >= 2
		.name = "SWITCH",
	#endif
		.init = hidd_init,
		.reset = hidd_reset,
		.open = hidd_open,
		.control_xfer_cb = hidd_control_xfer_cb,
		.xfer_cb = hidd_xfer_cb,
		.sof = NULL
	};
}

void SwitchProDriver::process(Gamepad * gamepad, uint8_t * outBuffer) {
    switchReport.dpad_up =    ((gamepad->state.dpad & GAMEPAD_MASK_DPAD) == GAMEPAD_MASK_UP);
    switchReport.dpad_down =  ((gamepad->state.dpad & GAMEPAD_MASK_DPAD) == GAMEPAD_MASK_DOWN);
    switchReport.dpad_left =  ((gamepad->state.dpad & GAMEPAD_MASK_DPAD) == GAMEPAD_MASK_LEFT);
    switchReport.dpad_right = ((gamepad->state.dpad & GAMEPAD_MASK_DPAD) == GAMEPAD_MASK_RIGHT);

    switchReport.charging_grip = 1;

    switchReport.button_y = gamepad->pressedB3();
    switchReport.button_x = gamepad->pressedB4();
    switchReport.button_b = gamepad->pressedB1();
    switchReport.button_a = gamepad->pressedB2();
    switchReport.button_right_sr = 0;
    switchReport.button_right_sl = 0;
    switchReport.button_r = gamepad->pressedR1();
    switchReport.button_zr = gamepad->pressedR2();
    switchReport.button_minus = gamepad->pressedS1();
    switchReport.button_plus = gamepad->pressedS2();
    switchReport.button_thumb_r = gamepad->pressedR3();
    switchReport.button_thumb_l = gamepad->pressedL3();
    switchReport.button_home = gamepad->pressedA1();
    switchReport.button_capture = gamepad->pressedA2();
    switchReport.button_left_sr = 0;
    switchReport.button_left_sl = 0;
    switchReport.button_l = gamepad->pressedL1();
    switchReport.button_zl = gamepad->pressedL2();

    // analog
    switchReport.analog[0] = (gamepad->state.lx & 0xFF);
    switchReport.analog[1] = ((gamepad->state.lx >> 8) & 0x0F) | ((gamepad->state.ly & 0x0F) << 4);
    switchReport.analog[2] = (gamepad->state.ly >> 4) & 0xFF;

    switchReport.analog[3] = (gamepad->state.rx & 0xFF);
    switchReport.analog[4] = ((gamepad->state.rx >> 8) & 0x0F) | ((gamepad->state.ry & 0x0F) << 4);
    switchReport.analog[5] = (gamepad->state.ry >> 4) & 0xFF;

    switchReport.vibrator_input_report = 0x09;

	// Wake up TinyUSB device
	if (tud_suspended())
		tud_remote_wakeup();

    uint16_t report_size = SWITCH_PRO_ENDPOINT_SIZE;
    uint32_t now = to_ms_since_boot(get_absolute_time());
    memcpy(report, &switchReport, sizeof(switchReport));
    if (isReady) {
        if (memcmp(last_report, report, report_size) != 0) {
            // HID ready + report sent, copy previous report
            if (tud_hid_ready() && tud_hid_report(0, report, report_size) == true ) {
                memcpy(last_report, report, report_size);
            }
            last_report_timer = now;
        } else {
            if ((now - last_report_timer) > SWITCH_PRO_KEEPALIVE_TIMER) {
                last_report_counter = (last_report_counter+1) & 0xFF;
                switchReport.timestamp = last_report_counter;
                // the *next* process() will be a forced report (or real user input)
            }
        }
    }
}

// tud_hid_get_report_cb
uint16_t SwitchProDriver::get_report(uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen) {
    memcpy(buffer, &switchReport, sizeof(SwitchProReport));
	return sizeof(SwitchProReport);
}

void SwitchProDriver::set_report(uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize) {
    uint8_t switchReportID = buffer[0];
    uint8_t switchReportSubID = buffer[1];
    bool canSend = false;
    switch (report_type) {
        case HID_REPORT_TYPE_OUTPUT:
            if (switchReportID == SwitchReportID::REPORT_USB_OUTPUT_80) {
                memset(report, 0x00, bufsize);

                if (switchReportSubID == SwitchOutputSubtypes::IDENTIFY) {
                    //printf("SwitchProDriver::set_report: IDENTIFY\n");
                    report[0] = SwitchReportID::REPORT_USB_INPUT_81;
                    report[1] = SwitchOutputSubtypes::IDENTIFY;
                    report[2] = 0x00;
                    //report[3] = deviceInfo.controllerType;
                    report[3] = 0x02;
                    // MAC address
                    for (uint8_t i = 0; i < 6; i++) {
                        report[4+i] = deviceInfo.macAddress[5-i];
                    }
                    canSend = true;
                } else if (switchReportSubID == SwitchOutputSubtypes::HANDSHAKE) {
                    //printf("SwitchProDriver::set_report: HANDSHAKE\n");
                    report[0] = SwitchReportID::REPORT_USB_INPUT_81;
                    report[1] = SwitchOutputSubtypes::HANDSHAKE;
                    canSend = true;
                } else if (switchReportSubID == SwitchOutputSubtypes::BAUD_RATE) {
                    ////printf("SwitchProDriver::set_report: BAUD_RATE\n");
                    report[0] = SwitchReportID::REPORT_USB_INPUT_81;
                    report[1] = SwitchOutputSubtypes::BAUD_RATE;
                    canSend = true;
                } else if (switchReportSubID == SwitchOutputSubtypes::DISABLE_USB_TIMEOUT) {
                    ////printf("SwitchProDriver::set_report: DISABLE_USB_TIMEOUT\n");
                    report[0] = SwitchReportID::REPORT_USB_INPUT_81;
                    report[1] = SwitchOutputSubtypes::DISABLE_USB_TIMEOUT;
                    //if (handshakeCounter < 2) {
                    //    handshakeCounter++;
                    //} else {
                    //    isReady = true;
                    //}
                    canSend = true;
                } else if (switchReportSubID == SwitchOutputSubtypes::ENABLE_USB_TIMEOUT) {
                    ////printf("SwitchProDriver::set_report: ENABLE_USB_TIMEOUT\n");
                    report[0] = SwitchReportID::REPORT_USB_INPUT_81;
                    report[1] = SwitchOutputSubtypes::ENABLE_USB_TIMEOUT;
                    canSend = true;
                } else {
                    //printf("SwitchProDriver::set_report: Unknown Sub ID %02x\n", switchReportSubID);
                }
            } else if (switchReportID == SwitchReportID::REPORT_OUTPUT_01) {
                uint8_t commandID = buffer[10];
                uint32_t spiReadAddress = 0;
                uint8_t spiReadSize = 0;

                memset(report, 0x00, bufsize);

                report[0] = SwitchReportID::REPORT_OUTPUT_21;
                report[1] = switchReport.timestamp;

                switch (commandID) {
                    case SwitchCommands::GET_CONTROLLER_STATE:
                        //printf("SwitchProDriver::set_report: Rpt 0x01 GET_CONTROLLER_STATE\n");
                        report[13] = 0x80;
                        report[14] = commandID;
                        report[15] = 0x03;
                        canSend = true;
                        break;
                    case SwitchCommands::BLUETOOTH_PAIR_REQUEST:
                        //printf("SwitchProDriver::set_report: Rpt 0x01 BLUETOOTH_PAIR_REQUEST\n");
                        report[13] = 0x80;
                        report[14] = commandID;
                        report[15] = 0x03;
                        canSend = true;
                        break;
                    case SwitchCommands::REQUEST_DEVICE_INFO:
                        //printf("SwitchProDriver::set_report: Rpt 0x01 REQUEST_DEVICE_INFO\n");
                        report[13] = 0x82;
                        report[14] = 0x02;
                        memcpy(&report[15], &deviceInfo, sizeof(deviceInfo));
                        canSend = true;
                        break;
                    case SwitchCommands::SET_MODE:
                        //printf("SwitchProDriver::set_report: Rpt 0x01 SET_MODE\n");
                        inputMode = buffer[11];
                        report[13] = 0x80;
                        report[14] = 0x03;
                        canSend = true;
                        //printf("Input Mode set to ");
                        switch (inputMode) {
                            case 0x00:
                                //printf("NFC/IR Polling Data");
                                break;
                            case 0x01:
                                //printf("NFC/IR Polling Config");
                                break;
                            case 0x02:
                                //printf("NFC/IR Polling Data+Config");
                                break;
                            case 0x03:
                                //printf("IR Scan");
                                break;
                            case 0x23:
                                //printf("MCU Update");
                                break;
                            case 0x30:
                                //printf("Full Input");
                                break;
                            case 0x31:
                                //printf("NFC/IR");
                                break;
                            case 0x33:
                            case 0x35:
                                //printf("Unknown");
                                break;
                            case 0x3F:
                                //printf("Simple HID");
                                break;
                        }
                        //printf("\n----------------------------------------------\n");
                        break;
                    case SwitchCommands::TRIGGER_BUTTONS:
                        //printf("SwitchProDriver::set_report: Rpt 0x01 TRIGGER_BUTTONS\n");
                        report[13] = 0x83;
                        report[14] = 0x04;
                        canSend = true;
                        break;
                    case SwitchCommands::SET_SHIPMENT:
                        //printf("SwitchProDriver::set_report: Rpt 0x01 SET_SHIPMENT\n");
                        report[13] = 0x80;
                        report[14] = commandID;
                        report[15] = 0x03;
                        canSend = true;
                        break;
                    case SwitchCommands::SPI_READ:
                        //printf("SwitchProDriver::set_report: Rpt 0x01 SPI_READ\n");
                        spiReadAddress = (buffer[14] << 24) | (buffer[13] << 16) | (buffer[12] << 8) | (buffer[11]);
                        spiReadSize = buffer[15];
                        ////printf("Read From: 0x%08x Size %d\n", spiReadAddress, spiReadSize);
                        report[13] = 0x90;
                        report[14] = buffer[10];
                        report[15] = buffer[11];
                        report[16] = buffer[12];
                        report[17] = buffer[13];
                        report[18] = buffer[14];
                        if (spiReadAddress == 0x6050) {
                            memcpy(&report[20], &colorProfiles, spiReadSize);
                            //printf("Body Color: #%02x%02x%02x\n", report[22], report[21], report[20]);
                            //printf("Button Color: #%02x%02x%02x\n", report[25], report[24], report[23]);
                            //printf("L Grip Color: #%02x%02x%02x\n", report[28], report[27], report[26]);
                            //printf("R Grip Color: #%02x%02x%02x\n", report[31], report[30], report[29]);
                        }
                        canSend = true;
                        ////printf("----------------------------------------------\n");
                        break;
                    case SwitchCommands::SET_NFC_IR_CONFIG:
                        //printf("SwitchProDriver::set_report: Rpt 0x01 SET_NFC_IR_CONFIG\n");
                        report[13] = 0x80;
                        report[14] = commandID;
                        canSend = true;
                        break;
                    case SwitchCommands::SET_NFC_IR_STATE:
                        //printf("SwitchProDriver::set_report: Rpt 0x01 SET_NFC_IR_STATE\n");
                        report[13] = 0x80;
                        report[14] = commandID;
                        canSend = true;
                        break;
                    case SwitchCommands::SET_PLAYER_LIGHTS:
                        //printf("SwitchProDriver::set_report: Rpt 0x01 SET_PLAYER_LIGHTS\n");
                        playerID = buffer[11];
                        report[13] = 0x80;
                        report[14] = commandID;
                        canSend = true;
                        //printf("Player set to %d\n", playerID);
                        //printf("----------------------------------------------\n");
                        break;
                    case SwitchCommands::GET_PLAYER_LIGHTS:
                        //printf("SwitchProDriver::set_report: Rpt 0x01 GET_PLAYER_LIGHTS\n");
                        playerID = buffer[11];
                        report[13] = 0xB0;
                        report[14] = commandID;
                        report[15] = playerID;
                        canSend = true;
                        //printf("Player is %d\n", playerID);
                        //printf("----------------------------------------------\n");
                        break;
                    case SwitchCommands::COMMAND_UNKNOWN_33:
                        //printf("SwitchProDriver::set_report: Rpt 0x01 COMMAND_UNKNOWN_33\n");
                        // Command typically thrown by Chromium to detect if a Switch controller exists. Can ignore.
                        report[13] = 0x80;
                        report[14] = commandID;
                        report[15] = 0x03;
                        canSend = true;
                        break;
                    case SwitchCommands::SET_HOME_LIGHT:
                        //printf("SwitchProDriver::set_report: Rpt 0x01 SET_HOME_LIGHT\n");
                        // NYI
                        report[13] = 0x80;
                        report[14] = commandID;
                        report[15] = 0x00;
                        canSend = true;
                        break;
                    case SwitchCommands::TOGGLE_IMU:
                        //printf("SwitchProDriver::set_report: Rpt 0x01 TOGGLE_IMU\n");
                        isIMUEnabled = buffer[11];
                        report[13] = 0x80;
                        report[14] = commandID;
                        report[15] = 0x00;
                        canSend = true;
                        //printf("IMU set to %d\n", isIMUEnabled);
                        //printf("----------------------------------------------\n");
                        break;
                    case SwitchCommands::IMU_SENSITIVITY:
                        //printf("SwitchProDriver::set_report: Rpt 0x01 IMU_SENSITIVITY\n");
                        report[13] = 0x80;
                        report[14] = commandID;
                        canSend = true;
                        break;
                    case SwitchCommands::ENABLE_VIBRATION:
                        //printf("SwitchProDriver::set_report: Rpt 0x01 ENABLE_VIBRATION\n");
                        isVibrationEnabled = buffer[11];
                        report[13] = 0x80;
                        report[14] = commandID;
                        report[15] = 0x00;
                        canSend = true;
                        //printf("Vibration set to %d\n", isVibrationEnabled);
                        //printf("----------------------------------------------\n");
                        break;
                    case SwitchCommands::READ_IMU:
                        //printf("SwitchProDriver::set_report: Rpt 0x01 READ_IMU\n");
                        report[13] = 0xC0;
                        report[14] = commandID;
                        report[15] = buffer[11];
                        report[16] = buffer[12];
                        canSend = true;
                        //printf("IMU Addr: %02x, Size: %02x\n", buffer[11], buffer[12]);
                        //printf("----------------------------------------------\n");
                        break;
                    case SwitchCommands::GET_VOLTAGE:
                        //printf("SwitchProDriver::set_report: Rpt 0x01 GET_VOLTAGE\n");
                        report[13] = 0xD0;
                        report[14] = 0x50;
                        report[15] = 0x83;
                        report[16] = 0x06;
                        canSend = true;
                        break;
                    default:
                        //printf("SwitchProDriver::set_report: Rpt 0x01 Unknown 0x%02x\n", commandID);
                        report[13] = 0x80;
                        report[14] = commandID;
                        report[15] = 0x03;
                        canSend = true;
                        break;
                }
            } else {
                //printf("SwitchProDriver::set_report: Rpt %02x Unknown ID %02x\n", switchReportID, switchReportSubID);
            }

            if (canSend) {
                tud_hid_report(report_id, report, bufsize);
            }

            break;
        default:
            //printf("SwitchProDriver::set_report: Unknown HID Report %02x\n", switchReportID);
            break;
    }
}

// Only XboxOG and Xbox One use vendor control xfer cb
bool SwitchProDriver::vendor_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const *request) {
    return false;
}

const uint16_t * SwitchProDriver::get_descriptor_string_cb(uint8_t index, uint16_t langid) {
	const char *value = (const char *)switch_pro_string_descriptors[index];
	return getStringDescriptor(value, index); // getStringDescriptor returns a static array
}

const uint8_t * SwitchProDriver::get_descriptor_device_cb() {
    return switch_pro_device_descriptor;
}

const uint8_t * SwitchProDriver::get_hid_descriptor_report_cb(uint8_t itf) {
    return switch_pro_report_descriptor;
}

const uint8_t * SwitchProDriver::get_descriptor_configuration_cb(uint8_t index) {
    return switch_pro_configuration_descriptor;
}

const uint8_t * SwitchProDriver::get_descriptor_device_qualifier_cb() {
	return nullptr;
}

uint16_t SwitchProDriver::GetJoystickMidValue() {
	return SWITCH_PRO_JOYSTICK_MID << 8;
}
