#include "Chip.h"
#include <eers.h>


/**
 * \brief
 */
WILL_MOUNT(Chip)
{
    if(state->sys->init)
        /* TODO: API for Chip initialization config */
        state->sys->init(0);

    if(state->frequency.cpu) {
        eer_sys_clk_source_t cpu_frequency = state->frequency.cpu;
        state->sys->clock.set(&cpu_frequency);
    }

    if(props->on.boot)
        props->on.boot(self);
}

WILL_UPDATE_SKIP(Chip);

/**
 * \brief
 */
SHOULD_UPDATE(Chip)
{
    /* TODO: state->mode get from sys->mode.get() */
    if(state->mode == SYS_MODE_BOOT) {
        state->mode = SYS_MODE_READY;

        if(props->on.ready)
            props->on.ready(self);
    }

    return false;
}

/**
 * \brief
 */
RELEASE(Chip)
{
}

DID_MOUNT_SKIP(Chip);

DID_UPDATE_SKIP(Chip);
