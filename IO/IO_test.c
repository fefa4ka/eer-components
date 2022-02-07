#include <IO.h>
#include <eers.h>
#include <unit.h>
#include <version.h>


/* Indicator */
pin_t led_pin = hw_pin(led, 0);
IO_new(led, _({
                .io   = &hw(gpio),
                .pin  = &led_pin,
                .mode = IO_OUTPUT,
            }));

/* Sensor */
bool  enabled    = false;
pin_t sensor_pin = hw_pin(sensor, 0);
void  sensor_read(eer_t *trigger)
{
    IO_t *sensor = (IO_t *)trigger;
    enabled      = sensor->state.level;
}
IO_new(sensor, _({
                   .io   = &hw(gpio),
                   .pin  = &sensor_pin,
                   .mode = IO_INPUT,
                   .on   = {.change = sensor_read},
               }));

pin_t led_control_pin = hw_pin(led_control, 1);
pin_t debug_pin       = hw_pin(debug, 0);

test(toggle_sensor, toggle_led)
{
    // Event-loop
    loop(sensor)
    {
        apply(IO, led,
              _({
                  .level = hw(gpio).get(&led_control_pin) ? IO_HIGH : IO_LOW,
              }));
    }
}

result_t toggle_sensor()
{
    test_assert(hw(gpio).get(&sensor_pin) == 0, "Sensor pin should be off");
    test_assert(enabled == false, "Enabled should be false");

    hw(gpio).on(&sensor_pin);
    usleep(20);
    test_assert(enabled == true, "Enabled should be true");
    test_assert(hw(gpio).get(&sensor_pin) == 1, "Sensor pin should be on");

    hw(gpio).off(&sensor_pin);
    usleep(20);
    test_assert(hw(gpio).get(&sensor_pin) == 0, "Sensor pin should be off");
    test_assert(enabled == false, "Enabled should be false");

    return OK;
}

result_t toggle_led()
{
    test_assert(hw(gpio).get(&led_pin) == 0, "Led pin should be off");
    test_assert(hw(gpio).get(&led_control_pin) == 0,
                "Led control pin should be off");
    test_assert(hw(gpio).get(&debug_pin) == 0, "Debug pin should be off");

    hw(gpio).on(&led_control_pin);
    usleep(20);
    test_assert(hw(gpio).get(&led_control_pin) == 1,
                "Led control pin should be on");
    test_assert(hw(gpio).get(&debug_pin) == 0, "Debug pin should be off");
    test_assert(hw(gpio).get(&led_pin), "Led pin should be on");

    hw(gpio).off(&led_control_pin);
    usleep(20);
    test_assert(hw(gpio).get(&led_pin) == 0, "Led pin should be off");
    test_assert(hw(gpio).get(&led_control_pin) == 0,
                "Led control pin should be off");
    test_assert(hw(gpio).get(&debug_pin) == 0, "Debug pin should be off");

    hw(gpio).on(&debug_pin);
    usleep(20);
    test_assert(hw(gpio).get(&led_pin) == 0, "Led pin should be off");
    test_assert(hw(gpio).get(&led_control_pin) == 0,
                "Led control pin should be off");
    test_assert(hw(gpio).get(&debug_pin), "Debug pin should be on");

    hw(gpio).on(&led_control_pin);
    usleep(20);
    test_assert(hw(gpio).get(&led_pin), "Led pin should be on");
    test_assert(hw(gpio).get(&led_control_pin), "Led pin should be on");
    test_assert(hw(gpio).get(&debug_pin), "Debug pin should be on");

    hw(gpio).flip(&led_control_pin);
    usleep(10);
    test_assert(hw(gpio).get(&led_pin) == 0, "Led pin should be off");
    test_assert(hw(gpio).get(&led_control_pin) == 0,
                "Led control pin should be off");
    test_assert(hw(gpio).get(&debug_pin), "Debug pin should be on");

    hw(gpio).flip(&debug_pin);
    usleep(10);
    test_assert(hw(gpio).get(&led_pin) == 0, "Led pin should be off");
    test_assert(hw(gpio).get(&led_control_pin) == 0,
                "Led control pin should be off");
    test_assert(hw(gpio).get(&debug_pin) == 0, "Debug pin should be off");

    return OK;
}
