/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: Copyright (c) 2021 Jason Skuby (mytechtoybox.com)
 */

#include "addons/display.h"
#include "GamepadState.h"
#include "enums.h"
#include "storagemanager.h"
#include "pico/stdlib.h"
#include "bitmaps.h"
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

	gpDisplay = new GPGFX();

	GPGFX_DisplayTypeOptions gpOptions;

	if (PeripheralManager::getInstance().isI2CEnabled(options.i2cBlock)) {
		gpOptions.displayType = GPGFX_DisplayType::SSD1306;
		gpOptions.i2c = i2c;
	    gpOptions.size = options.size;
		gpOptions.address = options.i2cAddress;
		gpOptions.orientation = options.flip;
		gpOptions.inverted = options.invert;
	}

	gpDisplay->init(&gpOptions);

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

	gpDisplay->clearScreen();

	switch (getDisplayMode()) {
		case DisplayAddon::DisplayMode::CONFIG_INSTRUCTION:
			drawStatusBar(gamepad);
			drawText(0, 2, "[Web Config Mode]");
			drawText(0, 3, std::string("GP2040-CE : ") + std::string(GP2040VERSION));
			drawText(0, 4, "[http://192.168.7.1]");
			drawText(0, 5, "Preview:");
			drawText(5, 6, "B1 > Button");
			drawText(5, 7, "B2 > Splash");
			break;
		case DisplayAddon::DisplayMode::SPLASH:
			if (getDisplayOptions().splashMode == static_cast<SplashMode>(SPLASH_MODE_NONE)) {
				drawText(0, 4, " Splash NOT enabled.");
				break;
			}
			drawSplashScreen(getDisplayOptions().splashMode, (uint8_t*) Storage::getInstance().getDisplayOptions().splashImage.bytes, 90);
			break;
		case DisplayAddon::DisplayMode::BUTTONS:
			drawStatusBar(gamepad);
			const DisplayOptions& options = getDisplayOptions();
			ButtonLayoutCustomOptions buttonLayoutCustomOptions = options.buttonLayoutCustomOptions;

			switch (options.buttonLayout) {
				case BUTTON_LAYOUT_STICK:
					drawArcadeStick(8, (isInputHistoryEnabled ? 22 : 28), 8, 2);
					break;
				case BUTTON_LAYOUT_STICKLESS:
					drawStickless(8, 20, 8, 2);
					break;
				case BUTTON_LAYOUT_BUTTONS_ANGLED:
					drawWasdBox(8, (isInputHistoryEnabled ? 22 : 28), 7, 3);
					break;
				case BUTTON_LAYOUT_BUTTONS_BASIC:
					drawUDLR(8, (isInputHistoryEnabled ? 22 : 28), 8, 2);
					break;
				case BUTTON_LAYOUT_KEYBOARD_ANGLED:
					drawKeyboardAngled(18, (isInputHistoryEnabled ? 24 : 28), 5, 2);
					break;
				case BUTTON_LAYOUT_KEYBOARDA:
					drawMAMEA(8, (isInputHistoryEnabled ? 22 : 28), 10, 1);
					break;
				case BUTTON_LAYOUT_OPENCORE0WASDA:
					drawOpenCore0WASDA(16, (isInputHistoryEnabled ? 22 : 28), 10, 1);
					break;
				case BUTTON_LAYOUT_DANCEPADA:
					drawDancepadA(39, (isInputHistoryEnabled ? 10 : 12), (isInputHistoryEnabled ? 13 : 15), 2);
					break;
				case BUTTON_LAYOUT_TWINSTICKA:
					drawTwinStickA(8, (isInputHistoryEnabled ? 22 : 28), 8, 2);
					break;
				case BUTTON_LAYOUT_BLANKA:
					drawBlankA(0, 0, 0, 0);
					break;
				case BUTTON_LAYOUT_VLXA:
					drawVLXA(7, (isInputHistoryEnabled ? 22 : 28), 7, 2);
					break;
				case BUTTON_LAYOUT_CUSTOMA:
					drawButtonLayoutLeft(buttonLayoutCustomOptions.paramsLeft);
					break;
				case BUTTON_LAYOUT_FIGHTBOARD_STICK:
					drawArcadeStick(18, 22, 8, 2);
					break;
				case BUTTON_LAYOUT_FIGHTBOARD_MIRRORED:
					drawFightboardMirrored(0, 22, 7, 2);
					break;
			}

			switch (options.buttonLayoutRight) {
				case BUTTON_LAYOUT_ARCADE:
					drawArcadeButtons(8, (isInputHistoryEnabled ? 22 : 28), 8, 2);
					break;
				case BUTTON_LAYOUT_STICKLESSB:
					drawSticklessButtons(8, 20, 8, 2);
					break;
				case BUTTON_LAYOUT_BUTTONS_ANGLEDB:
					drawWasdButtons(8, (isInputHistoryEnabled ? 22 : 28), 7, 3);
					break;
				case BUTTON_LAYOUT_VEWLIX:
					drawVewlix(8, (isInputHistoryEnabled ? 22 : 28), 8, 2);
					break;
				case BUTTON_LAYOUT_VEWLIX7:
					drawVewlix7(8, (isInputHistoryEnabled ? 22 : 28), 8, 2);
					break;
				case BUTTON_LAYOUT_CAPCOM:
					drawCapcom(6, (isInputHistoryEnabled ? 22 : 28), 8, 2);
					break;
				case BUTTON_LAYOUT_CAPCOM6:
					drawCapcom6(16, (isInputHistoryEnabled ? 22 : 28), 8, 2);
					break;
				case BUTTON_LAYOUT_SEGA2P:
					drawSega2p(8, (isInputHistoryEnabled ? 22 : 28), 8, 2);
					break;
				case BUTTON_LAYOUT_NOIR8:
					drawNoir8(8, (isInputHistoryEnabled ? 22 : 28), 8, 2);
					break;
				case BUTTON_LAYOUT_KEYBOARDB:
					drawMAMEB(68, (isInputHistoryEnabled ? 22 : 28), 10, 1);
					break;
				case BUTTON_LAYOUT_KEYBOARD8B:
					drawMAME8B(68, (isInputHistoryEnabled ? 22 : 28), 10, 1);
					break;
				case BUTTON_LAYOUT_OPENCORE0WASDB:
					drawOpenCore0WASDB(68, (isInputHistoryEnabled ? 22 : 28), 10, 1);
					break;
				case BUTTON_LAYOUT_DANCEPADB:
					drawDancepadB(39, (isInputHistoryEnabled ? 10 : 12), (isInputHistoryEnabled ? 13 : 15), 2);
					break;
				case BUTTON_LAYOUT_TWINSTICKB:
					drawTwinStickB(100, (isInputHistoryEnabled ? 22 : 28), 8, 2);
					break;
				case BUTTON_LAYOUT_BLANKB:
					drawSticklessButtons(0, 0, 0, 0);
					break;
				case BUTTON_LAYOUT_VLXB:
					drawVLXB(6, (isInputHistoryEnabled ? 22 : 28), 7, 2);
					break;
				case BUTTON_LAYOUT_CUSTOMB:
					drawButtonLayoutRight(buttonLayoutCustomOptions.paramsRight);
					break;
				case BUTTON_LAYOUT_FIGHTBOARD:
					drawFightboard(8, 22, 7, 3);
					break;
				case BUTTON_LAYOUT_FIGHTBOARD_STICK_MIRRORED:
					drawArcadeStick(90, 22, 8, 2);
					break;
			}

			if(isInputHistoryEnabled && inputHistoryAddon != nullptr) {
				//inputHistoryAddon->drawHistory(&obd);
			}

			break;
	}

	gpDisplay->getDriver()->drawBuffer(NULL);
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

	return DisplayAddon::DisplayMode::BUTTONS;
}

const DisplayOptions& DisplayAddon::getDisplayOptions() {
	bool configMode = Storage::getInstance().GetConfigMode();
	return configMode ? Storage::getInstance().getPreviewDisplayOptions() : Storage::getInstance().getDisplayOptions();
}

void DisplayAddon::drawButtonLayoutLeft(ButtonLayoutParamsLeft& options)
{
	int32_t& startX    = options.common.startX;
	int32_t& startY    = options.common.startY;
	int32_t& buttonRadius  = options.common.buttonRadius;
	int32_t& buttonPadding = options.common.buttonPadding;

	switch (options.layout)
		{
			case BUTTON_LAYOUT_STICK:
				drawArcadeStick(startX, startY, buttonRadius, buttonPadding);
				break;
			case BUTTON_LAYOUT_STICKLESS:
				drawStickless(startX, startY, buttonRadius, buttonPadding);
				break;
			case BUTTON_LAYOUT_BUTTONS_ANGLED:
				drawWasdBox(startX, startY, buttonRadius, buttonPadding);
				break;
			case BUTTON_LAYOUT_BUTTONS_BASIC:
				drawUDLR(startX, startY, buttonRadius, buttonPadding);
				break;
			case BUTTON_LAYOUT_KEYBOARD_ANGLED:
				drawKeyboardAngled(startX, startY, buttonRadius, buttonPadding);
				break;
			case BUTTON_LAYOUT_KEYBOARDA:
				drawMAMEA(startX, startY, buttonRadius, buttonPadding);
				break;
			case BUTTON_LAYOUT_OPENCORE0WASDA:
				drawOpenCore0WASDA(startX, startY, buttonRadius, buttonPadding);
				break;
			case BUTTON_LAYOUT_DANCEPADA:
				drawDancepadA(startX, startY, buttonRadius, buttonPadding);
				break;
			case BUTTON_LAYOUT_TWINSTICKA:
				drawTwinStickA(startX, startY, buttonRadius, buttonPadding);
				break;
			case BUTTON_LAYOUT_BLANKA:
				drawBlankA(startX, startY, buttonRadius, buttonPadding);
				break;
			case BUTTON_LAYOUT_VLXA:
				drawVLXA(startX, startY, buttonRadius, buttonPadding);
				break;
			case BUTTON_LAYOUT_FIGHTBOARD_STICK:
				drawArcadeStick(startX, startY, buttonRadius, buttonPadding);
				break;
			case BUTTON_LAYOUT_FIGHTBOARD_MIRRORED:
				drawFightboardMirrored(startX, startY, buttonRadius, buttonPadding);
				break;
		}
}

void DisplayAddon::drawButtonLayoutRight(ButtonLayoutParamsRight& options)
{
	int32_t& startX        = options.common.startX;
	int32_t& startY        = options.common.startY;
	int32_t& buttonRadius  = options.common.buttonRadius;
	int32_t& buttonPadding = options.common.buttonPadding;

	switch (options.layout)
		{
			case BUTTON_LAYOUT_ARCADE:
				drawArcadeButtons(startX, startY, buttonRadius, buttonPadding);
				break;
			case BUTTON_LAYOUT_STICKLESSB:
				drawSticklessButtons(startX, startY, buttonRadius, buttonPadding);
				break;
			case BUTTON_LAYOUT_BUTTONS_ANGLEDB:
				drawWasdButtons(startX, startY, buttonRadius, buttonPadding);
				break;
			case BUTTON_LAYOUT_VEWLIX:
				drawVewlix(startX, startY, buttonRadius, buttonPadding);
				break;
			case BUTTON_LAYOUT_VEWLIX7:
				drawVewlix7(startX, startY, buttonRadius, buttonPadding);
				break;
			case BUTTON_LAYOUT_CAPCOM:
				drawCapcom(startX, startY, buttonRadius, buttonPadding);
				break;
			case BUTTON_LAYOUT_CAPCOM6:
				drawCapcom6(startX, startY, buttonRadius, buttonPadding);
				break;
			case BUTTON_LAYOUT_SEGA2P:
				drawSega2p(startX, startY, buttonRadius, buttonPadding);
				break;
			case BUTTON_LAYOUT_NOIR8:
				drawNoir8(startX, startY, buttonRadius, buttonPadding);
				break;
			case BUTTON_LAYOUT_KEYBOARDB:
				drawMAMEB(startX, startY, buttonRadius, buttonPadding);
				break;
			case BUTTON_LAYOUT_KEYBOARD8B:
				drawMAME8B(startX, startY, buttonRadius, buttonPadding);
				break;
			case BUTTON_LAYOUT_OPENCORE0WASDB:
				drawOpenCore0WASDB(startX, startY, buttonRadius, buttonPadding);
				break;
			case BUTTON_LAYOUT_DANCEPADB:
				drawDancepadB(startX, startY, buttonRadius, buttonPadding);
				break;
			case BUTTON_LAYOUT_TWINSTICKB:
				drawTwinStickB(startX, startY, buttonRadius, buttonPadding);
				break;
			case BUTTON_LAYOUT_BLANKB:
				drawSticklessButtons(startX, startY, buttonRadius, buttonPadding);
				break;
			case BUTTON_LAYOUT_VLXB:
				drawVLXB(startX, startY, buttonRadius, buttonPadding);
				break;
			case BUTTON_LAYOUT_FIGHTBOARD:
				drawFightboard(startX, startY, buttonRadius, buttonPadding);
				break;
			case BUTTON_LAYOUT_FIGHTBOARD_STICK_MIRRORED:
				drawArcadeStick(startX, startY, buttonRadius, buttonPadding);
				break;
		}
}

void DisplayAddon::drawDiamond(int cx, int cy, int size, uint8_t colour, uint8_t filled)
{
	if (filled) {
		int i;
		for (i = 0; i < size; i++) {
			gpDisplay->drawLine(cx - i, cy - size + i, cx + i, cy - size + i, colour, 0);
			gpDisplay->drawLine(cx - i, cy + size - i, cx + i, cy + size - i, colour, 0);
		}
		gpDisplay->drawLine(cx - size, cy, cx + size, cy, colour, 0); // Fill in the middle
	}
	gpDisplay->drawLine(cx - size, cy, cx, cy - size, colour, 0);
	gpDisplay->drawLine(cx, cy - size, cx + size, cy, colour, 0);
	gpDisplay->drawLine(cx + size, cy, cx, cy + size, colour, 0);
	gpDisplay->drawLine(cx, cy + size, cx - size, cy, colour, 0);
}

void DisplayAddon::drawStickless(int startX, int startY, int buttonRadius, int buttonPadding)
{

	const int buttonMargin = buttonPadding + (buttonRadius * 2);

	gpDisplay->drawEllipse(startX, startY, buttonRadius, buttonRadius, 1, pressedLeft());
	gpDisplay->drawEllipse(startX + buttonMargin, startY, buttonRadius, buttonRadius, 1, pressedDown());
	gpDisplay->drawEllipse(startX + (buttonMargin * 1.875), startY + (buttonMargin / 2), buttonRadius, buttonRadius, 1, pressedRight());
	gpDisplay->drawEllipse(startX + (buttonMargin * (isInputHistoryEnabled ? 1.875 : 2.25)), startY + buttonMargin * (isInputHistoryEnabled ? 1.5 : 1.875), buttonRadius, buttonRadius, 1, pressedUp());
}

void DisplayAddon::drawWasdBox(int startX, int startY, int buttonRadius, int buttonPadding)
{
	const int buttonMargin = buttonPadding + (buttonRadius * 2);

	// WASD
	gpDisplay->drawEllipse(startX, startY + buttonMargin * 0.5, buttonRadius, buttonRadius, 1, pressedLeft());
	gpDisplay->drawEllipse(startX + buttonMargin, startY + buttonMargin * 0.875, buttonRadius, buttonRadius, 1, pressedDown());
	gpDisplay->drawEllipse(startX + buttonMargin * 1.5, startY - buttonMargin * 0.125, buttonRadius, buttonRadius, 1, pressedUp());
	gpDisplay->drawEllipse(startX + (buttonMargin * 2), startY + buttonMargin * 1.25, buttonRadius, buttonRadius, 1, pressedRight());
}

void DisplayAddon::drawUDLR(int startX, int startY, int buttonRadius, int buttonPadding)
{
	const int buttonMargin = buttonPadding + (buttonRadius * 2);

	// UDLR
	gpDisplay->drawEllipse(startX, startY + buttonMargin / 2, buttonRadius, buttonRadius, 1, pressedLeft());
	gpDisplay->drawEllipse(startX + (buttonMargin * 0.875), startY - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pressedUp());
	gpDisplay->drawEllipse(startX + (buttonMargin * 0.875), startY + buttonMargin * 1.25, buttonRadius, buttonRadius, 1, pressedDown());
	gpDisplay->drawEllipse(startX + (buttonMargin * 1.625), startY + buttonMargin / 2, buttonRadius, buttonRadius, 1, pressedRight());
}

void DisplayAddon::drawArcadeStick(int startX, int startY, int buttonRadius, int buttonPadding)
{
	const int buttonMargin = buttonPadding + (buttonRadius * 2);

	// Stick
	gpDisplay->drawEllipse(startX + (buttonMargin / 2), startY + (buttonMargin / 2), buttonRadius * 1.25, buttonRadius * 1.25, 1, 0);

	if (pressedUp()) {
		if (pressedLeft()) {
			gpDisplay->drawEllipse(startX + (buttonMargin / 5), startY + (buttonMargin / 5), buttonRadius, buttonRadius, 1, 1);
		} else if (pressedRight()) {
			gpDisplay->drawEllipse(startX + (buttonMargin * 0.875), startY + (buttonMargin / 5), buttonRadius, buttonRadius, 1, 1);
		} else {
			gpDisplay->drawEllipse(startX + (buttonMargin / 2), startY, buttonRadius, buttonRadius, 1, 1);
		}
	} else if (pressedDown()) {
		if (pressedLeft()) {
			gpDisplay->drawEllipse(startX + (buttonMargin / 5), startY + (buttonMargin * 0.875), buttonRadius, buttonRadius, 1, 1);
		} else if (pressedRight()) {
			gpDisplay->drawEllipse(startX + (buttonMargin * 0.875), startY + (buttonMargin * 0.875), buttonRadius, buttonRadius, 1, 1);
		} else {
			gpDisplay->drawEllipse(startX + buttonMargin / 2, startY + buttonMargin, buttonRadius, buttonRadius, 1, 1);
		}
	} else if (pressedLeft()) {
		gpDisplay->drawEllipse(startX, startY + buttonMargin / 2, buttonRadius, buttonRadius, 1, 1);
	} else if (pressedRight()) {
		gpDisplay->drawEllipse(startX + buttonMargin, startY + buttonMargin / 2, buttonRadius, buttonRadius, 1, 1);
	} else {
		gpDisplay->drawEllipse(startX + buttonMargin / 2, startY + buttonMargin / 2, buttonRadius, buttonRadius, 1, 1);
	}
}

void DisplayAddon::drawVLXA(int startX, int startY, int buttonRadius, int buttonPadding)
{
	const int buttonMargin = buttonPadding + (buttonRadius * 2);

	// Stick
	gpDisplay->drawEllipse(startX + (buttonMargin / 2), startY + (buttonMargin / 2), buttonRadius * 1.25, buttonRadius * 1.25, 1, 0);

	if (pressedUp()) {
		if (pressedLeft()) {
			gpDisplay->drawEllipse(startX + (buttonMargin / 5), startY + (buttonMargin / 5), buttonRadius, buttonRadius, 1, 1);
		} else if (pressedRight()) {
			gpDisplay->drawEllipse(startX + (buttonMargin * 0.875), startY + (buttonMargin / 5), buttonRadius, buttonRadius, 1, 1);
		} else {
			gpDisplay->drawEllipse(startX + (buttonMargin / 2), startY, buttonRadius, buttonRadius, 1, 1);
		}
	} else if (pressedDown()) {
		if (pressedLeft()) {
			gpDisplay->drawEllipse(startX + (buttonMargin / 5), startY + (buttonMargin * 0.875), buttonRadius, buttonRadius, 1, 1);
		} else if (pressedRight()) {
			gpDisplay->drawEllipse(startX + (buttonMargin * 0.875), startY + (buttonMargin * 0.875), buttonRadius, buttonRadius, 1, 1);
		} else {
			gpDisplay->drawEllipse(startX + buttonMargin / 2, startY + buttonMargin, buttonRadius, buttonRadius, 1, 1);
		}
	} else if (pressedLeft()) {
		gpDisplay->drawEllipse(startX, startY + buttonMargin / 2, buttonRadius, buttonRadius, 1, 1);
	} else if (pressedRight()) {
		gpDisplay->drawEllipse(startX + buttonMargin, startY + buttonMargin / 2, buttonRadius, buttonRadius, 1, 1);
	} else {
		gpDisplay->drawEllipse(startX + buttonMargin / 2, startY + buttonMargin / 2, buttonRadius, buttonRadius, 1, 1);
	}
}

void DisplayAddon::drawFightboardMirrored(int startX, int startY, int buttonRadius, int buttonPadding)
{
	const int buttonMargin = buttonPadding + (buttonRadius * 2);
    const int leftMargin = startX + buttonPadding + buttonRadius;

	gpDisplay->drawEllipse(leftMargin, startY - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedL1());
	gpDisplay->drawEllipse(leftMargin + buttonMargin, startY - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedR1());
	gpDisplay->drawEllipse(leftMargin + (buttonMargin*2), startY - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedB4());
	gpDisplay->drawEllipse(leftMargin + (buttonMargin*3), startY * 1.25, buttonRadius, buttonRadius, 1, pGamepad->pressedB3());

	gpDisplay->drawEllipse(leftMargin, startY + buttonMargin - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedL2());
	gpDisplay->drawEllipse(leftMargin + buttonMargin, startY + buttonMargin - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedR2());
	gpDisplay->drawEllipse(leftMargin + (buttonMargin*2), startY + buttonMargin - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedB2());
	gpDisplay->drawEllipse(leftMargin + (buttonMargin*3), startY + buttonMargin * 1.25, buttonRadius, buttonRadius, 1, pGamepad->pressedB1());

    // Extra buttons
    gpDisplay->drawEllipse(startX + buttonMargin * 0.5, startY + (buttonMargin * 1.5), 3, 3, 1, pGamepad->pressedL3());
    gpDisplay->drawEllipse(startX + buttonMargin * 1.0625, startY + (buttonMargin * 1.5), 3, 3, 1, pGamepad->pressedS1());
    gpDisplay->drawEllipse(startX + buttonMargin * 1.625, startY + (buttonMargin * 1.5), 3, 3, 1, pGamepad->pressedA1());
    gpDisplay->drawEllipse(startX + buttonMargin * 2.125+0.0625, startY + (buttonMargin * 1.5), 3, 3, 1, pGamepad->pressedS2());
    gpDisplay->drawEllipse(startX + buttonMargin * 2.75, startY + (buttonMargin * 1.5), 3, 3, 1, pGamepad->pressedR3());
}

void DisplayAddon::drawTwinStickA(int startX, int startY, int buttonRadius, int buttonPadding)
{
	const int buttonMargin = buttonPadding + (buttonRadius * 2);

	// Stick
	gpDisplay->drawEllipse(startX + (buttonMargin / 2), startY + (buttonMargin / 2), buttonRadius * 1.25, buttonRadius * 1.25, 1, 0);

	if (pressedUp()) {
		if (pressedLeft()) {
			gpDisplay->drawEllipse(startX + (buttonMargin / 5), startY + (buttonMargin / 5), buttonRadius, buttonRadius, 1, 1);
		} else if (pressedRight()) {
			gpDisplay->drawEllipse(startX + (buttonMargin * 0.875), startY + (buttonMargin / 5), buttonRadius, buttonRadius, 1, 1);
		} else {
			gpDisplay->drawEllipse(startX + (buttonMargin / 2), startY, buttonRadius, buttonRadius, 1, 1);
		}
	} else if (pressedDown()) {
		if (pressedLeft()) {
			gpDisplay->drawEllipse(startX + (buttonMargin / 5), startY + (buttonMargin * 0.875), buttonRadius, buttonRadius, 1, 1);
		} else if (pressedRight()) {
			gpDisplay->drawEllipse(startX + (buttonMargin * 0.875), startY + (buttonMargin * 0.875), buttonRadius, buttonRadius, 1, 1);
		} else {
			gpDisplay->drawEllipse(startX + buttonMargin / 2, startY + buttonMargin, buttonRadius, buttonRadius, 1, 1);
		}
	} else if (pressedLeft()) {
		gpDisplay->drawEllipse(startX, startY + buttonMargin / 2, buttonRadius, buttonRadius, 1, 1);
	} else if (pressedRight()) {
		gpDisplay->drawEllipse(startX + buttonMargin, startY + buttonMargin / 2, buttonRadius, buttonRadius, 1, 1);
	} else {
		gpDisplay->drawEllipse(startX + buttonMargin / 2, startY + buttonMargin / 2, buttonRadius, buttonRadius, 1, 1);
	}
}

void DisplayAddon::drawTwinStickB(int startX, int startY, int buttonRadius, int buttonPadding)
{
	const int buttonMargin = buttonPadding + (buttonRadius * 2);

	// Stick
	gpDisplay->drawEllipse(startX + (buttonMargin / 2), startY + (buttonMargin / 2), buttonRadius * 1.25, buttonRadius * 1.25, 1, 0);

	if (pGamepad->pressedB4()) {
		if (pGamepad->pressedB3()) {
			gpDisplay->drawEllipse(startX + (buttonMargin / 5), startY + (buttonMargin / 5), buttonRadius, buttonRadius, 1, 1);
		} else if (pGamepad->pressedB2()) {
			gpDisplay->drawEllipse(startX + (buttonMargin * 0.875), startY + (buttonMargin / 5), buttonRadius, buttonRadius, 1, 1);
		} else {
			gpDisplay->drawEllipse(startX + (buttonMargin / 2), startY, buttonRadius, buttonRadius, 1, 1);
		}
	} else if (pGamepad->pressedB1()) {
		if (pGamepad->pressedB3()) {
			gpDisplay->drawEllipse(startX + (buttonMargin / 5), startY + (buttonMargin * 0.875), buttonRadius, buttonRadius, 1, 1);
		} else if (pGamepad->pressedB2()) {
			gpDisplay->drawEllipse(startX + (buttonMargin * 0.875), startY + (buttonMargin * 0.875), buttonRadius, buttonRadius, 1, 1);
		} else {
			gpDisplay->drawEllipse(startX + buttonMargin / 2, startY + buttonMargin, buttonRadius, buttonRadius, 1, 1);
		}
	} else if (pGamepad->pressedB3()) {
		gpDisplay->drawEllipse(startX, startY + buttonMargin / 2, buttonRadius, buttonRadius, 1, 1);
	} else if (pGamepad->pressedB2()) {
		gpDisplay->drawEllipse(startX + buttonMargin, startY + buttonMargin / 2, buttonRadius, buttonRadius, 1, 1);
	} else {
		gpDisplay->drawEllipse(startX + buttonMargin / 2, startY + buttonMargin / 2, buttonRadius, buttonRadius, 1, 1);
	}
}

void DisplayAddon::drawMAMEA(int startX, int startY, int buttonSize, int buttonPadding)
{
	const int buttonMargin = buttonPadding + buttonSize;

	// MAME
	gpDisplay->drawRectangle(startX, startY + buttonMargin, startX + buttonSize, startY + buttonSize + buttonMargin, 1, pressedLeft());
	gpDisplay->drawRectangle(startX + buttonMargin, startY + buttonMargin, startX + buttonSize + buttonMargin, startY + buttonSize + buttonMargin, 1, pressedDown());
	gpDisplay->drawRectangle(startX + buttonMargin, startY, startX + buttonSize + buttonMargin, startY + buttonSize, 1, pressedUp());
	gpDisplay->drawRectangle(startX + buttonMargin * 2, startY + buttonMargin, startX + buttonSize + buttonMargin * 2, startY + buttonSize + buttonMargin, 1, pressedRight());

}

void DisplayAddon::drawOpenCore0WASDA(int startX, int startY, int buttonSize, int buttonPadding)
{
	const int buttonMargin = buttonPadding + buttonSize;

	// Open_Core0 WASD
	gpDisplay->drawRectangle(startX, startY + buttonMargin, startX + buttonSize, startY + buttonSize + buttonMargin, 1, pressedLeft());
	gpDisplay->drawRectangle(startX + buttonMargin, startY + buttonMargin, startX + buttonSize + buttonMargin, startY + buttonSize + buttonMargin, 1, pressedDown());
	gpDisplay->drawRectangle(startX + buttonMargin, startY, startX + buttonSize + buttonMargin, startY + buttonSize, 1, pressedUp());
	gpDisplay->drawRectangle(startX + buttonMargin * 2, startY + buttonMargin, startX + buttonSize + buttonMargin * 2, startY + buttonSize + buttonMargin, 1, pressedRight());

	// Aux buttons
    gpDisplay->drawEllipse(startX - 15 + buttonMargin * 0.5, startY - 25 + (buttonMargin * 1.5), 3, 3, 1, pGamepad->pressedS2());
    gpDisplay->drawEllipse(startX - 15 + buttonMargin * 1.25, startY - 25 + (buttonMargin * 1.5), 3, 3, 1, pGamepad->pressedS1());
    gpDisplay->drawEllipse(startX - 15 + buttonMargin * 2, startY -25 + (buttonMargin * 1.5), 3, 3, 1, pGamepad->pressedA1());
    gpDisplay->drawEllipse(startX - 15 + buttonMargin * 2.75, startY -25 + (buttonMargin * 1.5), 3, 3, 1, pGamepad->pressedA2());
    gpDisplay->drawEllipse(startX - 15 + buttonMargin * 3.5, startY -25 + (buttonMargin * 1.5), 3, 3, 1, pGamepad->pressedL3());
	gpDisplay->drawEllipse(startX - 15 + buttonMargin * 4.25, startY -25 + (buttonMargin * 1.5), 3, 3, 1, pGamepad->pressedR3());

}

void DisplayAddon::drawMAMEB(int startX, int startY, int buttonSize, int buttonPadding)
{
	const int buttonMargin = buttonPadding + buttonSize;

	// 6-button MAME Style
	gpDisplay->drawRectangle(startX, startY, startX + buttonSize, startY + buttonSize, 1, pGamepad->pressedB3());
	gpDisplay->drawRectangle(startX + buttonMargin, startY, startX + buttonSize + buttonMargin, startY + buttonSize, 1, pGamepad->pressedB4());
	gpDisplay->drawRectangle(startX + buttonMargin * 2, startY, startX + buttonSize + buttonMargin * 2, startY + buttonSize, 1, pGamepad->pressedR1());

	gpDisplay->drawRectangle(startX, startY + buttonMargin, startX + buttonSize, startY + buttonMargin + buttonSize, 1, pGamepad->pressedB1());
	gpDisplay->drawRectangle(startX + buttonMargin, startY + buttonMargin, startX + buttonSize + buttonMargin, startY + buttonMargin + buttonSize, 1, pGamepad->pressedB2());
	gpDisplay->drawRectangle(startX + buttonMargin * 2, startY + buttonMargin, startX + buttonSize + buttonMargin * 2, startY + buttonMargin + buttonSize, 1, pGamepad->pressedR2());

}

void DisplayAddon::drawMAME8B(int startX, int startY, int buttonSize, int buttonPadding)
{
	const int buttonMargin = buttonPadding + buttonSize;

	// 8-button MAME Style
	gpDisplay->drawRectangle(startX, startY, startX + buttonSize, startY + buttonSize, 1, pGamepad->pressedB3());
	gpDisplay->drawRectangle(startX + buttonMargin, startY - (buttonMargin / 3), startX + buttonSize + buttonMargin, startY - (buttonMargin / 3) + buttonSize, 1, pGamepad->pressedB4());
	gpDisplay->drawRectangle(startX + buttonMargin * 2, startY - (buttonMargin / 3), startX + buttonSize + buttonMargin * 2, startY - (buttonMargin / 3)+ buttonSize, 1, pGamepad->pressedR1());
	gpDisplay->drawRectangle(startX + buttonMargin * 3, startY, startX + buttonSize + buttonMargin * 3, startY + buttonSize, 1, pGamepad->pressedL1());

	gpDisplay->drawRectangle(startX, startY + buttonMargin, startX + buttonSize, startY + buttonMargin + buttonSize, 1, pGamepad->pressedB1());
	gpDisplay->drawRectangle(startX + buttonMargin, startY - (buttonMargin / 3) + buttonMargin, startX + buttonSize + buttonMargin, startY - (buttonMargin / 3) + buttonMargin + buttonSize, 1, pGamepad->pressedB2());
	gpDisplay->drawRectangle(startX + buttonMargin * 2, startY - (buttonMargin / 3) + buttonMargin, startX + buttonSize + buttonMargin * 2, startY - (buttonMargin / 3) + buttonMargin + buttonSize, 1, pGamepad->pressedR2());
	gpDisplay->drawRectangle(startX + buttonMargin * 3, startY + buttonMargin, startX + buttonSize + buttonMargin * 3, startY + buttonMargin + buttonSize, 1, pGamepad->pressedL2());

}

void DisplayAddon::drawOpenCore0WASDB(int startX, int startY, int buttonSize, int buttonPadding)
{
	const int buttonMargin = buttonPadding + buttonSize;

	// 8-button Open_Core0 WASD
	gpDisplay->drawRectangle(startX, startY, startX + buttonSize, startY + buttonSize, 1, pGamepad->pressedB3());
	gpDisplay->drawRectangle(startX + buttonMargin, startY - (buttonMargin / 3), startX + buttonSize + buttonMargin, startY - (buttonMargin / 3) + buttonSize, 1, pGamepad->pressedB4());
	gpDisplay->drawRectangle(startX + buttonMargin * 2, startY - (buttonMargin / 3), startX + buttonSize + buttonMargin * 2, startY - (buttonMargin / 3)+ buttonSize, 1, pGamepad->pressedR1());
	gpDisplay->drawRectangle(startX + buttonMargin * 3, startY, startX + buttonSize + buttonMargin * 3, startY + buttonSize, 1, pGamepad->pressedL1());

	gpDisplay->drawRectangle(startX, startY + buttonMargin, startX + buttonSize, startY + buttonMargin + buttonSize, 1, pGamepad->pressedB1());
	gpDisplay->drawRectangle(startX + buttonMargin, startY - (buttonMargin / 3) + buttonMargin, startX + buttonSize + buttonMargin, startY - (buttonMargin / 3) + buttonMargin + buttonSize, 1, pGamepad->pressedB2());
	gpDisplay->drawRectangle(startX + buttonMargin * 2, startY - (buttonMargin / 3) + buttonMargin, startX + buttonSize + buttonMargin * 2, startY - (buttonMargin / 3) + buttonMargin + buttonSize, 1, pGamepad->pressedR2());
	gpDisplay->drawRectangle(startX + buttonMargin * 3, startY + buttonMargin, startX + buttonSize + buttonMargin * 3, startY + buttonMargin + buttonSize, 1, pGamepad->pressedL2());

}

void DisplayAddon::drawKeyboardAngled(int startX, int startY, int buttonRadius, int buttonPadding)
{
	const int buttonMargin = buttonPadding + (buttonRadius * 2);

	// MixBox
	drawDiamond(startX, startY, buttonRadius, 1, pressedLeft());
	drawDiamond(startX + buttonMargin / 2, startY + buttonMargin / 2, buttonRadius, 1, pressedDown());
	drawDiamond(startX + buttonMargin, startY, buttonRadius, 1, pressedUp());
	drawDiamond(startX + buttonMargin, startY + buttonMargin, buttonRadius, 1, pressedRight());
}

void DisplayAddon::drawVewlix(int startX, int startY, int buttonRadius, int buttonPadding)
{
	const int buttonMargin = buttonPadding + (buttonRadius * 2);

	// 8-button Vewlix
	gpDisplay->drawEllipse(startX + (buttonMargin * 2.75), startY + (buttonMargin * 0.2), buttonRadius, buttonRadius, 1, pGamepad->pressedB3());
	gpDisplay->drawEllipse(startX + (buttonMargin * 3.75), startY - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedB4());
	gpDisplay->drawEllipse(startX + (buttonMargin * 4.75), startY - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedR1());
	gpDisplay->drawEllipse(startX + (buttonMargin * 5.75), startY - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedL1());

	gpDisplay->drawEllipse(startX + (buttonMargin * 2.75) - (buttonMargin / 3), startY + buttonMargin + (buttonMargin * 0.2), buttonRadius, buttonRadius, 1, pGamepad->pressedB1());
	gpDisplay->drawEllipse(startX + (buttonMargin * 3.75) - (buttonMargin / 3), startY + buttonMargin - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedB2());
	gpDisplay->drawEllipse(startX + (buttonMargin * 4.75) - (buttonMargin / 3), startY + buttonMargin - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedR2());
	gpDisplay->drawEllipse(startX + (buttonMargin * 5.75) - (buttonMargin / 3), startY + buttonMargin - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedL2());
}

void DisplayAddon::drawVLXB(int startX, int startY, int buttonRadius, int buttonPadding)
{
	const int buttonMargin = buttonPadding + (buttonRadius * 2);

	// 9-button Hori VLX
	gpDisplay->drawEllipse(startX + (buttonMargin * 2.75), startY + (buttonMargin * 0.2), buttonRadius, buttonRadius, 1, pGamepad->pressedB3());
	gpDisplay->drawEllipse(startX + (buttonMargin * 3.75), startY - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedB4());
	gpDisplay->drawEllipse(startX + (buttonMargin * 4.75), startY - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedR1());
	gpDisplay->drawEllipse(startX + (buttonMargin * 5.75), startY - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedL1());

	gpDisplay->drawEllipse(startX + (buttonMargin * 2.75) - (buttonMargin / 3), startY + buttonMargin + (buttonMargin * 0.2), buttonRadius, buttonRadius, 1, pGamepad->pressedB1());
	gpDisplay->drawEllipse(startX + (buttonMargin * 3.75) - (buttonMargin / 3), startY + buttonMargin - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedB2());
	gpDisplay->drawEllipse(startX + (buttonMargin * 4.75) - (buttonMargin / 3), startY + buttonMargin - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedR2());
	gpDisplay->drawEllipse(startX + (buttonMargin * 5.75) - (buttonMargin / 3), startY + buttonMargin - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedL2());

	gpDisplay->drawEllipse(startX + (buttonMargin * 7.4) - (buttonMargin / 3.5), startY + buttonMargin - (buttonMargin / 1.5), buttonRadius *.8, buttonRadius * .8, 1, pGamepad->pressedS2());
}

void DisplayAddon::drawFightboard(int startX, int startY, int buttonRadius, int buttonPadding)
{
	const int buttonMargin = buttonPadding + (buttonRadius * 2);

	gpDisplay->drawEllipse((startX + buttonMargin * 3.625), startY * 1.25, buttonRadius, buttonRadius, 1, pGamepad->pressedB3());
	gpDisplay->drawEllipse((startX + buttonMargin * 4.625), startY - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedB4());
	gpDisplay->drawEllipse((startX + buttonMargin * 5.625), startY - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedR1());
	gpDisplay->drawEllipse((startX + buttonMargin * 6.625), startY - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedL1());

	gpDisplay->drawEllipse((startX + buttonMargin * 3.625), startY + buttonMargin * 1.25, buttonRadius, buttonRadius, 1, pGamepad->pressedB1());
	gpDisplay->drawEllipse((startX + buttonMargin * 4.625), startY + buttonMargin - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedB2());
	gpDisplay->drawEllipse((startX + buttonMargin * 5.625), startY + buttonMargin - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedR2());
	gpDisplay->drawEllipse((startX + buttonMargin * 6.625), startY + buttonMargin - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedL2());

    // Extra buttons
	gpDisplay->drawEllipse(startX + buttonMargin * 4.5, startY + (buttonMargin * 1.5), 3, 3, 1, pGamepad->pressedL3());
	gpDisplay->drawEllipse(startX + buttonMargin * 5.0625, startY + (buttonMargin * 1.5), 3, 3, 1, pGamepad->pressedS1());
	gpDisplay->drawEllipse(startX + buttonMargin * 5.625, startY + (buttonMargin * 1.5), 3, 3, 1, pGamepad->pressedA1());
	gpDisplay->drawEllipse(startX + buttonMargin * 6.125+0.0625, startY + (buttonMargin * 1.5), 3, 3, 1, pGamepad->pressedS2());
	gpDisplay->drawEllipse(startX + buttonMargin * 6.75, startY + (buttonMargin * 1.5), 3, 3, 1, pGamepad->pressedR3());
}

void DisplayAddon::drawVewlix7(int startX, int startY, int buttonRadius, int buttonPadding)
{
	const int buttonMargin = buttonPadding + (buttonRadius * 2);

	// 8-button Vewlix
	gpDisplay->drawEllipse(startX + (buttonMargin * 2.75), startY + (buttonMargin * 0.2), buttonRadius, buttonRadius, 1, pGamepad->pressedB3());
	gpDisplay->drawEllipse(startX + (buttonMargin * 3.75), startY - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedB4());
	gpDisplay->drawEllipse(startX + (buttonMargin * 4.75), startY - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedR1());
	gpDisplay->drawEllipse(startX + (buttonMargin * 5.75), startY - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedL1());

	gpDisplay->drawEllipse(startX + (buttonMargin * 2.75) - (buttonMargin / 3), startY + buttonMargin + (buttonMargin * 0.2), buttonRadius, buttonRadius, 1, pGamepad->pressedB1());
	gpDisplay->drawEllipse(startX + (buttonMargin * 3.75) - (buttonMargin / 3), startY + buttonMargin - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedB2());
	gpDisplay->drawEllipse(startX + (buttonMargin * 4.75) - (buttonMargin / 3), startY + buttonMargin - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedR2());
	//gpDisplay->drawEllipse(startX + (buttonMargin * 5.75) - (buttonMargin / 3), startY + buttonMargin - (buttonMargin / 4), buttonRadius, buttonRadius, 1, gamepad->pressedL2());
}

void DisplayAddon::drawSega2p(int startX, int startY, int buttonRadius, int buttonPadding)
{
	const int buttonMargin = buttonPadding + (buttonRadius * 2);

	// 8-button Sega2P
	gpDisplay->drawEllipse(startX + (buttonMargin * 2.75), startY + (buttonMargin / 3), buttonRadius, buttonRadius, 1, pGamepad->pressedB3());
	gpDisplay->drawEllipse(startX + (buttonMargin * 3.75), startY - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedB4());
	gpDisplay->drawEllipse(startX + (buttonMargin * 4.75), startY - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedR1());
	gpDisplay->drawEllipse(startX + (buttonMargin * 5.75), startY, buttonRadius, buttonRadius, 1, pGamepad->pressedL1());

	gpDisplay->drawEllipse(startX + (buttonMargin * 2.75), startY + buttonMargin + (buttonMargin / 3), buttonRadius, buttonRadius, 1, pGamepad->pressedB1());
	gpDisplay->drawEllipse(startX + (buttonMargin * 3.75), startY + buttonMargin - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedB2());
	gpDisplay->drawEllipse(startX + (buttonMargin * 4.75), startY + buttonMargin - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedR2());
	gpDisplay->drawEllipse(startX + (buttonMargin * 5.75), startY + buttonMargin, buttonRadius, buttonRadius, 1, pGamepad->pressedL2());
}

void DisplayAddon::drawNoir8(int startX, int startY, int buttonRadius, int buttonPadding)
{
	const int buttonMargin = buttonPadding + (buttonRadius * 2);

	// 8-button Noir8
	gpDisplay->drawEllipse(startX + (buttonMargin * 2.75), startY + (buttonMargin / 3.5), buttonRadius, buttonRadius, 1, pGamepad->pressedB3());
	gpDisplay->drawEllipse(startX + (buttonMargin * 3.75), startY - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedB4());
	gpDisplay->drawEllipse(startX + (buttonMargin * 4.75), startY - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedR1());
	gpDisplay->drawEllipse(startX + (buttonMargin * 5.75), startY, buttonRadius, buttonRadius, 1, pGamepad->pressedL1());

	gpDisplay->drawEllipse(startX + (buttonMargin * 2.75), startY + buttonMargin + (buttonMargin / 3.5), buttonRadius, buttonRadius, 1, pGamepad->pressedB1());
	gpDisplay->drawEllipse(startX + (buttonMargin * 3.75), startY + buttonMargin - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedB2());
	gpDisplay->drawEllipse(startX + (buttonMargin * 4.75), startY + buttonMargin - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedR2());
	gpDisplay->drawEllipse(startX + (buttonMargin * 5.75), startY + buttonMargin, buttonRadius, buttonRadius, 1, pGamepad->pressedL2());
}

void DisplayAddon::drawCapcom(int startX, int startY, int buttonRadius, int buttonPadding)
{
	const int buttonMargin = buttonPadding + (buttonRadius * 2);

	// 8-button Capcom
	gpDisplay->drawEllipse(startX + buttonMargin * 3.25, startY, buttonRadius, buttonRadius, 1, pGamepad->pressedB3());
	gpDisplay->drawEllipse(startX + buttonMargin * 4.25, startY, buttonRadius, buttonRadius, 1, pGamepad->pressedB4());
	gpDisplay->drawEllipse(startX + buttonMargin * 5.25, startY, buttonRadius, buttonRadius, 1, pGamepad->pressedR1());
	gpDisplay->drawEllipse(startX + buttonMargin * 6.25, startY, buttonRadius, buttonRadius, 1, pGamepad->pressedL1());

	gpDisplay->drawEllipse(startX + buttonMargin * 3.25, startY + buttonMargin, buttonRadius, buttonRadius, 1, pGamepad->pressedB1());
	gpDisplay->drawEllipse(startX + buttonMargin * 4.25, startY + buttonMargin, buttonRadius, buttonRadius, 1, pGamepad->pressedB2());
	gpDisplay->drawEllipse(startX + buttonMargin * 5.25, startY + buttonMargin, buttonRadius, buttonRadius, 1, pGamepad->pressedR2());
	gpDisplay->drawEllipse(startX + buttonMargin * 6.25, startY + buttonMargin, buttonRadius, buttonRadius, 1, pGamepad->pressedL2());
}

void DisplayAddon::drawCapcom6(int startX, int startY, int buttonRadius, int buttonPadding)
{
	const int buttonMargin = buttonPadding + (buttonRadius * 2);

	// 6-button Capcom
	gpDisplay->drawEllipse(startX + buttonMargin * 3.25, startY, buttonRadius, buttonRadius, 1, pGamepad->pressedB3());
	gpDisplay->drawEllipse(startX + buttonMargin * 4.25, startY, buttonRadius, buttonRadius, 1, pGamepad->pressedB4());
	gpDisplay->drawEllipse(startX + buttonMargin * 5.25, startY, buttonRadius, buttonRadius, 1, pGamepad->pressedR1());

	gpDisplay->drawEllipse(startX + buttonMargin * 3.25, startY + buttonMargin, buttonRadius, buttonRadius, 1, pGamepad->pressedB1());
	gpDisplay->drawEllipse(startX + buttonMargin * 4.25, startY + buttonMargin, buttonRadius, buttonRadius, 1, pGamepad->pressedB2());
	gpDisplay->drawEllipse(startX + buttonMargin * 5.25, startY + buttonMargin, buttonRadius, buttonRadius, 1, pGamepad->pressedR2());
}

void DisplayAddon::drawSticklessButtons(int startX, int startY, int buttonRadius, int buttonPadding)
{
	const int buttonMargin = buttonPadding + (buttonRadius * 2);

	// 8-button
	gpDisplay->drawEllipse(startX + (buttonMargin * 2.75), startY, buttonRadius, buttonRadius, 1, pGamepad->pressedB3());
	gpDisplay->drawEllipse(startX + (buttonMargin * 3.75), startY - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedB4());
	gpDisplay->drawEllipse(startX + (buttonMargin * 4.75), startY - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedR1());
	gpDisplay->drawEllipse(startX + (buttonMargin * 5.75), startY, buttonRadius, buttonRadius, 1, pGamepad->pressedL1());

	gpDisplay->drawEllipse(startX + (buttonMargin * 2.75), startY + buttonMargin, buttonRadius, buttonRadius, 1, pGamepad->pressedB1());
	gpDisplay->drawEllipse(startX + (buttonMargin * 3.75), startY + buttonMargin - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedB2());
	gpDisplay->drawEllipse(startX + (buttonMargin * 4.75), startY + buttonMargin - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedR2());
	gpDisplay->drawEllipse(startX + (buttonMargin * 5.75), startY + buttonMargin, buttonRadius, buttonRadius, 1, pGamepad->pressedL2());
}

void DisplayAddon::drawWasdButtons(int startX, int startY, int buttonRadius, int buttonPadding)
{
	const int buttonMargin = buttonPadding + (buttonRadius * 2);

	// 8-button
	gpDisplay->drawEllipse(startX + buttonMargin * 3.625, startY, buttonRadius, buttonRadius, 1, pGamepad->pressedB3());
	gpDisplay->drawEllipse(startX + buttonMargin * 4.625, startY - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedB4());
	gpDisplay->drawEllipse(startX + buttonMargin * 5.625, startY - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedR1());
	gpDisplay->drawEllipse(startX + buttonMargin * 6.625, startY, buttonRadius, buttonRadius, 1, pGamepad->pressedL1());

	gpDisplay->drawEllipse(startX + buttonMargin * 3.25, startY + buttonMargin, buttonRadius, buttonRadius, 1, pGamepad->pressedB1());
	gpDisplay->drawEllipse(startX + buttonMargin * 4.25, startY + buttonMargin - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedB2());
	gpDisplay->drawEllipse(startX + buttonMargin * 5.25, startY + buttonMargin - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedR2());
	gpDisplay->drawEllipse(startX + buttonMargin * 6.25, startY + buttonMargin, buttonRadius, buttonRadius, 1, pGamepad->pressedL2());
}

void DisplayAddon::drawArcadeButtons(int startX, int startY, int buttonRadius, int buttonPadding)
{
	const int buttonMargin = buttonPadding + (buttonRadius * 2);

	// 8-button
	gpDisplay->drawEllipse(startX + buttonMargin * 3.125, startY, buttonRadius, buttonRadius, 1, pGamepad->pressedB3());
	gpDisplay->drawEllipse(startX + buttonMargin * 4.125, startY - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedB4());
	gpDisplay->drawEllipse(startX + buttonMargin * 5.125, startY - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedR1());
	gpDisplay->drawEllipse(startX + buttonMargin * 6.125, startY, buttonRadius, buttonRadius, 1, pGamepad->pressedL1());

	gpDisplay->drawEllipse(startX + buttonMargin * 2.875, startY + buttonMargin, buttonRadius, buttonRadius, 1, pGamepad->pressedB1());
	gpDisplay->drawEllipse(startX + buttonMargin * 3.875, startY + buttonMargin - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedB2());
	gpDisplay->drawEllipse(startX + buttonMargin * 4.875, startY + buttonMargin - (buttonMargin / 4), buttonRadius, buttonRadius, 1, pGamepad->pressedR2());
	gpDisplay->drawEllipse(startX + buttonMargin * 5.875, startY + buttonMargin, buttonRadius, buttonRadius, 1, pGamepad->pressedL2());
}

// I pulled this out of my PR, brought it back because of recent talks re: SOCD and rhythm games
// Enjoy!

void DisplayAddon::drawDancepadA(int startX, int startY, int buttonSize, int buttonPadding)
{
	const int buttonMargin = buttonPadding + buttonSize;

	gpDisplay->drawRectangle(startX, startY + buttonMargin, startX + buttonSize, startY + buttonSize + buttonMargin, 1, pressedLeft());
	gpDisplay->drawRectangle(startX + buttonMargin, startY + buttonMargin * 2, startX + buttonSize + buttonMargin, startY + buttonSize + buttonMargin * 2, 1, pressedDown());
	gpDisplay->drawRectangle(startX + buttonMargin, startY, startX + buttonSize + buttonMargin, startY + buttonSize, 1, pressedUp());
	gpDisplay->drawRectangle(startX + buttonMargin * 2, startY + buttonMargin, startX + buttonSize + buttonMargin * 2, startY + buttonSize + buttonMargin, 1, pressedRight());
}

void DisplayAddon::drawDancepadB(int startX, int startY, int buttonSize, int buttonPadding)
{
	const int buttonMargin = buttonPadding + buttonSize;

	gpDisplay->drawRectangle(startX, startY, startX + buttonSize, startY + buttonSize, 1, pGamepad->pressedB2()); // Up/Left
	gpDisplay->drawRectangle(startX, startY + buttonMargin * 2, startX + buttonSize, startY + buttonSize + buttonMargin * 2, 1, pGamepad->pressedB4()); // Down/Left
	gpDisplay->drawRectangle(startX + buttonMargin * 2, startY, startX + buttonSize + buttonMargin * 2, startY + buttonSize, 1, pGamepad->pressedB1()); // Up/Right
	gpDisplay->drawRectangle(startX + buttonMargin * 2, startY + buttonMargin * 2, startX + buttonSize + buttonMargin * 2, startY + buttonSize + buttonMargin * 2, 1, pGamepad->pressedB3()); // Down/Right
}

void DisplayAddon::drawBlankA(int startX, int startY, int buttonSize, int buttonPadding)
{
}

void DisplayAddon::drawBlankB(int startX, int startY, int buttonSize, int buttonPadding)
{
}

void DisplayAddon::drawSplashScreen(int splashMode, uint8_t * splashChoice, int splashSpeed)
{
    int mils = getMillis();
    switch (splashMode)
	{
		case SPLASH_MODE_STATIC: // Default, display static or custom image
			gpDisplay->drawSprite(splashChoice, 128, 64, 16, 0, 0, 1);
			break;
		case SPLASH_MODE_CLOSEIN: // Close-in. Animate the GP2040 logo
			gpDisplay->drawSprite((uint8_t *)bootLogoTop, 43, 39, 6, 43, std::min<int>((mils / splashSpeed) - 39, 0), 1);
			gpDisplay->drawSprite((uint8_t *)bootLogoBottom, 80, 21, 10, 24, std::max<int>(64 - (mils / (splashSpeed * 2)), 44), 1);
			break;
        case SPLASH_MODE_CLOSEINCUSTOM: // Close-in on custom image or delayed close-in if custom image does not exist
            gpDisplay->drawSprite(splashChoice, 128, 64, 16, 0, 0, 1);
            if (mils > 2500) {
                int milss = mils - 2500;
                gpDisplay->drawRectangle(0, 0, 127, 1 + (milss / splashSpeed), 0, 1);
                gpDisplay->drawRectangle(0, 63, 127, 62 - (milss / (splashSpeed * 2)), 0, 1);
                gpDisplay->drawSprite((uint8_t *)bootLogoTop, 43, 39, 6, 43, std::min<int>((milss / splashSpeed) - 39, 0), 1);
                gpDisplay->drawSprite((uint8_t *)bootLogoBottom, 80, 21, 10, 24, std::max<int>(64 - (milss / (splashSpeed * 2)), 44), 1);
            }
            break;
	}
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
	drawText(0, 0, statusBar);
}

bool DisplayAddon::pressedUp()
{
	switch (gamepad->getOptions().dpadMode)
	{
		case DPAD_MODE_DIGITAL:      return pGamepad->pressedUp();
		case DPAD_MODE_LEFT_ANALOG:  return pGamepad->state.ly == GAMEPAD_JOYSTICK_MIN;
		case DPAD_MODE_RIGHT_ANALOG: return pGamepad->state.ry == GAMEPAD_JOYSTICK_MIN;
	}

	return false;
}

bool DisplayAddon::pressedDown()
{
	switch (gamepad->getOptions().dpadMode)
	{
		case DPAD_MODE_DIGITAL:      return pGamepad->pressedDown();
		case DPAD_MODE_LEFT_ANALOG:  return pGamepad->state.ly == GAMEPAD_JOYSTICK_MAX;
		case DPAD_MODE_RIGHT_ANALOG: return pGamepad->state.ry == GAMEPAD_JOYSTICK_MAX;
	}

	return false;
}

bool DisplayAddon::pressedLeft()
{
	switch (gamepad->getOptions().dpadMode)
	{
		case DPAD_MODE_DIGITAL:      return pGamepad->pressedLeft();
		case DPAD_MODE_LEFT_ANALOG:  return pGamepad->state.lx == GAMEPAD_JOYSTICK_MIN;
		case DPAD_MODE_RIGHT_ANALOG: return pGamepad->state.rx == GAMEPAD_JOYSTICK_MIN;
	}

	return false;
}

bool DisplayAddon::pressedRight()
{
	switch (gamepad->getOptions().dpadMode)
	{
		case DPAD_MODE_DIGITAL:      return pGamepad->pressedRight();
		case DPAD_MODE_LEFT_ANALOG:  return pGamepad->state.lx == GAMEPAD_JOYSTICK_MAX;
		case DPAD_MODE_RIGHT_ANALOG: return pGamepad->state.rx == GAMEPAD_JOYSTICK_MAX;
	}

	return false;
}

void DisplayAddon::attachInputHistoryAddon(InputHistoryAddon* pInputHistoryAddon) {
	inputHistoryAddon = pInputHistoryAddon;
}
