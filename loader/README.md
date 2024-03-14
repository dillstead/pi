# Loader
Loads and runs images on the Raspberry Pi.

To build:
```
make
cp kernel7.img /media/<your SD Card>/
```

In order to load images onto the Pi, you'll need a USB to Serial TTL cable to connect your desktop/laptop to the Pi.  You can find the part and instructions 
for setting it up [here](https://www.adafruit.com/product/954).

Once that's setup and working, insert the card into the Pi, and power it up.  The green LED should blink twice, once when the loader itself has finished loading and another to indicate that the loader is ready to receive the image.

Next, run ```upload``` with the cable connected to the Pi:
```
sudo ./upload /dev/ttyUSB0 <Intel HEX image> 
```
Once the image is loaded, the loader will transfer control to the loaded image and it will begin running.

If you pass the ```-i``` option to ```upload```, it will pause waiting for a keystroke to begin running the image.

If the image supports it, command line arguments can also be passed to it by addding them to the end of the ```upload``` command line.

To show how this works, the project includes a sample image which blinks the LED:
```
sudo ./upload /dev/ttyUSB0 ./blink.hex
```
With no arguments, ```blink``` blinks the LED 5 times, with each blink (and delay between blinks) taking 10485760 units.

If you want to change the number of times or delay you can do so by adding the parameters to the ```upload``` command line.  To blink the LED 10 times:

```
sudo ./upload /dev/ttyUSB0 ./blink.hex blink 10
```
To blink the LED 15 times with delay of 100000 units:
```
sudo ./upload /dev/ttyUSB0 ./blink.hex blink 10 100000
```
It's important to note that the format of the command line argument follows that of C's ```argv``` with the 1st argument being the name of the executable.  In this case, ```blink```.

Also note that the loader only supports the [Intel HEX format](https://en.wikipedia.org/wiki/Intel_HEX) for images..



