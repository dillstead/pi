#ifndef LED_H
#define LED_H

#include <stdint.h>

void led_init(void);
void led_blink(int times, int32_t interval);
void led_on(void);
void led_off(void);
    
#endif
