# Button

`Button` components provides an abstraction layer for using digital buttons. It allows for the configuration of button types (push-pullup, push, toggle-pullup, toggle) as well as setting callbacks for button events such as press, release, and toggle.

## Usage

you can create an instance of the Button component using the `Button(instance, props)` macro, where `enter` is the name you want to give to the instance and passed props is an instance of the `Button_props_t` structure containing the properties for the component.

```c
Clock(clk, &hw(timer), TIMESTAMP);

pin_t enter_pin = hw_pin(ENTER_PORT, ENTER_PIN);
Button(enter, _({
    .io  = &hw(gpio),
    .pin = &belt_enter_pin,

    .clock = &clk.state.time,

    .type            = BUTTON_PUSH_PULLUP,
    .bounce_delay_ms = 100,
}));

/* Inside event-loop
 * Just initialize and let live */
 loop(enter) { ... }
 ignite(enter);

 /* Loop while enter component should update */
 with(enter) { ... }

 /* Utilize current lifecycle */
 use(enter);
```

In this example, the `enter` button is configured to be a push-pullup type button, with a bounce delay of 100 milliseconds. The `.io` field is set to the gpio HAL, and the `.pin` field is set to the specific pin that the button is connected to. The `.clock` field is set to the clk component's state's time field, which is used for timestamping button events for bounce filtering.

The `Button_is_pressed(enter)` macro can be used to check the current state of the button, whether it is pressed or not, which will return a boolean value indicating whether the button is currently pressed.

The `on` field is used to set callbacks for button events such as `press`, `release`, and `toggle`. These callbacks will be invoked when the corresponding event occurs.

## Dependencies

`Button` component uses the `in()`, `pullup()` and `get()` methods of the `gpio` HAL and the `time` field of [Clock](../Clock) component.
