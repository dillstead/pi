#include <string.h>
#include "aux.h"
#include "gpio.h"
#include "uart.h"
#include "dmb.h"
#include "mmio.h"

void uart_init(void)
{
    uint32_t reg;

    // Map UART to GPIO pins
    // Set alt5 for pins 14 and 15, TXD1 and RXD1 respectively 
    reg = mmio_read(GPFSEL1);
    reg &= ~(7 << 12);
    reg |= 2 << 12;
    reg &= ~(7 << 15);
    reg |= 2 << 15;
    mmio_write(GPFSEL1, reg);
    
    // Disable pull up/down for pin 14 and 15
    mmio_write(GPPUD, 0);
    delay(150);
    mmio_write(GPPUDCLK0, (1 << 14) | (1 << 15));
    delay(150);
    // Write 0 to GPPUDCLK0 to make it take effect
    mmio_write(GPPUDCLK0, 0);

    // Enable the mini UART
    reg = mmio_read(AUX_ENABLES);
    reg |= 1;
    mmio_write(AUX_ENABLES, reg);
    // Disable auto flow control and disable receiver and transmitter 
    mmio_write(AUX_MU_CNTL_REG, 0);
    // Enable 8-bit mode
    mmio_write(AUX_MU_LCR_REG, 3);
    // Set RTS line high
    mmio_write(AUX_MU_MCR_REG, 0);
    // Disable receive and transmit interrupts
    mmio_write(AUX_MU_IER_REG, 0);
    // Clear both FIFOs
    mmio_write(AUX_MU_IIR_REG, 0x6);
    // Set baud rate to 115200
    mmio_write(AUX_MU_BAUD_REG, 270);   
    
    // Enable transmitter and receiver
    mmio_write(AUX_MU_CNTL_REG, 3);
    //mmio_write(AUX_MU_CNTL_REG, 2);
}

uint8_t uart_read_byte(void)
{
    uint32_t byte;

    dmb();
    
    // Wait for UART to become ready to receive.
    while (!(mmio_read(AUX_MU_LSR_REG) & 0x01));
    byte = mmio_read(AUX_MU_IO_REG);

    dmb();
    
    return (uint8_t) (byte & 0xFF);
}

void uart_write_byte(uint8_t val)
{
    dmb();
    
    // Wait for UART to become ready to transmit.
    while (!(mmio_read(AUX_MU_LSR_REG) & 0x20));
    mmio_write(AUX_MU_IO_REG, val);

    dmb();
    
}
