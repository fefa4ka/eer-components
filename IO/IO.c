#include "IO.h"
#include <eers.h>


/**
 * \brief Initial pin mode configuration
 */
WILL_MOUNT(IO)
{
    /* Configure pin for operation in certain mode */
    if (state->mode == IO_INPUT)
        state->io->in(state->pin);
    else
        state->io->out(state->pin);
}

WILL_UPDATE_SKIP(IO);

/**
 * \brief Updates everytime in input mode,
 *        otherwise updates if props changes
 */
SHOULD_UPDATE(IO)
{
    if (state->mode == IO_INPUT)
        return true;

    if (props->level != next_props->level)
        return true;

    return false;
}

/**
 * \brief Actual pin state configuration
 *        and callbacks triggering
 */
RELEASE(IO)
{
    bool changed = true;

    if (state->mode == IO_OUTPUT) {
        /* Set pin level */
        if (props->level == IO_HIGH)
            state->io->on(state->pin);
        else
            state->io->off(state->pin);
    } else {
        bool level = state->io->get(state->pin);
        if (state->level != level) {
            state->level = level;

            if (level != 0 && state->on.high)
                state->on.high(self);
            else if (level == 0 && state->on.low)
                state->on.low(self);
        } else {
            changed = false;
        }
    }

    if (changed && state->on.change)
        state->on.change(self);
}

DID_MOUNT_SKIP(IO);

DID_UPDATE_SKIP(IO);
