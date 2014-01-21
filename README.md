arduino-rgb-i2c
===============

Simple arduino sketch for controlling 2 RGB strips using PWM via the I2C protocol.


## Why does this exist?

I currently have two RGB LED strips which I wanted full control of via a Raspberry Pi. As these strips are controlled using PWM and the Raspberry Pi only has a single PWM output the easiest solution was to connect up an Arduino via I2C (without having to buy more components!) 

Apologies for the state of the code, it was knocked together very quickly using resources from the following URLs

http://arduino.cc/en/Tutorial/ColorCrossfader

http://blog.oscarliang.net/raspberry-pi-arduino-connected-i2c/


## How to use

As I am controlling this via node.js the examples are provided in this language.

The Arduino expects 4 bytes per request. 
First byte to contain integer 1-4 of which method to use
Second byte the red value (0 is on, 255 is off in the case of my setup)
Third byte the green value
Forth byte the blue value

### Install

```
npm install i2c
```

### Usage

```
var i2c = require('i2c');

// I2C address to use
var address = 0x04;
var wire = new i2c(address, {device: '/dev/i2c-0'});

// define the methods which can be used for changing the colours
var methods = {
  STRIP1_SET: 1,   // Instantly change strip 1 
  STRIP1_FADE: 2,  // Fade strip to colour on strip 1
  STRIP2_SET: 3,   // Instantly change strip 2
  STRIP2_FADE: 4   // Fade strip to colour on strip 2
}
```

Set strip 1 to white

```
wire.writeBytes(methods.STRIP1_SET, [0, 0, 0], function(err) {
  console.log(err);
});
```

Fade strip 1 to red
```
wire.writeBytes(methods.STRIP1_FADE, [0, 255, 255], function(err) {
  console.log(err);
});
```

Fade strip 2 to green
```
wire.writeBytes(methods.STRIP2_FADE, [255, 0, 255], function(err) {
  console.log(err);
});
```
