#pragma once

#include <eer.h>
#include <gpio.h>

#define Servo(instance, props, state)                                          \
    eer_define(Servo, instance, _(props), _(state))

typedef struct {
    uint8_t  speed; /* PWM period in ms disabled if 0 */
    uint16_t angle; /* Servo position */
} Servo_props_t;

typedef struct {
    eer_gpio_handler_t  *io;
    void                *pin;
    eer_timer_handler_t *timer;
    struct eer_callback  release;

    uint16_t duty_cycle;
    uint16_t remain_time;
    enum { SERVO_STANDY, SERVO_ON_DUTY, SERVO_ON_OFFSET } on_duty;
} Servo_state_t;


eer_header(Servo);
