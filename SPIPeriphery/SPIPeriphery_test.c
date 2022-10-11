
#include <SPIComputer.h>
#include <SPIPeriphery.h>
#include <string.h>

#include <eers.h>
#include <unit.h>

#define BUFFER_SIZE 16
struct lr_cell     cells[BUFFER_SIZE] = {0};
struct linked_ring buffer             = {cells, BUFFER_SIZE};

Clock(clk, &hw(timer), TIMESTAMP);

pin_t copi_pin        = hw_pin(MOSI, 0);
pin_t cipo_pin        = hw_pin(MISO, 0);
pin_t clk_pin         = hw_pin(CLK, 0);
pin_t chip_select_pin = hw_pin(CS, 0);

pin_t debug_pin = hw_pin(DEBUG, 1);

SPIComputer(spi, _({.io       = &hw(gpio),
                    .clock    = &clk.state.time,
                    // Could failed if speed too much for debug eer step
                    .baudrate = 2400,
                    .buffer   = &buffer,
                    .bus      = {
                             .copi_pin = &copi_pin,
                             .cipo_pin = &cipo_pin,
                             .clk_pin  = &clk_pin,
                    }}));

uint8_t debug_address = 0;
void    mirror_echo(eer_t *instance)
{
    SPIPeriphery_t *mirror  = (SPIPeriphery_t *)instance;
    uint8_t         address = mirror->state.address + 1;
    if (!address)
        eer_stop = 1;

    lr_write(&buffer, address, lr_owner(instance));
}

void mirror_receive(eer_t *instance)
{
    SPIPeriphery_t *mirror = (SPIPeriphery_t *)instance;
}

SPIPeriphery(mirror, _({
            .io = &hw(gpio),
            .buffer = &buffer,
            .bus      = {
                .copi_pin = &copi_pin,
                .cipo_pin = &cipo_pin,
                .clk_pin  = &clk_pin,
                .chip_select_pin = &chip_select_pin,
            },
            .on = { .start = mirror_echo,
            .receive = mirror_receive, },
            }));

const char                 world[] = "world";
extern struct eer_callback callback;
result_t                   hello(void *argument, void *message)
{
    uint8_t address = *(uint8_t *)message;

    hw(gpio).flip(&debug_pin);

    SPI_read(&spi, address, &callback, &chip_select_pin);

    test_assert(strcmp((char *)argument, "world") == 0,
                "argument should be ptr to world");

    if(debug_address)
        test_assert((address - debug_address) == 1, "Address should be incremented by one, but %d-%d=%d", address, debug_address, address - debug_address);
    debug_address = address;

    return OK;
}

struct eer_callback callback = {hello, world};

test(spi_callback)
{
    SPI_write(&spi, 0x31, 0x33, &chip_select_pin);
    SPI_read(&spi, 0x01, &callback, &chip_select_pin);

    loop(clk, mirror, spi);
}


result_t spi_callback()
{
    sleep(2);

    test_assert(debug_address == 0xff,
                "Address should incremented to 0xFF, but 0x%x", debug_address);

    return OK;
}
