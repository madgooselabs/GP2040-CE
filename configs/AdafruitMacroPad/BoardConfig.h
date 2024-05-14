/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: Copyright (c) 2024 OpenStickCommunity (gp2040-ce.info)
 * SPDX-FileCopyrightText: Copyright (c) 2023 Robert Dale Smith (controlleradapter.com)
 */

#ifndef MACROPAD_BOARD_CONFIG_H_
#define MACROPAD_BOARD_CONFIG_H_

#include "enums.pb.h"
#include "class/hid/hid.h"

#define BOARD_CONFIG_LABEL "Adafruit MacroPad RP2040"

// Main pin mapping Configuration
//                                                  // GP2040 | Xinput | Switch  | PS3/4/5  | Dinput | Arcade |
#define GPIO_PIN_00 GpioAction::BUTTON_PRESS_A1     // A1     | Guide  | Home    | PS       | 13     | ~      |
#define GPIO_PIN_01 GpioAction::BUTTON_PRESS_B1     // B1     | A      | B       | Cross    | 2      | K1     |
#define GPIO_PIN_02 GpioAction::BUTTON_PRESS_UP     // UP     | UP     | UP      | UP       | UP     | UP     |
#define GPIO_PIN_03 GpioAction::BUTTON_PRESS_B2     // B2     | B      | A       | Circle   | 3      | K2     |
#define GPIO_PIN_04 GpioAction::BUTTON_PRESS_LEFT   // LEFT   | LEFT   | LEFT    | LEFT     | LEFT   | LEFT   |
#define GPIO_PIN_05 GpioAction::BUTTON_PRESS_S2     // S2     | Start  | Plus    | Start    | 10     | Start  |
#define GPIO_PIN_06 GpioAction::BUTTON_PRESS_RIGHT  // RIGHT  | RIGHT  | RIGHT   | RIGHT    | RIGHT  | RIGHT  |
#define GPIO_PIN_07 GpioAction::BUTTON_PRESS_B3     // B3     | X      | Y       | Square   | 1      | P1     |
#define GPIO_PIN_08 GpioAction::BUTTON_PRESS_DOWN   // DOWN   | DOWN   | DOWN    | DOWN     | DOWN   | DOWN   |
#define GPIO_PIN_09 GpioAction::BUTTON_PRESS_B4     // B4     | Y      | X       | Triangle | 4      | P2     |
#define GPIO_PIN_10 GpioAction::BUTTON_PRESS_L1     // L1     | LB     | L       | L1       | 5      | P4     |
#define GPIO_PIN_11 GpioAction::BUTTON_PRESS_S1     // S1     | Back   | Minus   | Select   | 9      | Coin   |
#define GPIO_PIN_12 GpioAction::BUTTON_PRESS_R1     // R1     | RB     | R       | R1       | 6      | P3     |

// Keyboard Mapping Configuration
//                                            // GP2040 | Xinput | Switch  | PS3/4/5  | Dinput | Arcade |
#define KEY_DPAD_UP     HID_KEY_ARROW_UP      // UP     | UP     | UP      | UP       | UP     | UP     |
#define KEY_DPAD_DOWN   HID_KEY_ARROW_DOWN    // DOWN   | DOWN   | DOWN    | DOWN     | DOWN   | DOWN   |
#define KEY_DPAD_RIGHT  HID_KEY_ARROW_RIGHT   // RIGHT  | RIGHT  | RIGHT   | RIGHT    | RIGHT  | RIGHT  |
#define KEY_DPAD_LEFT   HID_KEY_ARROW_LEFT    // LEFT   | LEFT   | LEFT    | LEFT     | LEFT   | LEFT   |
#define KEY_BUTTON_B1   HID_KEY_SHIFT_LEFT    // B1     | A      | B       | Cross    | 2      | K1     |
#define KEY_BUTTON_B2   HID_KEY_Z             // B2     | B      | A       | Circle   | 3      | K2     |
#define KEY_BUTTON_R2   HID_KEY_X             // R2     | RT     | ZR      | R2       | 8      | K3     |
#define KEY_BUTTON_L2   HID_KEY_V             // L2     | LT     | ZL      | L2       | 7      | K4     |
#define KEY_BUTTON_B3   HID_KEY_CONTROL_LEFT  // B3     | X      | Y       | Square   | 1      | P1     |
#define KEY_BUTTON_B4   HID_KEY_ALT_LEFT      // B4     | Y      | X       | Triangle | 4      | P2     |
#define KEY_BUTTON_R1   HID_KEY_SPACE         // R1     | RB     | R       | R1       | 6      | P3     |
#define KEY_BUTTON_L1   HID_KEY_C             // L1     | LB     | L       | L1       | 5      | P4     |
#define KEY_BUTTON_S1   HID_KEY_5             // S1     | Back   | Minus   | Select   | 9      | Coin   |
#define KEY_BUTTON_S2   HID_KEY_1             // S2     | Start  | Plus    | Start    | 10     | Start  |
#define KEY_BUTTON_L3   HID_KEY_EQUAL         // L3     | LS     | LS      | L3       | 11     | LS     |
#define KEY_BUTTON_R3   HID_KEY_MINUS         // R3     | RS     | RS      | R3       | 12     | RS     |
#define KEY_BUTTON_A1   HID_KEY_9             // A1     | Guide  | Home    | PS       | 13     | ~      |
#define KEY_BUTTON_A2   HID_KEY_F2            // A2     | ~      | Capture | ~        | 14     | ~      |
#define KEY_BUTTON_FN   -1                    // Hotkey Function                                        |

#define BOARD_LEDS_PIN 19
#define LED_BRIGHTNESS_MAXIMUM 100
#define LED_BRIGHTNESS_STEPS 5
#define LED_FORMAT LED_FORMAT_GRB
#define LEDS_PER_PIXEL 1
#define LEDS_BASE_ANIMATION_INDEX 1
#define LEDS_BUTTON_B1   0
#define LEDS_DPAD_UP     1
#define LEDS_BUTTON_B2   2
#define LEDS_DPAD_LEFT   3
#define LEDS_BUTTON_S2   4
#define LEDS_DPAD_RIGHT  5
#define LEDS_BUTTON_B3   6
#define LEDS_DPAD_DOWN   7
#define LEDS_BUTTON_B4   8
#define LEDS_BUTTON_L1   9
#define LEDS_BUTTON_S1   10
#define LEDS_BUTTON_R1   11

// No I2C Display available - uses SPI
#define HAS_I2C_DISPLAY 1
#define BUTTON_LAYOUT BUTTON_LAYOUT_BLANKA
#define BUTTON_LAYOUT_RIGHT BUTTON_LAYOUT_BLANKB
#define SPLASH_MODE SPLASH_MODE_STATIC
#define SPLASH_DURATION 3000

// I2C Peripheral Enabled for General Use
#define I2C0_ENABLED 1
#define I2C0_PIN_SDA 20
#define I2C0_PIN_SCL 21

// SPI Peripheral Enabled for Display
#define SPI1_ENABLED 1
#define SPI1_PIN_RX 28
#define SPI1_PIN_SCK 26
#define SPI1_PIN_TX 27

#define DISPLAY_SPI_CS 22
#define DISPLAY_SPI_RESET 23
#define DISPLAY_SPI_DC 24

#define BOARD_LED_ENABLED 1
#define BOARD_LED_PIN 13
#define BOARD_LED_TYPE ON_BOARD_LED_MODE_MODE_INDICATOR

#define BUZZER_ENABLED 1
#define BUZZER_PIN 16
#define BUZZER_ENABLE_PIN 14

#define ROTARY_ENCODER_ENABLED 1
#define ENCODER_ONE_ENABLED 1
#define ENCODER_ONE_PIN_A 17
#define ENCODER_ONE_PIN_B 18
#define ENCODER_ONE_MODE ENCODER_MODE_LEFT_ANALOG_X

#endif
