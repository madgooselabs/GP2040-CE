// These could theoretically be created from enums.proto
export const BUTTON_ACTIONS = {
	NONE: -10,
	RESERVED: -5,
	ASSIGNED_TO_ADDON: 0,
	BUTTON_PRESS_UP: 1,
	BUTTON_PRESS_DOWN: 2,
	BUTTON_PRESS_LEFT: 3,
	BUTTON_PRESS_RIGHT: 4,
	BUTTON_PRESS_B1: 5,
	BUTTON_PRESS_B2: 6,
	BUTTON_PRESS_B3: 7,
	BUTTON_PRESS_B4: 8,
	BUTTON_PRESS_L1: 9,
	BUTTON_PRESS_R1: 10,
	BUTTON_PRESS_L2: 11,
	BUTTON_PRESS_R2: 12,
	BUTTON_PRESS_S1: 13,
	BUTTON_PRESS_S2: 14,
	BUTTON_PRESS_A1: 15,
	BUTTON_PRESS_A2: 16,
	BUTTON_PRESS_L3: 17,
	BUTTON_PRESS_R3: 18,
	BUTTON_PRESS_FN: 19,
	BUTTON_PRESS_DDI_UP: 20,
	BUTTON_PRESS_DDI_DOWN: 21,
	BUTTON_PRESS_DDI_LEFT: 22,
	BUTTON_PRESS_DDI_RIGHT: 23,
	SUSTAIN_DP_MODE_DP: 24,
	SUSTAIN_DP_MODE_LS: 25,
	SUSTAIN_DP_MODE_RS: 26,
	SUSTAIN_SOCD_MODE_UP_PRIO: 27,
	SUSTAIN_SOCD_MODE_NEUTRAL: 28,
	SUSTAIN_SOCD_MODE_SECOND_WIN: 29,
	SUSTAIN_SOCD_MODE_FIRST_WIN: 30,
	SUSTAIN_SOCD_MODE_BYPASS: 31,
	BUTTON_PRESS_TURBO: 32,
	BUTTON_PRESS_MACRO: 33,
	BUTTON_PRESS_MACRO_1: 34,
	BUTTON_PRESS_MACRO_2: 35,
	BUTTON_PRESS_MACRO_3: 36,
	BUTTON_PRESS_MACRO_4: 37,
	BUTTON_PRESS_MACRO_5: 38,
	BUTTON_PRESS_MACRO_6: 39,
	CUSTOM_BUTTON_COMBO: 40,
	BUTTON_PRESS_A3: 41,
	BUTTON_PRESS_A4: 42,
	BUTTON_PRESS_E1: 43,
	BUTTON_PRESS_E2: 44,
	BUTTON_PRESS_E3: 45,
	BUTTON_PRESS_E4: 46,
	BUTTON_PRESS_E5: 47,
	BUTTON_PRESS_E6: 48,
	BUTTON_PRESS_E7: 49,
	BUTTON_PRESS_E8: 50,
	BUTTON_PRESS_E9: 51,
	BUTTON_PRESS_E10: 52,
	BUTTON_PRESS_E11: 53,
	BUTTON_PRESS_E12: 54,
	DIGITAL_DIRECTION_UP: 55,
	DIGITAL_DIRECTION_DOWN: 56,
	DIGITAL_DIRECTION_LEFT: 57,
	DIGITAL_DIRECTION_RIGHT: 58,
	ANALOG_DIRECTION_LS_X_NEG: 59,
	ANALOG_DIRECTION_LS_X_POS: 60,
	ANALOG_DIRECTION_LS_Y_NEG: 61,
	ANALOG_DIRECTION_LS_Y_POS: 62,
	ANALOG_DIRECTION_RS_X_NEG: 63,
	ANALOG_DIRECTION_RS_X_POS: 64,
	ANALOG_DIRECTION_RS_Y_NEG: 65,
	ANALOG_DIRECTION_RS_Y_POS: 66,
	ANALOG_DIRECTION_MOD_LOW: 67,
	ANALOG_DIRECTION_MOD_HIGH: 68,
	BUTTON_PRESS_INPUT_REVERSE: 69,
} as const;

export const PIN_DIRECTIONS = {
	DIRECTION_INPUT: 0,
	DIRECTION_OUTPUT: 1,
} as const;

type PinActionKeys = keyof typeof BUTTON_ACTIONS;
export type PinActionValues = (typeof BUTTON_ACTIONS)[PinActionKeys];

type PinDirectionKeys = keyof typeof PIN_DIRECTIONS;
export type PinDirectionValues = (typeof PIN_DIRECTIONS)[PinDirectionKeys];
