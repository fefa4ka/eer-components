#include "IO.h"
#include <eers.h>


/**
 * \brief Initial pin mode configuration
 */
WILL_MOUNT(IO)
{
    /* Configure pin for operation in certain mode */
    if (state->mode == IO_INPUT) {
        state->io->in(state->pin);
    } else {
        state->io->out(state->pin);
    }
}

/**
 * \brief Updates everytime in input mode,
 *        otherwise updates if props changes
 */
SHOULD_UPDATE(IO)
{
    if (state->mode == IO_INPUT) {
        return true;
    }

    if (props->level != next_props->level) {
        return true;
    }

    return false;
}

WILL_UPDATE(IO) {}

/**
 * \brief Actual pin state configuration
 *        and callbacks triggering
 */
RELEASE(IO)
{
    if (state->mode == IO_OUTPUT) {
        /* Set pin level */
        if (props->level == IO_HIGH) {
            state->io->on(state->pin);
        } else {
            state->io->off(state->pin);
        }
    } else {
        bool level = state->io->get(state->pin);
        if (state->level != level) {
            state->level = level;

            if (state->on.change) {
                state->on.change(self);
            }

            if (level != 0 && state->on.high) {
                state->on.high(self);
            } else if (level == 0 && state->on.low) {
                state->on.low(self);
            }
        }
    }
}

/**
 * \brief First level reading
 */
DID_MOUNT(IO)
{
    if (state->on.change) {
        state->on.change(self);
    }
}

DID_UPDATE(IO) {}
