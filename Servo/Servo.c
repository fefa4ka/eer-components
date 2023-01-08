#include "Servo.h"
#include <eers.h>

static unsigned int g_seed = 31337;

static inline int fast_random()
{
    g_seed = (214013 * g_seed + 2531011);
    return (g_seed >> 16) & 0x7FFF;
}

WILL_MOUNT(Servo)
{
    state->io->out(state->pin);

    printf("MOUNT duty_cycle = %d, remain_time = %d\n", 1e3 + props->angle * 50 / 9, props->speed * 10e3 - state->duty_cycle);
    state->on_duty     = true;
    state->duty_cycle  = 1e3 + props->angle * 50 / 9;
    state->remain_time = props->speed * 10e3 - state->duty_cycle;
    printf("MOUNT duty_cycle = %d, remain_time = %d\n", 1e3 + props->angle * 50 / 9, props->speed * 10e3 - state->duty_cycle);

    state->release.method = (enum eer_result (*)(void *argument, void *trigger))self->release;
    state->release.argument = self;
}

SHOULD_UPDATE(Servo)
{
    if (props->angle != next_props->angle || props->speed != next_props->speed)
        return true;

    if (props->speed && state->on_duty == SERVO_STANDY)
        return true;

    return false;
}

WILL_UPDATE(Servo)
{
    state->duty_cycle  = 1e3 + props->angle * 50 / 9;
    state->remain_time = props->speed * 10e3 - state->duty_cycle;

    printf("UPDATE duty_cycle = %d, remain_time = %d\n", 1e3 + props->angle * 50 / 9, props->speed * 10e3 - state->duty_cycle);

    if (state->on_duty != SERVO_STANDY)
        self->stage.state.step = STAGE_RELEASED;

    if (props->speed == 0 && next_props->speed) {
        Servo_will_mount(self, next_props);
    }
}

RELEASE(Servo)
{
    if (props->speed) {
        if (state->on_duty == SERVO_ON_DUTY) {
            printf("Servo off\n");
            state->io->off(state->pin);
            state->on_duty = SERVO_ON_OFFSET;
        } else {
            printf("Servo on\n");
            state->io->on(state->pin);

            state->on_duty = SERVO_ON_DUTY;
        }

        // TODO: What happens if pass extra argument to func with less arguments
        struct eer_timer_isr timer_settings = {
            .ticks = (state->on_duty == SERVO_ON_DUTY ? state->duty_cycle : state->remain_time)
                     + (state->on_duty ? 0 : fast_random()),
            TIMER_EVENT_COMPARE,
        };

        struct eer_callback timer_callback = { self->release, self };

        state->timer->isr.enable(&timer_settings, &timer_callback);
        printf("Set on %d timer %d, duty_cycle = %d,  speed = %d, remain_time =%d\n", state->on_duty, timer_settings.ticks, state->duty_cycle, props->speed, state->remain_time);
    } else {
        printf("Servo standby\n");
        state->io->off(state->pin);
        state->on_duty = SERVO_STANDY;
    }
}


DID_MOUNT_SKIP(Servo);
DID_UPDATE_SKIP(Servo);
