#pragma once

#include <eer.h>
#include <lr.h>

#define SPIPeriphery(instance, props)                                          \
    eer_define(SPIPeriphery, instance, _(props), {0})

typedef struct {
    eer_gpio_handler_t *  io;

    struct linked_ring *buffer;

    struct {
        void *copi_pin;
        void *cipo_pin;
        void *clk_pin;
        void *chip_select_pin;
    } bus;

    enum eer_bit_order
        bit_order : 1; /* Order in which sending and reading bits */

    struct {
    void (*start)(eer_t *instance);
    void (*receive)(eer_t *instance);
    } on;
} SPIPeriphery_props_t;

typedef struct {
    uint8_t address;
    uint8_t data;
    uint8_t sending;

    uint8_t bit_position;
    bool    clk_level;
} SPIPeriphery_state_t;

eer_header(SPIPeriphery);
