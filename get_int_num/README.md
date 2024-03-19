# Get Interrupt Number
Implemention of the assembly code included in the BCM2835 ARM Peripherals documention on page 111.

To build/run, make sure you are running on a 32-bit ARM linux:
```
./get_int_num.sh
```
The program fetches some pending interrupts and asserts that they are the correct numbers.
