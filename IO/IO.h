#pragma once

#include <eer.h>

#define IO(instance)            eer(IO, instance)
#define IO_new(instance, state) eer_withstate(IO, instance, _(state))
#define IO_level(instance)      eer_state(IO, instance, level)

typedef struct {
    /* Desired level of the pin (high/low) for output mode */
    enum { IO_LOW, IO_HIGH } level;
} IO_props_t;

typedef struct {
    /* Current level of the pin (high/low) */
    bool level;
    /* HAL handlers for GPIO */
    eer_gpio_handler_t *io;
    /* Pointer to pin object that could be handled by HAL */
    void *pin;

    /* Pin operation mode */
    enum { IO_OUTPUT, IO_INPUT } mode;

    /* Callbacks on IO evens */
    struct {
        void (*change)(eer_t *instance);
        void (*low)(eer_t *instance);
        void (*high)(eer_t *instance);
    } on;
} IO_state_t;

eer_header(IO);
