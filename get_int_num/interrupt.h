#ifndef __INTERRUPT_H
#define __INTERRUPT_H

// Interrupts range from 0 to MAX_INTERRUPTS - 1: 
//   00...63 correspond to IRQ 0 - IRQ 63 
//   64...MAX_INTERRUPTS - 1 correspond to the ARM peripherals table
// See 7 Interrupts in the BCM2835 ARM Peripherals manual
#define MAX_INTERRUPTS 72
// Base interrupt number for each register.
#define INT_IRQ1_BASE  0
#define INT_IRQ2_BASE  32
#define INT_IRQ0_BASE  64

#endif
