#include <Chip.h>

#include <eer_test.h>
#include <unit.h>


bool is_booted = false;
bool is_looped = false;
bool is_ready = false;

void chip_boot(eer_t *chip) {
    is_booted = true;
}

void chip_ready(eer_t *chip) {
    is_ready = true;
}

Chip(sys,
    _({
        .on = {
            .boot = chip_boot,
            .ready = chip_ready,
        }
    }),
    _({
        .frequency = {.cpu = 0},
        .sys       = &hw(sys),
    })
);

test(initial, ready, shutdowned)
{
    // Event-loop
    ignite(sys);

    if(!is_looped) {
        is_looped = true;
    }

    halt(0);
}

result_t initial()
{
    test_assert(!is_booted && !is_ready, "Should not booted, not ready");

    while(!is_booted);
    test_assert(is_booted && !is_looped && !is_ready, "Should booted, not looped, not ready. is_booted = %d is_looped = %d is_ready = %d", is_booted, is_looped, is_ready);

    return OK;
}

result_t ready()
{
    while(!is_booted || !is_looped);
    test_assert(is_booted && is_looped && !is_ready, "Should booted and looped, not ready. is_booted = %d is_looped = %d is_ready = %d", is_booted, is_looped, is_ready);

    while(!is_ready);
    test_assert(is_booted && is_ready, "Should be ready");

    return OK;
}

result_t shutdowned()
{
    return OK;
}
