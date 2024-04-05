#include "interrupts.h"
#include "dmb.h"
#include "mmio.h"
#include "system_timer.h"
#include "led.h"

// According to https://embedded-xinu.readthedocs.io/en/latest/arm/rpi/BCM2835-System-Timer.html
// the system timer can only use the system timer compare register 1 or 3 which correspond to
// IRQ line 1 and 3 respectively. 
#define INT_SYS_TIMER  1

#define SYS_TIMER_BASE  (PERIPHERAL_BASE + 0x3000)
#define SYS_TIMER_CTRL  (SYS_TIMER_BASE + 0x0000)
#define SYS_TIMER_LO    (SYS_TIMER_BASE + 0x0004)
#define SYS_TIMER_HI    (SYS_TIMER_BASE + 0x0008)
#define SYS_TIMER_COMP0 (SYS_TIMER_BASE + 0x000C)
#define SYS_TIMER_COMP1 (SYS_TIMER_BASE + 0x0010)
#define SYS_TIMER_COMP2 (SYS_TIMER_BASE + 0x0014)
#define SYS_TIMER_COMP3 (SYS_TIMER_BASE + 0x0018)

static uint32_t _tick;
static void (*_cb)(void *);
static void *_data;

static void set(void)
{
    uint32_t lo;
    
    dmb();

    lo = mmio_read(SYS_TIMER_LO);
    mmio_write(SYS_TIMER_COMP1, lo + _tick);

    dmb();
}

static void process_interrupt(void)
{
    _cb(_data);
    set();
}

static void clear_interrupt(void)
{
    uint32_t control;

    dmb();
    
    control = mmio_read(SYS_TIMER_CTRL);
    // Clear the match status bit for compare register 1 to clear
    // the interrupt request line.
    control |= 2;
    mmio_write(SYS_TIMER_CTRL, control);
    
    dmb();
}

void system_timer_init(uint32_t tick, void (*cb)(void *data), void *data)
{
    register_process_interrupt(INT_SYS_TIMER, process_interrupt);
    register_clear_interrupt(INT_SYS_TIMER, clear_interrupt);
    _tick = tick;
    _cb = cb;
    _data = data;
    set();
}
