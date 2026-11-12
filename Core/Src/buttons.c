#include "main.h"
#include "buttons.h"

#define BUTTON_SCAN_MS       5U
#define DEBOUNCE_TIME_MS    20U
#define LONG_PRESS_MS      300U

#define DEBOUNCE_TICKS    (DEBOUNCE_TIME_MS / BUTTON_SCAN_MS)
#define LONG_PRESS_TICKS  (LONG_PRESS_MS / BUTTON_SCAN_MS)

typedef struct
{
    GPIO_TypeDef *port;
    uint16_t pin;

    uint16_t debounceCounter;
    uint16_t holdCounter;

    uint8_t stableState;       // 0 = released, 1 = pressed
    uint8_t lastRawState;

    bool eventPending;
    bool longPressTriggered;
    MenuEvent_t pendingEvent;

} Button_t;

static Button_t buttonUp = {
    .port = MENU_BTN_UP_GPIO_Port,
    .pin = MENU_BTN_UP_Pin
};

static Button_t buttonDown = {
    .port = MENU_BTN_DOWN_GPIO_Port,
    .pin = MENU_BTN_DOWN_Pin
};

static bool ReadButton(Button_t *btn)
{
    return HAL_GPIO_ReadPin(btn->port, btn->pin) == GPIO_PIN_RESET;
}

static void SetEvent(Button_t *btn, MenuEvent_t event)
{
    // A new event replaces an older unconsumed event.
    btn->pendingEvent = event;
    btn->eventPending = true;
}

static void UpdateButton(Button_t *btn,
                         MenuEvent_t shortEvent,
                         MenuEvent_t longEvent)
{
    uint8_t raw = ReadButton(btn) ? 1U : 0U;

    // Debounce: require a consistent raw state for DEBOUNCE_TICKS scans.
    if (raw != btn->lastRawState)
    {
        btn->lastRawState = raw;
        btn->debounceCounter = 0U;
    }
    else if (btn->debounceCounter < DEBOUNCE_TICKS)
    {
        btn->debounceCounter++;
    }

    // Only accept a new state after it has remained stable long enough.
    if ((btn->debounceCounter >= DEBOUNCE_TICKS) &&
        (raw != btn->stableState))
    {
        btn->stableState = raw;
        btn->debounceCounter = 0U;

        if (btn->stableState == 1U) // Pressed
        {
            btn->holdCounter = 0U;
            btn->longPressTriggered = false;
        }
        else // Released
        {
            if (!btn->longPressTriggered)
            {
                SetEvent(btn, shortEvent);
            }

            btn->holdCounter = 0U;
        }
    }

    // Count the hold only while the debounced state is pressed.
    if ((btn->stableState == 1U) && !btn->longPressTriggered)
    {
        if (btn->holdCounter < LONG_PRESS_TICKS)
        {
            btn->holdCounter++;
        }

        if (btn->holdCounter >= LONG_PRESS_TICKS)
        {
            btn->longPressTriggered = true;
            SetEvent(btn, longEvent);
        }
    }
}

void Buttons_Task(void)
{
    UpdateButton(&buttonUp, EVENT_UP, EVENT_BACK);
    UpdateButton(&buttonDown, EVENT_DOWN, EVENT_ENTER);
}

MenuEvent_t Buttons_GetEvent(void)
{
    if (buttonUp.eventPending)
    {
        buttonUp.eventPending = false;
        return buttonUp.pendingEvent;
    }

    if (buttonDown.eventPending)
    {
        buttonDown.eventPending = false;
        return buttonDown.pendingEvent;
    }

    return EVENT_NONE;
}

