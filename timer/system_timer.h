#ifndef __SYSTEM_TIMER_H
#define __SYSTEM_TIMER_H

#include <inttypes.h>

void system_timer_init(void);
void system_timer_set(uint32_t usecs);

#endif
