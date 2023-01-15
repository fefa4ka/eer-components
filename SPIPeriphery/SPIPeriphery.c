#include "SPIPeriphery.h"
#include <eers.h>

static inline uint8_t reverse(uint8_t byte)
{
    byte = (byte & 0xF0) >> 4 | (byte & 0x0F) << 4;
    byte = (byte & 0xCC) >> 2 | (byte & 0x33) << 2;
    byte = (byte & 0xAA) >> 1 | (byte & 0x55) << 1;

    return byte;
}

WILL_MOUNT(SPIPeriphery)
{
    props->io->out(props->bus.cipo_pin);
    props->io->in(props->bus.copi_pin);
    props->io->in(props->bus.clk_pin);
}

SHOULD_UPDATE(SPIPeriphery)
{
    bool clk_level = props->io->get(props->bus.clk_pin);

    /* No communication */
    if (!clk_level && state->bit_position == 0) {
        return false;
    }

    /* Component not selected */
    if (props->bus.chip_select_pin
        && props->io->get(props->bus.chip_select_pin) == 0) {
        return false;
    }

    /* All done */
    if (state->clk_level == clk_level) {
        return false;
    }

    state->clk_level = clk_level;

    /* Clock tick */
    return true;
}

WILL_UPDATE(SPIPeriphery)
{
    /* Data for sending available */
    if (!state->sending && state->bit_position >= 8) {
        lr_data_t sending;
        lr_get(props->buffer, (lr_data_t *)&sending, lr_owner(self));
        if (props->bit_order == BIT_ORDER_MSB) {
            state->sending = reverse((uint8_t)sending);
        } else {

            state->sending = (uint8_t)sending;
        }
    }
}

RELEASE(SPIPeriphery)
{
    uint8_t *pointer;
    uint8_t  position;

    if (state->bit_position < 8) {
        pointer  = &state->address;
        position = state->bit_position;
    } else if (state->bit_position >= 8) {
        pointer  = &state->data;
        position = state->bit_position - 8;
    }

    if (state->clk_level) {
        /* Receiving */
        if (props->io->get(props->bus.copi_pin))
            bit_set(*pointer, position);
        else
            bit_clear(*pointer, position);

    } else if (state->sending && state->bit_position >= 8) {
        /* Sending */
        if (bit_value(state->sending, position))
            props->io->on(props->bus.cipo_pin);
        else
            props->io->off(props->bus.cipo_pin);
    }
}

DID_MOUNT(SPIPeriphery) {}

DID_UPDATE(SPIPeriphery)
{
    void (*callback)(eer_t *) = 0;

    if (state->clk_level && state->bit_position == 7 && props->on.start) {
        callback = props->on.start;

        if(props->bit_order == BIT_ORDER_MSB)
            state->address = reverse(state->address);
    }


    if (state->bit_position == 16) {
        state->bit_position = 0;
        state->sending      = 0;

        if(props->bit_order == BIT_ORDER_MSB)
            state->data = reverse(state->data);

        if (props->on.receive)
            callback = props->on.receive;

        props->io->off(props->bus.cipo_pin);
    } else if (state->clk_level) {
        state->bit_position++;
    }

    if (callback)
        (*callback)(self);
}
