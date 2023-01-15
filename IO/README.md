# IO

`IO` components provide an abstraction layer for using HAL `gpio`-based digital pins. It allows configuration of pin modes (input/output) as well as toggling/reading pins. It also provides callbacks for pin events such as change, low, and high.

## Usage

Declartaion and usage of IO component in output mode:

```c
bool  indicator_value = true;
pin_t indicator_pin   = hw_pin(B, 12);
void  indicator_toggle(eer_t *indicator) { indicator_value = !indicator_value; }
IO_new(indicator, _({
    .io = &hw(gpio),
    .pin = &indicator_pin,
    .mode = IO_OUTPUT,
    .on = { .change = indicator_toggle }
}));


/* Apply new properties inside event-loop */
apply(IO, indicator, _({
    .level = indicator_value,
}));

```

In input mode:

```c
IO_new(sensor, _({
    .io   = &hw(gpio),
    .pin  = &sensor_pin,
    .mode = IO_INPUT,
    .on   = {.change = sensor_read},
}));


/* Inside event-loop
 * Just initialize and let live */
loop(sensor) { ... }
ignite(sensor);

/* Loop while sensor component should update */
with(sensor) { ... }

/* Utilize current lifecycle */
use(sensor);

```

## Dependencies

IO component based on `gpio` HAL:

```c
typedef struct eer_gpio_handler {
    void (*in)(void *pin);
    void (*out)(void *pin);
    void (*on)(void *pin);
    void (*off)(void *pin);
    void (*flip)(void *pin);
    void (*pullup)(void *pin);
    void (*pulldown)(void *pin);
    bool (*get)(void *pin);

    eer_isr_handler_t isr;
} eer_gpio_handler_t;
```

These methods are used: `in()`, `out()`, `on()`, `off()` and `get()`.

## Props Description

| Name  | Type | Description                                          |
| ----- | ---- | ---------------------------------------------------- |
| level | enum | Desired level of the pin (high/low) for output mode. |

## State Description

| Name  | Type               | Size | Description                                         |
| ----- | ------------------ | ---- | --------------------------------------------------- |
| level | bool               | 1B   | Actual level of the pin (high/low).                 |
| io    | eer_gpio_handler_t | 4B   | HAL handlers for GPIO.                              |
| pin   | void \*            | 4B   | Pointer to pin object that could be handled by HAL. |
| mode  | enum               | 1B   | Pin operation mode.                                 |
| on    | struct on          | 12B  | Callbacks for IO events                             |

-   [ ] Don't store callbacks in state, instead store pointer to `struct on`

## Applications

`IO` components can be used in various applications, for example for controlling:

-   [Basic Example](https://github.com/fefa4ka/eer-apps/tree/master/apps/basic) - eer application with sensor is implemented by reading the gpio input signal and indicating by adjusting the gpio output signal
-   [Button](https://github.com/fefa4ka/eer-components/tree/master/Button) - uses the IO component to read the state of the button when it is pressed or released
-   [Bitbang](https://github.com/fefa4ka/eer-components/tree/master/Bitbang) - to toggle/read the state of a bunch pins

## Unit Tests

-   [IO_test.c](https://github.com/fefa4ka/eer-components/blob/master/IO/IO_test.c) — the sensor is implemented by reading the gpio input signal and indicating by adjusting the gpio output signal, and the callbacks are invoked on gpio change.

-   IO_test_mock.c — mock implementation of the gpio HAL.

#### Input Mode

-   [x] Initialize IO component in `input` mode.
-   [x] Read pin value.
-   [x] Verify that pin value is the same as expected.
-   [x] Invoke callback on change.
-   [x] Verify that callback has been invoked.

#### Output Mode

-   [x] Initialize IO component in `output` mode.
-   [x] Toggle pin.
-   [x] Verify that pin value is the same as expected.
-   [ ] Invoke callback on change.
-   [ ] Verify that callback has been invoked.
