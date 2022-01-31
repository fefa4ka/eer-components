#pragma once

#include <Clock.h>
#include <lr.h>
#include <stddef.h>

#define Bitbang(instance, props)     eer_define(Bitbang, instance, _(props), 0)
#define Bitbang_new(instance, state) eer_withstate(Bitbang, instance, state)


#ifndef BITBANG_MAX_LINES
    #define BITBANG_MAX_LINES 8
#endif

typedef struct {
    eer_io_handler_t *   io;
    struct Clock_time *clock;

    // TODO: use macros and pack to uint8_t
    uint16_t baudrate;

    enum eer_io_level chip_select_polarity : 1;
    enum eer_bit_order
        bit_order : 1; /* Order in which sending and reading bits */

    void **pins; /* List of pins used for lines */
    enum eer_pin_mode
        *modes; /* Related pins mapping for IO mode configuration */
    struct linked_ring *buffer;  /* Bitbang data buffers for each line */
    void *              clk_pin; /* Optional clock pin */

    struct {
        struct eer_callback *start;
        struct eer_callback *transmitted;
    } on;
} Bitbang_props_t;

typedef struct {
    bool operating : 1;
    bool clock : 1;

    uint8_t data[BITBANG_MAX_LINES]; /* Current sending and reading bytes
                                              for each data line */

    uint8_t position; /* Data bit index now operating */

    uint16_t tick;
} Bitbang_state_t;

eer_header(Bitbang);
