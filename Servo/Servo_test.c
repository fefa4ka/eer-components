#include <Clock.h>
#include <Scheduler.h>
#include <Servo.h>
#include <eers.h>
#include <unit.h>


Clock(clk, &hw(timer), 0);
Scheduler(scheduler, 15, _({.timer = &hw(timer)}));
Scheduler_timer_handler(scheduler);


/* Motors */
bool is_motors_enabled = true;
#define SERVO_SPEED 10
#define Servo_setup(instance, pin_definition)                                  \
    pin_t instance##_pin = _(pin_definition);                                  \
    Servo(instance, {0},                                                       \
          _({                                                                  \
              .io    = &hw(gpio),                                              \
              .pin   = &instance##_pin,                                        \
              .timer = &scheduler_timer_handler,                               \
          }))
#define Servo_drive(instance, servo_angle)                                     \
    apply(Servo, instance,                                                     \
          _({                                                                  \
              .speed = is_motors_enabled ? SERVO_SPEED : 0,                    \
              .angle = servo_angle,                                            \
          }))

uint16_t thrust = 0;
Servo_setup(engine, hw_pin(A, 0));

test(init)
{
    // Event-loop
    loop(clk, scheduler) { Servo_drive(engine, thrust); }
}

result_t init() { sleep(1); return OK; }

