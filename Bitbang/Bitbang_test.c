#include <Bitbang.h>

#include <string.h>
#include <eer_test.h>


#define BUFFER_SIZE 32
#define buffer_size(owner)                                                     \
    lr_length_owned(&buffer, lr_owner(owner))

struct lr_cell     cells[BUFFER_SIZE] = {0};
struct linked_ring buffer             = {cells, BUFFER_SIZE};

Clock(clk, &hw(timer), TIMESTAMP);

pin_t clk_pin  = hw_pin(CLK, 1);
pin_t copi_pin = hw_pin(MOSI, 1);
pin_t cipo_pin = hw_pin(MISO, 1);


/* Bitbang SPI output */
pin_t *       spi_pins[]  = {&copi_pin, &cipo_pin, NULL};
enum pin_mode spi_modes[] = {PIN_MODE_OUTPUT, PIN_MODE_INPUT};
Bitbang(spi, _({.io        = &hw(gpio),
                .clock     = &clk.state.time,
                .baudrate  = 9600,
                .bit_order = BIT_ORDER_MSB,
                .pins      = (void **)spi_pins,
                .clk_pin   = &clk_pin,
                .modes     = spi_modes,
                .buffer    = &buffer}));

test(count) {
    loop(clk, spi);
}

result_t count()
{
    unsigned char number_order[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 0};
    lr_data_t input_data;

    test_assert(buffer_size(&copi_pin) == 0,
                "COPI buffer should be empty, but %d", buffer_size(&copi_pin));
    test_assert(buffer_size(&cipo_pin) == 0,
                "CIPO buffer should be empty, but %d", buffer_size(&cipo_pin));

    hw(gpio).on(&cipo_pin);
    lr_write_string(&buffer, number_order, lr_owner(&copi_pin));
    sleep(3);

    test_assert(buffer_size(&copi_pin) == 0,
                "COPI buffer should be empty, but %d", buffer_size(&copi_pin));
    test_assert(buffer_size(&cipo_pin) == strlen((const char *)number_order),
                "CIPO buffer should be %lu, but %d", strlen((const char *)number_order),
                buffer_size(&cipo_pin));

    lr_read(&buffer, &input_data, lr_owner(&cipo_pin));
    test_assert((uint8_t)input_data == 255, "First byte should be 255, but %d", (uint8_t)input_data);
    lr_read(&buffer, &input_data, lr_owner(&cipo_pin));
    test_assert((uint8_t)input_data == 255, "Second byte should be 255, but %d", (uint8_t)input_data);

    return OK;
}
