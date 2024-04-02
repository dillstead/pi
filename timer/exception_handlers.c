#include "debug_log.h"

void __attribute__ ((interrupt ("ABORT"))) reset_handler(void)
{
    log_str("reset");
    while(1);
}

void __attribute__ ((interrupt ("ABORT"))) prefetch_abort_handler(void)
{
    log_str("prefetch abort");
    while(1);
}

void __attribute__ ((interrupt ("ABORT"))) data_abort_handler(void)
{
    log_str("data_abort");
    while(1);
}

void __attribute__ ((interrupt ("UNDEF"))) undefined_instruction_handler(void)
{
    log_str("undefined instruction");
    while(1);
}

void __attribute__ ((interrupt ("SWI"))) software_interrupt_handler(void)
{
    log_str("software interrupt");
    while(1);
}

void __attribute__ ((interrupt ("FIQ"))) fast_interrupt_handler(void)
{
    log_str("fast_interrupt");
    while(1);
}
