#include "Bitbang.h"
#include <eers.h>

#define foreach_pins(pin, pins)                                                \
    void **pin;                                                                \
    for (pin = pins; *pin; pin++)

unsigned char reverse(unsigned char b)
{
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;

    return b;
}

/**
 * \brief    Configure used pins for selected modes
 */
WILL_MOUNT(Bitbang)
{
    // setup pins
    enum eer_pin_mode *mode = props->modes;

    if (props->clk_pin) {
        props->io->out(props->clk_pin);
        props->io->off(props->clk_pin);
    }

    foreach_pins(pin, props->pins)
    {
        if (*mode == PIN_MODE_INPUT)
            props->io->in(*pin);
        else
            props->io->out(*pin);

        mode++;
    }
}

/**
 * \brief    Updates if output data available
 *           or if next operation scheduled.
 *           Primary output data should be
 *           on the first pin.
 */
SHOULD_UPDATE(Bitbang)
{
    /* Component free for operation */
    if (!state->operating) {
        // TODO: Check every OUTPUT pin
        if (lr_exists(props->buffer, lr_owner(*props->pins)))
            /* Data available */
            return true;

        /* No data available */
        return false;
    }

    if (!props->baudrate)
        return true;

    /* Change state on baudrate */
    // TODO: Optimize baudrate calculation
    uint16_t bit_duration_us    = 1e6 / props->baudrate;
    uint16_t next_tick          = props->clock->us;
    int16_t  probably_passed_us = next_tick - state->tick;
    if (probably_passed_us < 0)
        probably_passed_us = 65535 - state->tick + next_tick;

    if (state->clock && probably_passed_us >= bit_duration_us >> 1) {
        /* Clock tick fall */
        state->clock = false;
        if (props->clk_pin)
            props->io->off(props->clk_pin);
    }

    if (probably_passed_us >= bit_duration_us)
        return true;

    return false;
}

/**
 * \brief    Prepare sending data and read input
 *           Should be called with non zero clock->us
 */
WILL_UPDATE(Bitbang)
{
    uint8_t *      data    = state->data;
    enum eer_pin_mode *mode    = props->modes;
    bool           sending = state->operating;

    state->tick = props->clock->us;

    foreach_pins(pin, props->pins)
    {
        if (*mode == PIN_MODE_INPUT) {
            if (state->position == 8) {
                /* Write full byte from input */
                // TODO: if zero?
                if (*data)
                    lr_write(props->buffer, *data, lr_owner(*pin));
            } else {
                /* Read bit */
                if (props->io->get(*pin))
                    bit_set(*data, state->position);
                else
                    bit_clear(*data, state->position);
            }
        } else {
            /* Read new byte for output */
            if (!state->operating) {
                lr_data_t cell_data = 0;
                if (lr_read(props->buffer, &cell_data, lr_owner(*pin))
                    == OK) {
                    sending = true;
                    *data   = (uint8_t)cell_data;
                    if (props->bit_order == BIT_ORDER_MSB)
                        *data = reverse(*data);
                }
            }
        }

        data++;
        mode++;
    }


    /* Callback with argument */
    if (sending && state->operating == false)
        if (props->on.start && props->on.start->method)
            props->on.start->method(self, props->on.start->argument);

    state->operating = sending;

    if (state->position == 8) {
        /* Clear session */
        state->tick      = 0;
        state->position  = 0;
        state->operating = false;

        if (props->clk_pin)
            /* Clock falling tick */
            props->io->off(props->clk_pin);

        if (props->on.transmitted && props->on.transmitted->method)
            props->on.transmitted->method(self, props->on.transmitted->argument);
    }
}

/**
 * \brief     Set level for output lines
 */
RELEASE(Bitbang)
{
    uint8_t *      data = state->data;
    enum eer_pin_mode *mode = props->modes;

    foreach_pins(pin, props->pins)
    {
        if (*mode == PIN_MODE_OUTPUT) {
            if (bit_value(*data, state->position))
                props->io->on(*pin);
            else
                props->io->off(*pin);

            if (state->position == 7)
                *data = 0;
        }
        data++;
        mode++;
    }
}

DID_MOUNT(Bitbang) {}

/**
 * \brief    Clock ticks and transmitting finishing
 */
DID_UPDATE(Bitbang)
{
    if (state->operating) {
        /* Increment bit position */
        state->position++;

        /* Clock rise tick */
        state->clock = true;
        if (props->clk_pin)
            props->io->on(props->clk_pin);
    }
}

// TODO: Unmount Off every pin
