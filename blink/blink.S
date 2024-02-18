.section ".text"

	// On-board LED
	// Pin 16
        mov	r1, #0x10000
	
begin:
        // Turn LED on
        // GPIO Output Clear 0
	ldr     r0, =0x20200028
        str     r1, [r0]

        mov     r3, #655360
delay_off:  
        subs    r3, r3, #1
        bne     delay_off

        // Turn LED off
        // GPIO Output Set 0
        ldr     r0, =0x2020001C	
        str     r1, [r0]

        mov     r3, #655360
delay_on:
        subs    r3, r3, #1
        bne     delay_on

        b       begin
