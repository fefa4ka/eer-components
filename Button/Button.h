#pragma once

#include <Clock.h>

#define Button(instance, props)     eer_define(Button, instance, _(props), {0})
#define Button_is_pressed(instance) eer_state(Button, instance, pressed)

typedef struct {
    eer_gpio_handler_t *io;
    void *              pin;

    enum {
        BUTTON_PUSH_PULLUP, /* Button pressed only when pushed */
        BUTTON_PUSH,
        BUTTON_TOGGLE_PULLUP, /* Pressed state change after every push */
        BUTTON_TOGGLE
    } type;


    struct Clock_time *clock; /* Timestamp for bounce filtering */
    int                bounce_delay_ms;

    struct {
        void (*press)(eer_t *instance);
        void (*release)(eer_t *instance);
        void (*toggle)(eer_t *instance); /* When pressed state changes */
    } on;
} Button_props_t;

typedef struct {
    union {
        struct {
            bool inverse : 1; /* Is pin operating in pullup configuration */
            bool level : 1;
            bool pressed : 1; /* Is button pressed */
        };
        unsigned char flags;
    };
    unsigned long tick; /* Timestamp when pin state changed */
} Button_state_t;

eer_header(Button);
