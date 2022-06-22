/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: Copyright (c) 2021 Jason Skuby (mytechtoybox.com)
 */

#pragma once

// The available input modes
// TODO: Think about making enums "class enum" instead. This will keep them out of the global namespace
// and provide strong type assurance instead of being treated as ints. Also, modern c++ would tend towards
// using Pascal-case naming for the individual declarations.
typedef enum
{
	INPUT_MODE_XINPUT,
	INPUT_MODE_SWITCH,
	INPUT_MODE_HID,
	INPUT_MODE_CONFIG = 255,
} InputMode;

// The available stick emulation modes
typedef enum
{
	DPAD_MODE_DIGITAL,
	DPAD_MODE_LEFT_ANALOG,
	DPAD_MODE_RIGHT_ANALOG,
} DpadMode;

// The available SOCD cleaning methods
typedef enum
{	
	// Legacy SOCD Modes for compatability purposes
	SOCD_MODE_UP_PRIORITY,           // U+D=U, L+R=N
	SOCD_MODE_NEUTRAL,               // U+D=N, L+R=N
	SOCD_MODE_SECOND_INPUT_PRIORITY, // U>D=D, L>R=R (Last Input Priority, aka Last Win)

	// Comprehensive SOCD Modes (LIP = Last Input Priority / FIP = First Input Priority)
	SOCD_MODE_X_NEUTRAL_Y_NEUTRAL,   // L+R=N, U+D=N
	SOCD_MODE_X_NEUTRAL_Y_DOWN,      // L+R=N, U+D=D
	SOCD_MODE_X_NEUTRAL_Y_UP,        // L+R=N, U+D=U
	SOCD_MODE_X_NEUTRAL_Y_LIP,       // L+R=N, U+D=LIP
	SOCD_MODE_X_NEUTRAL_Y_FIP,       // L+R=N, U+D=FIP
	SOCD_MODE_X_LEFT_Y_NEUTRAL,      // L+R=L, U+D=N
	SOCD_MODE_X_LEFT_Y_DOWN,         // L+R=L, U+D=D
	SOCD_MODE_X_LEFT_Y_UP,           // L+R=L, U+D=U
	SOCD_MODE_X_LEFT_Y_LIP,          // L+R=L, U+D=LIP
	SOCD_MODE_X_LEFT_Y_FIP,          // L+R=L, U+D=FIP
	SOCD_MODE_X_RIGHT_Y_NEUTRAL,     // L+R=R, U+D=N
	SOCD_MODE_X_RIGHT_Y_DOWN,        // L+R=R, U+D=D
	SOCD_MODE_X_RIGHT_Y_UP,          // L+R=R, U+D=U
	SOCD_MODE_X_RIGHT_Y_LIP,         // L+R=R, U+D=LIP
	SOCD_MODE_X_RIGHT_Y_FIP,         // L+R=R, U+D=FIP
	SOCD_MODE_X_LIP_Y_NEUTRAL,       // L+R=LIP, U+D=N
	SOCD_MODE_X_LIP_Y_DOWN,          // L+R=LIP, U+D=D
	SOCD_MODE_X_LIP_Y_UP,            // L+R=LIP, U+D=U
	SOCD_MODE_X_LIP_Y_LIP,           // L+R=LIP, U+D=LIP
	SOCD_MODE_X_LIP_Y_FIP,           // L+R=LIP, U+D=FIP
	SOCD_MODE_X_FIP_Y_NEUTRAL,       // L+R=FIP, U+D=N
	SOCD_MODE_X_FIP_Y_DOWN,          // L+R=FIP, U+D=D
	SOCD_MODE_X_FIP_Y_UP,            // L+R=FIP, U+D=U
	SOCD_MODE_X_FIP_Y_LIP,           // L+R=FIP, U+D=LIP
	SOCD_MODE_X_FIP_Y_FIP,           // L+R=FIP, U+D=FIP
} SOCDMode;

// Enum for tracking last direction state of Second Input SOCD method
typedef enum
{
	DIRECTION_NONE,
	DIRECTION_UP,
	DIRECTION_DOWN,
	DIRECTION_LEFT,
	DIRECTION_RIGHT
} DpadDirection;

// The available hotkey actions
typedef enum
{
	HOTKEY_NONE              = 0x00,
	HOTKEY_DPAD_DIGITAL      = (1U << 0),
	HOTKEY_DPAD_LEFT_ANALOG  = (1U << 1),
	HOTKEY_DPAD_RIGHT_ANALOG = (1U << 2),
	HOTKEY_HOME_BUTTON       = (1U << 3),
	HOTKEY_CAPTURE_BUTTON    = (1U << 4),
	HOTKEY_SOCD_UP_PRIORITY  = (1U << 5),
	HOTKEY_SOCD_NEUTRAL      = (1U << 6),
	HOTKEY_SOCD_LAST_INPUT   = (1U << 7),
	HOTKEY_INVERT_X_AXIS     = (1U << 8),
	HOTKEY_INVERT_Y_AXIS     = (1U << 9),
} GamepadHotkey;
