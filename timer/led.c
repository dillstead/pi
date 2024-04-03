#include "gpio.h"
#include "mmio.h"
#include "dmb.h"
#include "led.h"

void led_init(void)
{
    uint32_t selector;

    selector = mmio_read(GPFSEL1);
    selector &= ~(7 << 18);
    selector |= 1 << 18;
    mmio_write(GPFSEL1, selector);
}

void led_on(void)
{
    dmb();
    
    mmio_write(GPCLR0, 1 << 16);

    dmb();
}

void led_off(void)
{
    dmb();
    
    mmio_write(GPSET0, 1 << 16);

    dmb();
}
