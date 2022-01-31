#include <SPIComputer.h>
#include <string.h>

#include <eers.h>
#include <unit.h>
#include <version.h>

#define BUFFER_SIZE 16
struct lr_cell     cells[BUFFER_SIZE] = {0};
struct linked_ring buffer             = {cells, BUFFER_SIZE};

Clock(clk, &hw.timer, TIMESTAMP);

pin_t copi_pin        = hw_pin(MOSI, 0);
pin_t cipo_pin        = hw_pin(MISO, 0);
pin_t clk_pin         = hw_pin(CLK, 0);
pin_t chip_select_pin = hw_pin(CS, 0);

pin_t debug_pin = hw_pin(DEBUG, 1);

SPIComputer(spi, _({.io        = &hw.io,
                    .clock     = &clk.state.time,
                    .baudrate  = 1200,
                    .buffer    = &buffer,
                    .bit_order = BIT_ORDER_MSB,
                    .bus       = {
                        .copi_pin = &copi_pin,
                        .cipo_pin = &cipo_pin,
                        .clk_pin  = &clk_pin,
                    }}));

const char                 world[] = "world";
extern struct eer_callback callback;
result_t                   hello(void *message, void *argument)
{
    uint8_t address = *(uint8_t *)message;

    hw.io.flip(&debug_pin);

    test_assert(strcmp((char *)argument, "world") == 0,
                "argument should be ptr to world")

        return OK;
}

struct eer_callback callback = {hello, world};

test(spi_callback)
{
    SPI_write(&spi, 0x31, 0x33, &chip_select_pin);
    SPI_read(&spi, 0x07, &callback, &chip_select_pin);

    loop(clk, spi);
}


result_t spi_callback()
{
    sleep(2);
    test_assert(hw.io.get(&debug_pin),
                "Pin shoud be high, raised by SPI_read callback");
    test_assert(lr_length(&buffer) == 0,
                "Buffer should be empty after communication, but %d",
                lr_length(&buffer));
    return OK;
}
