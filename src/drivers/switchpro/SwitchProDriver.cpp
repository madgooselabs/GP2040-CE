#include "drivers/switchpro/SwitchProDriver.h"
#include "drivers/shared/driverhelper.h"
#include "pico/rand.h"

void SwitchProDriver::initialize() {
    _switchRequestReport[0] = 0x80;
    _switchRequestReport[1] = 0x01;
    uint8_t newAddr[] = {0x7c,
                        0xbb,
                        0x8a,
                        (uint8_t)(get_rand_32() % 0xff),
                        (uint8_t)(get_rand_32() % 0xff),
                        (uint8_t)(get_rand_32() % 0xff)};
    memcpy(_addr, newAddr, 6);

	switchReport = {
		.batteryConnection = 0x81,
		.buttons = 0,
		.hat = SWITCH_PRO_HAT_NOTHING,
		.lx = SWITCH_PRO_JOYSTICK_MID,
		.ly = SWITCH_PRO_JOYSTICK_MID,
		.rx = SWITCH_PRO_JOYSTICK_MID,
		.ry = SWITCH_PRO_JOYSTICK_MID,
		.vendor = 0,
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
	switch (gamepad->state.dpad & GAMEPAD_MASK_DPAD)
	{
		case GAMEPAD_MASK_UP:                        switchReport.hat = SWITCH_PRO_HAT_UP;        break;
		case GAMEPAD_MASK_UP | GAMEPAD_MASK_RIGHT:   switchReport.hat = SWITCH_PRO_HAT_UPRIGHT;   break;
		case GAMEPAD_MASK_RIGHT:                     switchReport.hat = SWITCH_PRO_HAT_RIGHT;     break;
		case GAMEPAD_MASK_DOWN | GAMEPAD_MASK_RIGHT: switchReport.hat = SWITCH_PRO_HAT_DOWNRIGHT; break;
		case GAMEPAD_MASK_DOWN:                      switchReport.hat = SWITCH_PRO_HAT_DOWN;      break;
		case GAMEPAD_MASK_DOWN | GAMEPAD_MASK_LEFT:  switchReport.hat = SWITCH_PRO_HAT_DOWNLEFT;  break;
		case GAMEPAD_MASK_LEFT:                      switchReport.hat = SWITCH_PRO_HAT_LEFT;      break;
		case GAMEPAD_MASK_UP | GAMEPAD_MASK_LEFT:    switchReport.hat = SWITCH_PRO_HAT_UPLEFT;    break;
		default:                                     switchReport.hat = SWITCH_PRO_HAT_NOTHING;   break;
	}

	switchReport.buttons = 0
		| (gamepad->pressedB1() ? SWITCH_PRO_MASK_B       : 0)
		| (gamepad->pressedB2() ? SWITCH_PRO_MASK_A       : 0)
		| (gamepad->pressedB3() ? SWITCH_PRO_MASK_Y       : 0)
		| (gamepad->pressedB4() ? SWITCH_PRO_MASK_X       : 0)
		| (gamepad->pressedL1() ? SWITCH_PRO_MASK_L       : 0)
		| (gamepad->pressedR1() ? SWITCH_PRO_MASK_R       : 0)
		| (gamepad->pressedL2() ? SWITCH_PRO_MASK_ZL      : 0)
		| (gamepad->pressedR2() ? SWITCH_PRO_MASK_ZR      : 0)
		| (gamepad->pressedS1() ? SWITCH_PRO_MASK_MINUS   : 0)
		| (gamepad->pressedS2() ? SWITCH_PRO_MASK_PLUS    : 0)
		| (gamepad->pressedL3() ? SWITCH_PRO_MASK_L3      : 0)
		| (gamepad->pressedR3() ? SWITCH_PRO_MASK_R3      : 0)
		| (gamepad->pressedA1() ? SWITCH_PRO_MASK_HOME    : 0)
		| (gamepad->pressedA2() ? SWITCH_PRO_MASK_CAPTURE : 0)
	;

	switchReport.lx = static_cast<uint8_t>(gamepad->state.lx >> 8);
	switchReport.ly = static_cast<uint8_t>(gamepad->state.ly >> 8);
	switchReport.rx = static_cast<uint8_t>(gamepad->state.rx >> 8);
	switchReport.ry = static_cast<uint8_t>(gamepad->state.ry >> 8);

	// Wake up TinyUSB device
	if (tud_suspended())
		tud_remote_wakeup();

	//void * report = &switchReport;
	//uint16_t report_size = sizeof(switchReport);
	void * report = generate_usb_report();
	uint16_t report_size = 64;
	if (memcmp(last_report, report, report_size) != 0) {
		// HID ready + report sent, copy previous report
		if (tud_hid_ready() && tud_hid_report(0, report, report_size) == true ) {
			memcpy(last_report, report, report_size);
		}
	}
}

// tud_hid_get_report_cb
uint16_t SwitchProDriver::get_report(uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen) {
    memcpy(buffer, &switchReport, sizeof(SwitchProReport));
	return sizeof(SwitchProReport);
}

// Only PS4 does anything with set report
void SwitchProDriver::set_report(uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize) {}

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

uint8_t* SwitchProDriver::generate_usb_report() {
  set_empty_report();

  if (_switchRequestReport[0] == 0x80) {
    _report[0] = 0x81;
    _report[1] = _switchRequestReport[1];
    switch (_switchRequestReport[1]) {
      case 0x01:
        _report[3] = 0x03;
        for (int i = 0; i < 6; i++) {
          _report[4 + i] = _addr[5 - i];
        }
        break;
      case 0x02:
      case 0x03:
        break;
      default:
        _report[0] = 0x30;
        //_controller->getSwitchReport(&_switchReport);
        memcpy(_report + 2, (uint8_t *)&switchReport, sizeof(SwitchProReport));
        break;
    }
    if (_switchRequestReport[0] > 0x00) {
      set_empty_switch_request_report();
    }
    return _report;
  } else {
    generate_report();
    if (_switchRequestReport[0] > 0x00) {
      set_empty_switch_request_report();
    }
    // _report is a bluetooth report starting with 0xA1, which usb skips
    return _report + 1;
  }
};

void SwitchProDriver::set_empty_report() {
  memset(_report, 0x00, sizeof(_report));
}

void SwitchProDriver::set_empty_switch_request_report() {
  memset(_switchRequestReport, 0x00, sizeof(_switchRequestReport));
}

uint8_t* SwitchProDriver::generate_report() {
  set_empty_report();
  _report[0] = 0xa1;
  switch (_switchRequestReport[10]) {
    case BLUETOOTH_PAIR_REQUEST:
      set_subcommand_reply();
      //set_bt();
      break;
    case REQUEST_DEVICE_INFO:
      _device_info_queried = true;
      set_subcommand_reply();
      //set_device_info();
      break;
    case SET_SHIPMENT:
      set_subcommand_reply();
      //set_shipment();
      break;
    case SPI_READ:
      set_subcommand_reply();
      //spi_read();
      break;
    case SET_MODE:
      set_subcommand_reply();
      //set_mode();
      break;
    case TRIGGER_BUTTONS:
      set_subcommand_reply();
      //set_trigger_buttons();
      break;
    case TOGGLE_IMU:
      set_subcommand_reply();
      //toggle_imu();
      break;
    case IMU_SENSITIVITY:
      set_subcommand_reply();
      //imu_sensitivity();
      break;
    case ENABLE_VIBRATION:
      set_subcommand_reply();
      //enable_vibration();
      break;
    case SET_PLAYER:
      set_subcommand_reply();
      //set_player_lights();
      break;
    case SET_NFC_IR_STATE:
      set_subcommand_reply();
      //set_nfc_ir_state();
      break;
    case SET_NFC_IR_CONFIG:
      set_subcommand_reply();
      //set_nfc_ir_config();
      break;
    default:
      set_full_input_report();
      break;
  }
  return _report;
}

void SwitchProDriver::set_subcommand_reply() {
  // Input Report ID
  _report[1] = 0x21;

  // TODO: Find out what the vibrator byte is doing.
  // This is a hack in an attempt to semi-emulate
  // actions of the vibrator byte as it seems to change
  // when a subcommand reply is sent.
  if (_vibration_enabled) {
    _vibration_idx = (_vibration_idx + 1) % 4;
    _vibration_report = VIB_OPTS[_vibration_idx];
  }

  set_standard_input_report();
}

void SwitchProDriver::set_standard_input_report() {
  set_timer();

  //_controller->getSwitchReport(&_switchReport);
  memcpy(_report + 3, (uint8_t *)&switchReport, sizeof(SwitchProReport));
  _report[13] = _vibration_report;
}

void SwitchProDriver::set_timer() {
  // If the timer hasn't been set before
  if (_timestamp == 0) {
    _timestamp = to_ms_since_boot(get_absolute_time());
    _report[2] = 0x00;
    return;
  }

  // Get the time that has passed since the last timestamp
  // in milliseconds
  uint32_t now = to_ms_since_boot(get_absolute_time());
  uint32_t delta_t = (now - _timestamp);

  // Get how many ticks have passed in hex with overflow at 255
  // Joy-Con uses 4.96ms as the timer tick rate
  uint32_t elapsed_ticks = int(delta_t * 4);
  _timer = (_timer + elapsed_ticks) & 0xFF;

  _report[2] = _timer;
  _timestamp = now;
}

void SwitchProDriver::set_full_input_report() {
  // Setting Report ID to full standard input report ID
  _report[1] = 0x30;

  set_standard_input_report();
  //set_imu_data();
}

void SwitchProDriver::set_bt() {
  _report[14] = 0x81;
  _report[15] = 0x01;
  _report[16] = 0x03;
}

void SwitchProDriver::set_device_info() {
  // ACK Reply
  _report[14] = 0x82;

  // Subcommand Reply
  _report[15] = 0x02;

  // Firmware version
  _report[16] = 0x03;
  _report[17] = 0x48;

  // Controller ID
  _report[18] = 0x03;

  // Unknown Byte, always 2
  _report[19] = 0x02;

  // Controller Bluetooth Address
  memcpy(_report + 20, _addr, 6);

  // Unknown byte, always 1
  _report[26] = 0x01;

  // Controller colours location (read from SPI)
  _report[27] = 0x01;
}

void SwitchProDriver::set_shipment() {
  // ACK Reply
  _report[14] = 0x80;

  // Subcommand reply
  _report[15] = 0x08;
}

void SwitchProDriver::toggle_imu() {
  _imu_enabled = _switchRequestReport[11] == 0x01;

  // ACK Reply
  _report[14] = 0x80;

  // Subcommand reply
  _report[15] = 0x40;
}

void SwitchProDriver::imu_sensitivity() {
  _report[14] = 0x80;
  _report[15] = 0x41;
}

void SwitchProDriver::set_imu_data() {
  if (!_imu_enabled) {
    return;
  }

  uint8_t imu_data[49] = {0x75, 0xFD, 0xFD, 0xFF, 0x09, 0x10, 0x21, 0x00, 0xD5,
                          0xFF, 0xE0, 0xFF, 0x72, 0xFD, 0xF9, 0xFF, 0x0A, 0x10,
                          0x22, 0x00, 0xD5, 0xFF, 0xE0, 0xFF, 0x76, 0xFD, 0xFC,
                          0xFF, 0x09, 0x10, 0x23, 0x00, 0xD5, 0xFF, 0xE0, 0xFF};
  memcpy(_report + 14, imu_data, sizeof(imu_data));
}
void SwitchProDriver::spi_read() {
  uint8_t addr_top = _switchRequestReport[12];
  uint8_t addr_bottom = _switchRequestReport[11];
  uint8_t read_length = _switchRequestReport[15];

  // ACK byte
  _report[14] = 0x90;

  // Subcommand reply
  _report[15] = 0x10;

  // Read address
  _report[16] = addr_bottom;
  _report[17] = addr_top;

  // Read length
  _report[20] = read_length;

  // Stick Parameters
  // Params are generally the same for all sticks
  // Notable difference is the deadzone (10%
  // Joy-Con vs 15% Pro Con)
  uint8_t params[18] = {0x0F, 0x30, 0x61,  // Unused
                        0x96, 0x30,
                        0xF3,               // Dead Zone/Range Ratio
                        0xD4, 0x14, 0x54,   // X/Y ?
                        0x41, 0x15, 0x54,   // X/Y ?
                        0xC7, 0x79, 0x9C,   // X/Y ?
                        0x33, 0x36, 0x63};  // X/Y ?

  // Serial Number read
  if (addr_top == 0x60 && addr_bottom == 0x00) {
    // Switch will take this as no serial number
    memset(_report + 21, 0xff, 16);
  } else if (addr_top == 0x60 && addr_bottom == 0x50) {
    // Body colour
    memset(_report + 21, 0x32, 3);
    // Buttons colour
    memset(_report + 24, 0xff, 3);
    // Left/right grip colours (Pro controller)
    memset(_report + 27, 0xff, 7);
  } else if (addr_top == 0x60 && addr_bottom == 0x80) {
    // Six-Axis factory parameters
    _report[21] = 0x50;
    _report[22] = 0xFD;
    _report[23] = 0x00;
    _report[24] = 0x00;
    _report[25] = 0xC6;
    _report[26] = 0x0F;

    memcpy(_report + 27, params, sizeof(params));

    // Stick device parameters 2
  } else if (addr_top == 0x60 && addr_bottom == 0x98) {
    // Setting same params since controllers always
    // have duplicates of stick params 1 for stick params 2
    memcpy(_report + 21, params, sizeof(params));

    // User analog stick calibration
  } else if (addr_top == 0x80 && addr_bottom == 0x10) {
    // Fill report with null user calibration info
    memset(_report + 21, 0xff, 3);

    // Factory analog stick calibration
  } else if (addr_top == 0x60 && addr_bottom == 0x3D) {
    // Left/right stick calibration
    uint8_t l_calibration[9] = {0xD4, 0x75, 0x61, 0xE5, 0x87,
                                0x7C, 0xEC, 0x55, 0x61};
    uint8_t r_calibration[9] = {0x5D, 0xD8, 0x7F, 0x18, 0xE6,
                                0x61, 0x86, 0x65, 0x5D};

    // Left stick calibration

    memcpy(_report + 21, l_calibration, sizeof(l_calibration));

    // Right stick calibration
    memcpy(_report + 30, r_calibration, sizeof(r_calibration));

    // Spacer byte
    _report[39] = 0xFF;

    // Body colour
    memset(_report + 40, 0x32, 3);
    // Buttons colour
    memset(_report + 43, 0xff, 3);

    // Six-Axis motion sensor factor
    // calibration
  } else if (addr_top == 0x60 && addr_bottom == 0x20) {
    // 1: Acceleration origin position
    // 2: Acceleration sensitivity coefficient
    // 3: Gyro origin when still
    // 4: Gyro sensitivity coefficient
    uint8_t sa_calibration[24] = {0xcc, 0x00, 0x40, 0x00, 0x91, 0x01,   // 1
                                  0x00, 0x40, 0x00, 0x40, 0x00, 0x40,   // 2
                                  0xe7, 0xff, 0x0e, 0x00, 0xdc, 0xff,   // 3
                                  0x3b, 0x34, 0x3b, 0x34, 0x3b, 0x34};  // 4

    memcpy(_report + 21, sa_calibration, sizeof(sa_calibration));
  } else {
    memset(_report + 21, 0xFF, read_length);
  }
}

void SwitchProDriver::set_mode() {
  // ACK byte
  _report[14] = 0x80;

  // Subcommand reply
  _report[15] = 0x03;
}

void SwitchProDriver::set_trigger_buttons() {
  // ACK byte
  _report[14] = 0x83;

  // Subcommand reply
  _report[15] = 0x04;
}

void SwitchProDriver::enable_vibration() {
  // ACK Reply
  _report[14] = 0x80;

  // Subcommand reply
  _report[15] = 0x48;

  // Set class property
  _vibration_enabled = true;
  _vibration_idx = 0;
  _vibration_report = VIB_OPTS[_vibration_idx];
}

void SwitchProDriver::set_player_lights() {
  // ACK byte
  _report[14] = 0x80;

  // Subcommand reply
  _report[15] = 0x30;

  uint8_t bitfield = _switchRequestReport[11];

  if (bitfield == 0x01 || bitfield == 0x10) {
    _player_number = 1;
  } else if (bitfield == 0x03 || bitfield == 0x30) {
    _player_number = 2;
  } else if (bitfield == 0x07 || bitfield == 0x70) {
    _player_number = 3;
  } else if (bitfield == 0x0F || bitfield == 0xF0) {
    _player_number = 4;
  }
}

void SwitchProDriver::set_nfc_ir_state() {
  // ACK byte
  _report[14] = 0x80;

  // Subcommand reply
  _report[15] = 0x22;
}

void SwitchProDriver::set_nfc_ir_config() {
  // ACK byte
  _report[14] = 0xA0;

  // Subcommand reply
  _report[15] = 0x21;

  // NFC/IR state data
  uint8_t params[8] = {0x01, 0x00, 0xFF, 0x00, 0x08, 0x00, 0x1B, 0x01};
  memcpy(_report + 16, params, sizeof(params));
  _report[49] = 0xC8;
}

void SwitchProDriver::set_controller_rumble(bool rumble) {
  //_controller->setRumble(rumble);
}