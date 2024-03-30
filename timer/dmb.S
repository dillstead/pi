.globl dmb

// BCM2835 ARM Peripherals 1.3 indicates that a data memory barrier must
// be used before the first write and last read of a peripheral to ensure
// correct memory ordering.        
dmb:
        mov	r12, #0
	mcr	p15, 0, r12, c7, c10, 5
	mov 	pc, lr
        
        
