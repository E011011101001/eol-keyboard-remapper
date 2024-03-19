#include <stdio.h>
#include <stdlib.h>
#include <linux/input.h>

typedef struct input_event InputEvent;

// tools

const InputEvent eventSync = {.type = EV_SYN, .code = SYN_REPORT, .value = 0};

void output_event(const InputEvent* pEvent) {
    fwrite(pEvent, sizeof(InputEvent), 1, stdout);
}

void sync(void) {
    output_event(&eventSync);
}

// set event to be eventSync
void reset_event (InputEvent* pEvent) {
    pEvent->type = EV_SYN;
    pEvent->code = SYN_REPORT;
    pEvent->value = 0;
}

// modifiers

/**
 * modifiers should be set to 2 or above after used,
 * so that modifier keys can be used as the original keys if pressed alone
 */

int modEsc = 0;

// remappers
void swap_esc_capslock(InputEvent* pEvent) {
    if (pEvent->type == EV_KEY) {
        if (pEvent->code == KEY_CAPSLOCK) {
            pEvent->code = KEY_ESC;
        } else if (pEvent->code == KEY_ESC) {
            pEvent->code = KEY_CAPSLOCK;
        }
    }
}

void esc_as_modifier (InputEvent* pEvent) {
    if (pEvent->type == EV_KEY && pEvent->code == KEY_ESC) {
        if (pEvent->value == 1) {
            modEsc = 1;
            reset_event(pEvent);
        } else if (pEvent->value == 2) {
            reset_event(pEvent);
        } else if (pEvent->value == 0) {
            if (modEsc >= 2) {
                reset_event(pEvent);
            } else {
                // esc released without used, so it should function as a normal esc
                pEvent->value = 1;
                output_event(pEvent);
                sync();

                pEvent->value = 0;
            }
            modEsc = 0;
        }
    }
}

void esc_hjkl_as_arrows (InputEvent* pEvent) {
    if (modEsc && pEvent->type == EV_KEY) {
        if (pEvent->code == KEY_H) {
            pEvent->code = KEY_LEFT;
        } else if (pEvent->code == KEY_J) {
            pEvent->code = KEY_DOWN;
        } else if (pEvent->code == KEY_K) {
            pEvent->code = KEY_UP;
        } else if (pEvent->code == KEY_L) {
            pEvent->code = KEY_RIGHT;
        }
        modEsc = 2; // even if other keys are pressed
    }
}

int main () {
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    InputEvent event;

    while (fread(&event, sizeof(event), 1, stdin) == 1) {
        swap_esc_capslock(&event);
        esc_as_modifier(&event);
        esc_hjkl_as_arrows(&event);

        output_event(&event);
    }

    return 0;
}

