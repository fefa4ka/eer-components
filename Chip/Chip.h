#pragma once

#include <eer.h>
#include <sys.h>

#define Chip(instance, props, state) eer_define(Chip, instance, _(props), _(state))

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

    struct {
        eer_sys_clk_source_t cpu;
        eer_sys_rtc_source_t rtc;
    } frequency;

    eer_sys_handler_t *sys;
} Chip_state_t;

eer_header(Chip);
