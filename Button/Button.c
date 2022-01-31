#include "Button.h"
#include <eers.h>

/**
 * \brief  Configure pins for input and pullup if needed
 */
WILL_MOUNT(Button)
{
    eer_io_handler_t *io = props->io;

    /* Setup pin as input */
    io->in(props->pin);
    state->inverse = props->type == BUTTON_PUSH_PULLUP
                     || props->type == BUTTON_TOGGLE_PULLUP;

    if (state->inverse) {
        io->pullup(props->pin);
    }
}

/**
 * \brief  Check if level doesn't change during bounce_delay_ms
 *         or check if push button released
 */
SHOULD_UPDATE(Button)
{
    bool level          = props->io->get(props->pin);
    bool is_level_equal = state->level == level;
    bool state_level    = state->inverse ? !state->level : state->level;
    int  passed         = props->clock->ms - state->tick;

    /* Tick overflow */
    if (passed < 0) {
        passed = 65535 - state->tick + props->clock->ms;
    }

    /* Second check after bounce_delay_ms */
    if (state->tick && passed >= props->bounce_delay_ms) {
        return true;
    }

    /* Already pressed */
    if (state_level == 1 && is_level_equal && state->pressed) {
        return false;
    }

    /* First high level detected */
    if (state_level == 0 && !is_level_equal) {
        return true;
    }

    /* Another checks after pressed */
    if ((props->type == BUTTON_PUSH_PULLUP || props->type == BUTTON_PUSH)
        && state->pressed && (level == 0 || state->inverse)) {
        /* Push button unpressed after release */
        return true;
    }

    state->level = level;
    return false;
}

/**
 * \brief            Prepare button internal states
 */
WILL_UPDATE(Button)
{
    /* Actual state reading */
    state->level     = props->io->get(props->pin);
    bool state_level = state->inverse ? !state->level : state->level;

    /* Set initial tick to start delay counting */
    if (!state->tick && state_level) {
        state->tick = props->clock->ms;
    }
}

/**
 * \brief            Button pressed state logic
 */
RELEASE(Button)
{
    int  passed      = props->clock->ms - state->tick;
    bool pressed     = false;
    bool state_level = state->inverse ? !state->level : state->level;


    /* Tick overflow */
    if (passed < 0) {
        passed = 65535 - state->tick + props->clock->ms;
    }

    /* Cache button state for toggling */
    if (props->type == BUTTON_TOGGLE_PULLUP || props->type == BUTTON_TOGGLE) {
        pressed = state->pressed;
    }

    /* Change state if bounce filtered */
    if (state->tick && passed > props->bounce_delay_ms) {
        if (state_level) {
            if (props->type == BUTTON_TOGGLE_PULLUP
                || props->type == BUTTON_TOGGLE) {
                pressed = !state->pressed;
            } else {
                pressed = true;
            }
        }

        state->tick = 0;
    }

    /* Callback triggering */
    if (state->pressed != pressed) {
        if (pressed) {
            if (props->on.press)
                props->on.press(self);
        } else {
            if (props->on.release)
                props->on.release(self);
        }

        if (props->on.toggle)
            props->on.toggle(self);

        state->pressed = pressed;
    }
}

DID_MOUNT(Button) {}
DID_UPDATE(Button) {}
DID_UNMOUNT(Button) {}
