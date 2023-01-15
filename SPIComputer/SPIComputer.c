#include "SPIComputer.h"
#include <eers.h>

static result_t SPI_init(void *spi, void *bitbanger);
static result_t SPI_receive(void *spi_ptr, void *bitbanger);
static enum eer_pin_mode SPI_modes[] = {PIN_MODE_OUTPUT, PIN_MODE_INPUT};

WILL_MOUNT(SPIComputer)
{
    Bitbang(bitbanger, _({
                           .io    = props->io,
                           .clock = props->clock,

                           .baudrate = props->baudrate,
                           .bit_order = props->bit_order,

                           .pins   = (void **)&props->bus,
                           .modes  = SPI_modes,
                           .buffer = props->buffer,

                           .clk_pin = props->bus.clk_pin,

                           .on = { .start       = &state->on.start,
                                 .transmitted = &state->on.receive,
                                 },
                       }));

    state->bitbanger = bitbanger;

    state->on.start.method   = SPI_init;
    state->on.receive.method = SPI_receive;
    state->on.start.argument = state->on.receive.argument = self;

    eer_staging((eer_t *)&state->bitbanger, 0);
}

SHOULD_UPDATE(SPIComputer)
{
    return Bitbang_should_update(&state->bitbanger, 0);
}

WILL_UPDATE(SPIComputer) { Bitbang_will_update(&state->bitbanger, 0); }

RELEASE(SPIComputer) { Bitbang_release(&state->bitbanger); }

DID_MOUNT(SPIComputer) {}

DID_UPDATE(SPIComputer) { Bitbang_did_update(&state->bitbanger); }


#define lr_owner_chip_select(owner, value) ((owner << 4) | value)
#define lr_owner_callback(owner, pointer)                                      \
    ((owner & 0x0F) | (lr_owner(pointer) << 4))

static result_t SPI_init(void *spi_ptr, void *bitbanger_ptr)
{
    Bitbang_t     *bitbanger  = (Bitbang_t *)bitbanger_ptr;
    SPIComputer_t *spi        = (SPIComputer_t *)spi_ptr;
    lr_owner_t     copi_owner = lr_owner(spi->props.bus.copi_pin);

    if (lr_get(spi->props.buffer, (lr_data_t *)&spi->state.chip_select_pin,
                lr_owner_chip_select(copi_owner, *bitbanger->state.data))) {
        spi->state.chip_select_pin = NULL;
    }

    if (lr_get(spi->props.buffer, (lr_data_t *)&spi->state.callback,
                lr_owner_callback(copi_owner, *bitbanger->state.data))
        == ERROR_BUFFER_EMPTY) {
        spi->state.callback = NULL;
    }

    if (spi->state.chip_select_pin) {
        spi->props.io->out(spi->state.chip_select_pin);
        spi->props.io->on(spi->state.chip_select_pin);
    }

    return OK;
}

/* TODO: Architecture dependent masking */
void SPI_write(SPIComputer_t *spi, unsigned char address, unsigned char value,
               void *chip_select_pin)
{
    lr_owner_t copi_owner = lr_owner(spi->props.bus.copi_pin);

    lr_put(spi->props.buffer, address, copi_owner);
    lr_put(spi->props.buffer, (lr_data_t)chip_select_pin,
             lr_owner_chip_select(copi_owner, address));
    lr_put(spi->props.buffer, value, copi_owner);
    lr_put(spi->props.buffer, (lr_data_t)chip_select_pin,
             lr_owner_chip_select(copi_owner, value));
}

void SPI_read(SPIComputer_t *spi, unsigned char address,
              struct eer_callback *callback, void *chip_select_pin)
{
    lr_owner_t copi_owner = lr_owner(spi->props.bus.copi_pin);

    SPI_write(spi, address, 0, chip_select_pin);
    lr_put(spi->props.buffer, (lr_data_t)callback,
             lr_owner_callback(copi_owner, callback));
}

static result_t SPI_receive(void *spi_ptr, void *bitbanger_ptr)
{
    lr_data_t      data      = 0;
    Bitbang_t     *bitbanger = (Bitbang_t *)bitbanger_ptr;
    SPIComputer_t *spi       = (SPIComputer_t *)spi_ptr;
    uint8_t       *buffer    = bitbanger->state.data + 1;

    lr_get(spi->props.buffer, &data, lr_owner(spi->props.bus.cipo_pin));
    *(buffer) = (uint8_t)data;

    if (spi->state.callback && spi->state.callback->method) {
        spi->state.callback->method(spi->state.callback->argument, buffer);
    }
    if (spi->state.chip_select_pin) {
        spi->props.io->off(spi->state.chip_select_pin);
    }

    return OK;
}
