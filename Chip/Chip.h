#pragma once

#include <eer.h>

#define Chip(instance)            eer(Chip, instance)
#define Chip_new(instance, state) eer_withstate(Chip, instance, _(state))
#define Chip_level(instance)      eer_state(Chip, instance, level)

typedef struct {
    enum eer_sys_mode mode;

    struct {
        void (*boot)(eer_t *instance);
        void (*ready)(eer_t *instance);
        void (*sleep)(eer_t *instance);
        void (*wake)(eer_t *instance);
        void (*reboot)(eer_t *instance);
        void (*shutdown)(eer_t *instance);
    } on;
} Chip_props_t;

typedef struct {
    enum eer_sys_mode mode;

    enum eer_hw_sys_clk_source cpu_frequency;
    enum eer_hw_sys_rtc_source rtc_frequency;

    eer_sys_handler_t *sys;
} Chip_state_t;

eer_header(Chip);
