#ifndef SYSTEM_TIMER_H
#define SYSTEM_TIMER_H

#include <inttypes.h>

void system_timer_init(void);
void system_timer_tick(uint32_t tick, void (*cb)(void *), void *data);

#endif
