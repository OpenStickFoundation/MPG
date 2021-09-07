# MPG - Multi-Platform Gamepad Library

MPG is a C++ library for managing, manipulating and converting inputs into a usable state for your gamepad projects. With MPG you can:

* Track input state for XInput, DirectInput or Nintendo Switch
* Use a standard set of USB descriptors, report data structures and conversion methods for supported platforms
* Run per-button debouncing with a configurable interval
* Detect and react to hotkey actions
* Emulate Left and Right analog stick movement with digital (D-pad) inputs
* Support a variety of SOCD cleaning methods to prevent invalid directional inputs (finger guns at you /r/fightsticks)

## The MPG Class

The heart of the library is the `MPG` class, and more specifically the virtual `MPG::setup()` and `MPG::read()` class methods that require implementation in your project. Use the `setup()` method for things like initializing microcontroller pins, performing analog calibration, etc., and the `read()` method to implement your platform-specific logic to fill the `MPG.state` member variable. That `state` variable is then used in other class methods for input processing (debouncing, SOCD cleaning, etc.) and to generate USB report data for the selected input type. Once implemented, typical usage of MPG will look kind of like this:

```c++
/*
 * MyAwesomeGamepad.ino
 */

#define GAMEPAD_DEBOUNCE_MILLIS 5

#include <MPG.h>

MPG mpg;

void setup() {
  mpg.setup(); // Runs your custom setup logic
  mpg.load();  // Get saved options if enabled
  mpg.read();  // Perform an initial button read so we can set input mode

  // Use the inlined `pressed` convenience methods
  if (mpg.pressedR3())
    mpg.inputMode = INPUT_MODE_HID;
  else if (mpg.pressedS1())
    mpg.inputMode = INPUT_MODE_SWITCH;
  else if (mpg.pressedS2())
    mpg.inputMode = INPUT_MODE_XINPUT;

  // TODO: Add your USB initialization logic here, something like:
  // setupHardware(mpg.inputMode);
}

void loop() {
  // Cache report pointer and size value
  static uint8_t *report;
  static const uint8_t reportSize = mpg.getReportSize();

  mpg.read();               // Read inputs
  mpg.debounce();           // Run debouncing if required
  mpg.hotkey();             // Check for hotkey changes, can react to returned hotkey action
  mpg.process();            // Process the raw inputs into a usable state
  report = mpg.getReport(); // Convert state to USB report for the selected input mode
  
  // TODO: Add your USB report sending logic here, something like:
  // sendReport(report, reportSize);
}

```

## Implementation Details

MPG provides some declarations and virtual methods that require implementation in order for the library to function correctly. A basic implementation (no debouncing, no storage) requires just two methods to be implemented:

### Required Implementation

* **MPG::setup()** - Use to configure pins, calibrate analog, etc.
* **MPG::read()** - Use to fill the `MPG.state` class member, which is then used in other class methods

A basic `MPG` class implementation in Arduino-land for a Leonardo might look like this:

```c++
/*
 * LeoPad.cpp
 *
 * Example uses direct register reads for faster performance.
 * digitalRead() can still work, but not recommended because SLOW.
 */

#include <MPG.h>

/* Define port/pins for easy readability */

#define PORT_PIN_UP     PF7 // A0
#define PORT_PIN_DOWN   PF6 // A1
#define PORT_PIN_LEFT   PF5 // A2
#define PORT_PIN_RIGHT  PF4 // A3
#define PORT_PIN_P1     PD2 // 1
#define PORT_PIN_P2     PD3 // 0
#define PORT_PIN_P3     PB1 // 15
#define PORT_PIN_P4     PD4 // 4
#define PORT_PIN_K1     PD0 // 3/SCL
#define PORT_PIN_K2     PD1 // 2/SDA
#define PORT_PIN_K3     PB6 // 10
#define PORT_PIN_K4     PD7 // 6
#define PORT_PIN_SELECT PB3 // 14
#define PORT_PIN_START  PB2 // 16
#define PORT_PIN_LS     PB4 // 8
#define PORT_PIN_RS     PB5 // 9

/* Input masks and indexes for setup and read logic */

#define PORTB_INPUT_MASK 0b01111110
#define PORTD_INPUT_MASK 0b10011111
#define PORTF_INPUT_MASK 0b11110000

#define PORTB_INDEX 0
#define PORTD_INDEX 1
#define PORTF_INDEX 2


/* Real implementation starts here... */

void MPG::setup() {
  // Set to input (invert mask to set to 0)
  DDRB = DDRB & ~PORTB_INPUT_MASK;
  DDRD = DDRD & ~PORTD_INPUT_MASK;
  DDRF = DDRF & ~PORTF_INPUT_MASK;

  // Set to high/pullup
  PORTB = PORTB | PORTB_INPUT_MASK;
  PORTD = PORTD | PORTD_INPUT_MASK;
  PORTF = PORTF | PORTF_INPUT_MASK;
}


void MPG::read() {
  // Get port states, invert since INPUT_PULLUP
  uint8_t ports[] = { ~PINB, ~PIND, ~PINF };

  // No analog, but could read them here with analogRead()
  state.lt = 0;
  state.rt = 0;
  state.lx = GAMEPAD_JOYSTICK_MID;
  state.ly = GAMEPAD_JOYSTICK_MID;
  state.rx = GAMEPAD_JOYSTICK_MID;
  state.ry = GAMEPAD_JOYSTICK_MID;

  // Read digital inputs
  state.dpad = 0
    | ((ports[PORTF_INDEX] >> PORT_PIN_UP    & 1)  ? GAMEPAD_MASK_UP    : 0)
    | ((ports[PORTF_INDEX] >> PORT_PIN_DOWN  & 1)  ? GAMEPAD_MASK_DOWN  : 0)
    | ((ports[PORTF_INDEX] >> PORT_PIN_LEFT  & 1)  ? GAMEPAD_MASK_LEFT  : 0)
    | ((ports[PORTF_INDEX] >> PORT_PIN_RIGHT & 1)  ? GAMEPAD_MASK_RIGHT : 0);

  state.buttons = 0
    | ((ports[PORTD_INDEX] >> PORT_PIN_K1 & 1)     ? GAMEPAD_MASK_B1    : 0) // Switch: B, XInput: A
    | ((ports[PORTD_INDEX] >> PORT_PIN_K2 & 1)     ? GAMEPAD_MASK_B2    : 0) // Switch: A, XInput: B
    | ((ports[PORTD_INDEX] >> PORT_PIN_P1 & 1)     ? GAMEPAD_MASK_B3    : 0) // Switch: Y, XInput: X
    | ((ports[PORTD_INDEX] >> PORT_PIN_P2 & 1)     ? GAMEPAD_MASK_B4    : 0) // Switch: X, XInput: Y
    | ((ports[PORTD_INDEX] >> PORT_PIN_P4 & 1)     ? GAMEPAD_MASK_L1    : 0)
    | ((ports[PORTB_INDEX] >> PORT_PIN_P3 & 1)     ? GAMEPAD_MASK_R1    : 0)
    | ((ports[PORTD_INDEX] >> PORT_PIN_K4 & 1)     ? GAMEPAD_MASK_L2    : 0)
    | ((ports[PORTB_INDEX] >> PORT_PIN_K3 & 1)     ? GAMEPAD_MASK_R2    : 0)
    | ((ports[PORTB_INDEX] >> PORT_PIN_SELECT & 1) ? GAMEPAD_MASK_S1    : 0)
    | ((ports[PORTB_INDEX] >> PORT_PIN_START & 1)  ? GAMEPAD_MASK_S2    : 0)
    | ((ports[PORTB_INDEX] >> PORT_PIN_LS & 1)     ? GAMEPAD_MASK_L3    : 0)
    | ((ports[PORTB_INDEX] >> PORT_PIN_RS & 1)     ? GAMEPAD_MASK_R3    : 0)
  ;
}
```

As you can see, there really isn't much to the code, with most of it being defines and fancy formatting of bitwise button reads.

You can also extend and override methods in the `MPG` class if you want to do something like change hotkeys, customize input processing steps, etc.

### Optional Implementation

If you'd like to use all of the features available in MPG, you can also implement the following optional items:

#### Debouncing

Define the debounce time before any MPG library imports:

```c++
#define DEBOUNCE_MILLIS 5
```

Then implement a `getMills` function somewhere you import `MPG.h`:

```c++
// Arduino
uint32_t getMills() { return mills(); }
```

```c++
// Pico SDK
uint32_t getMillis() { return to_ms_since_boot(get_absolute_time()); }
```

#### Persistent Storage

Define the persist storage flag before any MPG library imports:

```c++
#define HAS_PERSISTENT_STORAGE 1
```

The implement the `GamepadStorage` class methods, something like this ATmega32U4 example:

```c++
/*
 * MyAwesomeStorage.cpp
 */

#include <GamepadStorage.h>
#include <GamepadConfig.h>
#include <EEPROM.h>

// No-op implmenetations

GamepadStorage::GamepadStorage() { }

void GamepadStorage::save() { }

// "Real-op" implementations

DpadMode GamepadStorage::getDpadMode() {
  DpadMode mode = DEFAULT_DPAD_MODE;
  EEPROM.get(STORAGE_DPAD_MODE_INDEX, mode);
  return mode;
}

void GamepadStorage::setDpadMode(DpadMode mode) {
  EEPROM.put(STORAGE_DPAD_MODE_INDEX, mode);
}

InputMode GamepadStorage::getInputMode() {
  InputMode mode = DEFAULT_INPUT_MODE;
  EEPROM.get(STORAGE_INPUT_MODE_INDEX, mode);
  return mode;
}

void GamepadStorage::setInputMode(InputMode mode) {
  EEPROM.put(STORAGE_INPUT_MODE_INDEX, mode);
}

SOCDMode GamepadStorage::getSOCDMode() {
  SOCDMode mode = DEFAULT_SOCD_MODE;
  EEPROM.get(STORAGE_SOCD_MODE_INDEX, mode);
  return mode;
}

void GamepadStorage::setSOCDMode(SOCDMode mode) {
  EEPROM.put(STORAGE_SOCD_MODE_INDEX, mode);
}

```
