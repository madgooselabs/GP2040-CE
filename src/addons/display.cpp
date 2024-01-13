/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: Copyright (c) 2021 Jason Skuby (mytechtoybox.com)
 */

#include "addons/display.h"
#include "GamepadState.h"
#include "enums.h"
#include "storagemanager.h"
#include "pico/stdlib.h"
#include "ps4_driver.h"
#include "version.h"
#include "config.pb.h"
#include "usb_driver.h"

bool DisplayAddon::available() {
	const DisplayOptions& options = Storage::getInstance().getDisplayOptions();
	return options.enabled && PeripheralManager::getInstance().isI2CEnabled(options.i2cBlock);
}

void DisplayAddon::setup() {
	const DisplayOptions& options = Storage::getInstance().getDisplayOptions();
	PeripheralI2C* i2c = PeripheralManager::getInstance().getI2C(options.i2cBlock);

	stdio_init_all();

	gpDisplay = new GPGFX();

	GPGFX_DisplayTypeOptions gpOptions;

	if (PeripheralManager::getInstance().isI2CEnabled(options.i2cBlock)) {
		gpOptions.displayType = GPGFX_DisplayType::TYPE_SSD1306;
		gpOptions.i2c = i2c;
	    gpOptions.size = options.size;
		gpOptions.address = options.i2cAddress;
		gpOptions.orientation = options.flip;
		gpOptions.inverted = options.invert;
		gpOptions.font.fontData = GP_Font_Standard;
		gpOptions.font.width = 6;
		gpOptions.font.height = 8;
	}

	gpDisplay->init(gpOptions);

	map<DisplayMode, GPScreen*>::iterator screenIterator;
	for (screenIterator = loadedScreens.begin(); screenIterator != loadedScreens.end(); screenIterator++) {
		screenIterator->second->setRenderer(gpDisplay);
	}
	((MainMenuScreen*)loadedScreens[DisplayAddon::DisplayMode::MAIN_MENU])->setMenu(currentMenu);

	gamepad = Storage::getInstance().GetGamepad();
	pGamepad = Storage::getInstance().GetProcessedGamepad();

	const FocusModeOptions& focusModeOptions = Storage::getInstance().getAddonOptions().focusModeOptions;
	isFocusModeEnabled = focusModeOptions.enabled && focusModeOptions.oledLockEnabled &&
		isValidPin(focusModeOptions.pin);
	prevButtonState = 0;
	displaySaverTimer = options.displaySaverTimeout;
	displaySaverTimeout = displaySaverTimer;
	configMode = Storage::getInstance().GetConfigMode();
	turnOffWhenSuspended = options.turnOffWhenSuspended;

	const InputHistoryOptions& inputHistoryOptions = Storage::getInstance().getAddonOptions().inputHistoryOptions;
	isInputHistoryEnabled = inputHistoryOptions.enabled;
}

bool DisplayAddon::isDisplayPowerOff()
{
	if (turnOffWhenSuspended && get_usb_suspended()) {
		if (displayIsPowerOn) setDisplayPower(0);
		return true;
	} else {
		if (!displayIsPowerOn) setDisplayPower(1);
	}

	if (!displaySaverTimeout && !isFocusModeEnabled) return false;

	float diffTime = getMillis() - prevMillis;
	displaySaverTimer -= diffTime;
	if (!!displaySaverTimeout && (gamepad->state.buttons || gamepad->state.dpad) && !focusModePrevState) {
		displaySaverTimer = displaySaverTimeout;
		setDisplayPower(1);
	} else if (!!displaySaverTimeout && displaySaverTimer <= 0) {
		setDisplayPower(0);
	}

	if (isFocusModeEnabled) {
		const FocusModeOptions& focusModeOptions = Storage::getInstance().getAddonOptions().focusModeOptions;
		bool isFocusModeActive = !gpio_get(focusModeOptions.pin);
		if (focusModePrevState != isFocusModeActive) {
			focusModePrevState = isFocusModeActive;
			if (isFocusModeActive) {
				setDisplayPower(0);
			} else {
				setDisplayPower(1);
			}
		}
	}

	prevMillis = getMillis();

	return (isFocusModeEnabled && focusModePrevState) || (!!displaySaverTimeout && displaySaverTimer <= 0);
}

void DisplayAddon::setDisplayPower(uint8_t status)
{
	if (displayIsPowerOn != status) {
		displayIsPowerOn = status;
		gpDisplay->getDriver()->setPower(status);
	}
}

void DisplayAddon::process() {
	if (!configMode && isDisplayPowerOff()) return;

	gpScreen = loadedScreens.find(getDisplayMode())->second;
	gpScreen->setDisplayOptions(Storage::getInstance().getDisplayOptions());

	drawStatusBar(gamepad);
	gpScreen->setGamepadState(pGamepad->state);

	if(isInputHistoryEnabled && inputHistoryAddon != nullptr) {
		//inputHistoryAddon->drawHistory(gpDisplay);
		gpScreen->footer = inputHistoryAddon->getHistory().c_str();
	}

	gpScreen->draw();
}

void DisplayAddon::testMenu() {
	currDisplayMode = DisplayAddon::DisplayMode::BUTTONS;
}

DisplayAddon::DisplayMode DisplayAddon::getDisplayMode() {
	if (configMode) {
		gamepad->read();
		uint16_t buttonState = gamepad->state.buttons;
		if (prevButtonState && !buttonState) { // has button been pressed (held and released)?
			switch (prevButtonState) {
				case (GAMEPAD_MASK_B1):
					prevDisplayMode =
						prevDisplayMode == DisplayAddon::DisplayMode::BUTTONS ?
							DisplayAddon::DisplayMode::CONFIG_INSTRUCTION : DisplayAddon::DisplayMode::BUTTONS;
						break;
				case (GAMEPAD_MASK_B2):
					prevDisplayMode =
						prevDisplayMode == DisplayAddon::DisplayMode::SPLASH ?
							DisplayAddon::DisplayMode::CONFIG_INSTRUCTION : DisplayAddon::DisplayMode::SPLASH;
					break;
				default:
					prevDisplayMode = DisplayAddon::DisplayMode::CONFIG_INSTRUCTION;
			}
		}
		prevButtonState = buttonState;
		return prevDisplayMode;
	} else {
		if (Storage::getInstance().getDisplayOptions().splashMode != static_cast<SplashMode>(SPLASH_MODE_NONE)) {
			uint32_t splashDuration = getDisplayOptions().splashDuration;
			if (splashDuration == 0 || getMillis() < splashDuration) {
				return DisplayAddon::DisplayMode::SPLASH;
			}
		}
	}

	//return DisplayAddon::DisplayMode::BUTTONS;
	//return DisplayAddon::DisplayMode::MAIN_MENU;
	return currDisplayMode;
}

const DisplayOptions& DisplayAddon::getDisplayOptions() {
	bool configMode = Storage::getInstance().GetConfigMode();
	return configMode ? Storage::getInstance().getPreviewDisplayOptions() : Storage::getInstance().getDisplayOptions();
}

void DisplayAddon::drawText(int x, int y, std::string text) {
	gpDisplay->drawText(x, y, text);
}

void DisplayAddon::drawStatusBar(Gamepad * gamepad)
{
	const TurboOptions& turboOptions = Storage::getInstance().getAddonOptions().turboOptions;

	// Limit to 21 chars with 6x8 font for now
	statusBar.clear();

	switch (gamepad->getOptions().inputMode)
	{
		case INPUT_MODE_HID:    statusBar += "DINPUT"; break;
		case INPUT_MODE_SWITCH: statusBar += "SWITCH"; break;
		case INPUT_MODE_XINPUT: statusBar += "XINPUT"; break;
		case INPUT_MODE_MDMINI: statusBar += "GEN/MD"; break;
		case INPUT_MODE_NEOGEO: statusBar += "NGMINI"; break;
		case INPUT_MODE_PCEMINI: statusBar += "PCE/TG"; break;
		case INPUT_MODE_EGRET: statusBar += "EGRET"; break;
		case INPUT_MODE_ASTRO: statusBar += "ASTRO"; break;
		case INPUT_MODE_PSCLASSIC: statusBar += "PSC"; break;
		case INPUT_MODE_XBOXORIGINAL: statusBar += "OGXBOX"; break;
		case INPUT_MODE_PS4:
			if ( PS4Data::getInstance().ps4ControllerType == PS4ControllerType::PS4_CONTROLLER ) {
				if (PS4Data::getInstance().authsent == true )
					statusBar += "PS4:AS";
				else
					statusBar += "PS4   ";
			} else if ( PS4Data::getInstance().ps4ControllerType == PS4ControllerType::PS4_ARCADESTICK ) {
				if (PS4Data::getInstance().authsent == true )
					statusBar += "PS5:AS";
				else
					statusBar += "PS5   ";
			}
			break;
		case INPUT_MODE_XBONE:    statusBar += "XBONE"; break;
		case INPUT_MODE_KEYBOARD: statusBar += "HID-KB"; break;
		case INPUT_MODE_CONFIG: statusBar += "CONFIG"; break;
	}

	if ( turboOptions.enabled && isValidPin(turboOptions.buttonPin) ) {
		statusBar += " T";
		if ( turboOptions.shotCount < 10 ) // padding
			statusBar += "0";
		statusBar += std::to_string(turboOptions.shotCount);
	} else {
		statusBar += "    "; // no turbo, don't show Txx setting
	}
	switch (gamepad->getOptions().dpadMode)
	{

		case DPAD_MODE_DIGITAL:      statusBar += " D"; break;
		case DPAD_MODE_LEFT_ANALOG:  statusBar += " L"; break;
		case DPAD_MODE_RIGHT_ANALOG: statusBar += " R"; break;
	}

	switch (Gamepad::resolveSOCDMode(gamepad->getOptions()))
	{
		case SOCD_MODE_NEUTRAL:               statusBar += " SOCD-N"; break;
		case SOCD_MODE_UP_PRIORITY:           statusBar += " SOCD-U"; break;
		case SOCD_MODE_SECOND_INPUT_PRIORITY: statusBar += " SOCD-L"; break;
		case SOCD_MODE_FIRST_INPUT_PRIORITY:  statusBar += " SOCD-F"; break;
		case SOCD_MODE_BYPASS:                statusBar += " SOCD-X"; break;
	}
	if (Storage::getInstance().getAddonOptions().macroOptions.enabled)
		statusBar += " M";

	gpScreen->header = statusBar;
}

void DisplayAddon::attachInputHistoryAddon(InputHistoryAddon* pInputHistoryAddon) {
	inputHistoryAddon = pInputHistoryAddon;
}
