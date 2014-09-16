rpi-sleep-circuit
=================

# Raspberry Pi Power Regulator with Sleep Mode

This code is intended to be used in the -controlled 
 described in [this article](http://leviathan.godigi.fi/24-raspberry-pi-shutdown-circuit-with-sleep-mode). 
Using the code the power regulator can be commanded to shut down power from Raspberry Pi 
permanently or temporarily. This allows your Raspberry Pi to sleep for specified amount of time
to eg. save battery life. This is useful especially in battery-operated projects, such as
time-lapse photography.

## Parts used

* [Adafruit Trinket](http://www.adafruit.com/product/1501)
* [Pololu power regulator](http://www.pololu.com/product/2866)
* [Pololu Pushbutton Power Switch SV](http://www.pololu.com/product/750)
* PNP Transistor

## Schematics

![Schematics](http://leviathan.godigi.fi/images/hardware/sleep_circuit/Power.jpg "Power regulator schematics")

## Building the Trinket code

The Trinket code depends on [TinyWireS library](https://github.com/rambo/TinyWire/tree/master/TinyWireS).
To build the code download and extract TinyWireS library into your **sketchbook/libraries** directory.

