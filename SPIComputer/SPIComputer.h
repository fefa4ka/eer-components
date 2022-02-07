#pragma once

#include <Bitbang.h>

#define SPIComputer(instance, props)                                           \
    eer_define(SPIComputer, instance, _(props), 0)

typedef struct {
    eer_gpio_handler_t *io;
    struct Clock_time * clock;

    uint16_t baudrate;
    enum eer_bit_order
        bit_order : 1; /* Order in which sending and reading bits */

    struct linked_ring *buffer;

    struct {
        void *copi_pin;
        void *cipo_pin;
        void *spacer;
        void *clk_pin;
    } bus;
} SPIComputer_props_t;

typedef struct {
    Bitbang_t bitbanger;

    struct eer_callback *callback;
    void *               chip_select_pin;

    struct {
        struct eer_callback start;
        struct eer_callback receive;
    } on;
} SPIComputer_state_t;

eer_header(SPIComputer);

void SPI_write(SPIComputer_t *spi, unsigned char address, unsigned char value,
               void *chip_select_pin);

void SPI_read(SPIComputer_t *spi, unsigned char address,
              struct eer_callback *callback, void *chip_select_pin);
