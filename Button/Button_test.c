#include <IO.h>
#include <Clock.h>
#include <Button.h>

#include <eers.h>
#include <unit.h>
#include <version.h>

/* Datetime couting */
Clock(clk, &hw(timer), 0);

/* Indicator */
pin_t led_pin = hw_pin(led, 1);
IO_new(led, _({
                .io   = &hw(gpio),
                .pin  = &led_pin,
                .mode = IO_OUTPUT,
            }));

/* Mode switcher. Led could blink When enabled. */
bool  enabled      = false;
pin_t switcher_pin = hw_pin(switcher, 1);
void  switcher_toggle(eer_t *trigger) { enabled = !enabled; }
void  switcher_pressed(eer_t *trigger) {}
void  switcher_released(eer_t *trigger) {}
Button(switcher, _({.io  = &hw(gpio),
                    .pin = &switcher_pin,

                    .clock = &clk.state.time,

                    .type            = BUTTON_TOGGLE,
                    .bounce_delay_ms = 1000,

                    .on.toggle  = switcher_toggle,
                    .on.release = switcher_released,
                    .on.press   = switcher_pressed}));

/* Push button for led blinking */
pin_t pusher_pin = hw_pin(pusher, 0);
void  pusher_toggle(eer_t *trigger) {}
void  pusher_pressed(eer_t *trigger) {}
void  pusher_released(eer_t *trigger) {}
Button(pusher, _({.io  = &hw(gpio),
                  .pin = &pusher_pin,

                  .clock = &clk.state.time,

                  .type            = BUTTON_PUSH,
                  .bounce_delay_ms = 100,

                  .on.toggle  = pusher_toggle,
                  .on.release = pusher_released,
                  .on.press   = pusher_pressed}));


test(initial_state, long_push)
{
    // Event-loop
    loop(clk, switcher)
    {
        if (enabled) {
            use(pusher);

            apply(IO, led, _({.level = Button_is_pressed(&pusher)}));
        }
    }
}

// Another thread time dependent run test cases
result_t initial_state()
{
    test_assert(enabled == false, "Should disabled at first");
    test_assert(Button_is_pressed(&switcher) == false,
                "Switcher unpushed at first");
    test_assert(Button_is_pressed(&pusher) == false, "Pusher unpushed at first");
    test_assert(hw(gpio).get(&switcher_pin) == hw(gpio).get(&pusher_pin),
                "Pins should be equal %d = %d", hw(gpio).get(&switcher_pin),
                hw(gpio).get(&pusher_pin));
    test_assert(hw(gpio).get(&pusher_pin) == 0,
                "Pusher pin sholud be 0, not %d at start",
                hw(gpio).get(&pusher_pin));

    return OK;
}

result_t long_push() {
    usleep(1000);

    hw(gpio).on(&switcher_pin);
    usleep(1000);
    test_assert(hw(gpio).get(&pusher_pin) == 0,
                "Pusher pin sholud be 0, not %d after swither on",
                hw(gpio).get(&pusher_pin));
    test_assert(hw(gpio).get(&switcher_pin) != hw(gpio).get(&pusher_pin),
                "Pins should be different");

    usleep(500000);
    test_assert(hw(gpio).get(&switcher_pin) == 1,
                "Switcher pin sholud be 1, not %d after 500 ms button press",
                hw(gpio).get(&pusher_pin));
    test_assert(hw(gpio).get(&pusher_pin) == 0,
                "Pusher pin sholud be 0, not %d before swither off",
                hw(gpio).get(&pusher_pin));

    hw(gpio).off(&switcher_pin);
    test_assert(hw(gpio).get(&switcher_pin) == 0,
                "Switcher pin sholud be 0, not %d after 500 ms button release",
                hw(gpio).get(&pusher_pin));
    test_assert(enabled == false, "Should disabled after half sec");
    test_assert(hw(gpio).get(&pusher_pin) == 0,
                "Pusher pin sholud be 0, not %d after swither off",
                hw(gpio).get(&pusher_pin));

    hw(gpio).on(&switcher_pin);
    usleep(1100000);
    test_assert(enabled == true, "Should enabled after switch for 1 sec");
    test_assert(
        hw(gpio).get(&switcher_pin) == 1,
        "Switcher pin should be 1, not %d after second 1 sec button press",
        hw(gpio).get(&pusher_pin));

    hw(gpio).off(&switcher_pin);
    usleep(10);
    test_assert(hw(gpio).get(&pusher_pin) == 0,
                "Pusher pin sholud be 0, not %d before swither off",
                hw(gpio).get(&pusher_pin));
    test_assert(Button_is_pressed(&pusher) == false,
                "Pusher unpushed after switcher manipulation");

    hw(gpio).on(&pusher_pin);
    usleep(10000);
    test_assert(Button_is_pressed(&pusher) == false,
                "Pusher unpushed after 10 ms");
    usleep(110000);
    test_assert(Button_is_pressed(&pusher), "Button pushed after 100 ms");
    test_assert(hw(gpio).get(&led_pin), "Led is on");
    hw(gpio).off(&pusher_pin);
    usleep(50000);
    test_assert(hw(gpio).get(&led_pin) == false, "Led is off");
    test_assert(Button_is_pressed(&pusher) == false, "Button unpushed");

    hw(gpio).on(&switcher_pin);
    usleep(500000);
    hw(gpio).off(&switcher_pin);
    usleep(1100000);
    test_assert(enabled == true, "Should enabled after half sec");

    hw(gpio).on(&switcher_pin);
    usleep(10000);
    hw(gpio).off(&switcher_pin);
    usleep(10000);
    hw(gpio).on(&switcher_pin);
    usleep(10000);
    hw(gpio).off(&switcher_pin);
    sleep(1);
    test_assert(enabled == true, "Should enabled after half sec");

    hw(gpio).on(&switcher_pin);
    usleep(9000);
    hw(gpio).off(&switcher_pin);
    usleep(1000);
    hw(gpio).on(&switcher_pin);
    usleep(1000000);
    hw(gpio).off(&switcher_pin);
    usleep(1000);
    test_assert(enabled == false, "Should disabled after switch for 1 sec");

    hw(gpio).on(&pusher_pin);
    usleep(900);
    test_assert(Button_is_pressed(&pusher) == false,
                "Button unpushed after 10 ms");
    usleep(9000);
    test_assert(Button_is_pressed(&pusher) == false,
                "Button unpushed after 100 ms");
    test_assert(hw(gpio).get(&led_pin) == false, "Led is off");

    hw(gpio).off(&pusher_pin);
    usleep(50000);
    test_assert(hw(gpio).get(&led_pin) == false, "Led is off");
    test_assert(Button_is_pressed(&pusher) == false, "Button unpushed");

    return OK;
}
