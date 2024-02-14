# Echo
Echoes key strokes sent to the Raspberry Pi in a terminal

To build:
```
make
cp kernel7.img /media/<your SD Card>/
```

In order to sent keystrokes to the Pi, you'll need a USB to Serial TTL cable to connect your desktop/laptop to the Pi.  You can find the part and instructions 
for setting it up [here](https://www.adafruit.com/product/954).

Once that's setup and working, insert the card into the Pi, and power it up.  The green LED should turn on when it's ready to receive serial input.  Next, 
run ```send``` with the USB device connected to the Pi:
```
sudo ./send /dev/ttyUSB0 
```
At this point, any character you type into the terminal will be sent to the Pi, sent back to the terminal, and displayed on the screen.  Typing ```'q'``` will quit and
the LED on the Pi will turn off.
