#pragma once

#include <eer.h>

#define IO(instance) eer(IO, instance)
#define IO_new(instance, state) eer_withstate(IO, instance, _(state))

typedef struct {
  enum { IO_LOW, IO_HIGH } level;
} IO_props_t;

typedef struct {
  bool level;

  eer_io_handler_t *io;
  void *pin;

  enum { IO_OUTPUT, IO_INPUT } mode;

  struct {
    void (*change)(eer_t *instance);
    void (*low)(eer_t *instance);
    void (*high)(eer_t *instance);
  } on;
} IO_state_t;

eer_header(IO);
