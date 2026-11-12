# menu-button-handler
Button handling logic used for STM32-based remote control.

## Project Overview

This repository contains the button processing logic for a two-button remote control interface.

The implementation provides:

- Button debouncing
- Short press detection
- Long press detection
- Menu event generation

The module is designed to work together with the menu system used in my robotics projects.

## Features

- Debounced button inputs
- Short press detection
- Long press detection
- Event-based interface
- Low CPU usage

## Hardware Assumptions

Buttons are connected using internal pull-up resistors.

Button state:

```text
GPIO_PIN_RESET = Pressed
GPIO_PIN_SET   = Released
```

## Adjustable Timing Configuration

| Parameter | Value |
|------------|------------|
| Scan Period | 5 ms |
| Debounce Time | 20 ms |
| Long Press Time | 300 ms |

These values were determined empirically and should be adjusted to your liking.

## Button Mapping

| Button | Short Press | Long Press |
|------------|------------|------------|
| UP | EVENT_UP | EVENT_BACK |
| DOWN | EVENT_DOWN | EVENT_ENTER |

## Control Layout

```text
UP
├── Short Press -> Up
└── Long Press  -> Back

DOWN
├── Short Press -> Down
└── Long Press  -> Enter
```

## Usage

Execute periodically every 5 ms:

```c
Buttons_Task();
```

Read pending events:

```c
MenuEvent_t event =
    Buttons_GetEvent();
```

Forward event to menu:

```c
Menu_Process(event);
```

## Event Flow

```text
Button Press
     │
     ▼
Debounce
     │
     ▼
Short / Long Press Detection
     │
     ▼
Event Queue
     │
     ▼
Menu System
```

## Notes

- Long presses generate their event immediately after the hold time expires.
- Short press events are generated on release.
- New button events overwrite older unconsumed events.
- Intended for simple two-button menu navigation, for example game pad joystick buttons.

## Future Improvements

- Auto-repeat functionality
- Configurable timing
- Support for additional buttons
- Encoder support
- Potentiometer support
