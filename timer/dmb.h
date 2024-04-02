#ifndef DMB_H
#define DMB_H

// BCM2835 ARM Peripherals 1.3 indicates that a data memory barrier must
// be used before the first write and last read of a peripheral to ensure
// correct memory ordering.        
static inline void dmb(void)
{
    uint32_t zero = 0;
    
    asm volatile("mcr p15, 0, %0, c7, c10, 5" : : "r"(zero));
}

#endif

