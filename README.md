# nspire-usb-uart-shenanigans
## Overview
This sketch is a simple tech demo of TI-Nspire CX (and, by extension, TI-84 Plus CE) communication to an Arduino-like board by using a USB-to-UART breakout with simple code that [simulating the single necessary command to make the calculator think a TI-Innovator Hub is attached](https://www.cemetech.net/forum/viewtopic.php?t=19697). This demo can control the state of a LED and read sensor data from an Adafruit BMP085 or BMP180 breakout. Serial communications through the breakout must take place at 115200 baud, since TI-Nspire OS hardcodes this speed. Disconnect the USB-to-UART breakout when programming your board. 

## Hardware
There's three basic hardware components necessary to get this running, aside from a suitably modern calculator (TI-Nspire CX or CX II with OS 4.3 or later, or TI-84 Plus CE):
* Arduino-like board. Some tweaking may be necessary on some boards (Leonardo, for example, exposes UART pins on Serial1 and not Serial). 
* USB-to-UART breakout. See the section on USB-to-UART Chipset Support for more information on what you can use. 
* BMP085 or BMP180 breakout. I use the Adafruit BMP180 breakout, which is discontinued. Modifying code to use other sensors as a data source isn't too difficult. This sketch uses the Adafruit BMP085 Unified library, which you'll need to install from the Arduino IDE Library Manager before compiling.

### USB-to-UART Chipset Support
Chipset support varies by calculator model. The TI-Nspire CX and TI-84 Plus CE support the Silabs CP2104 chipset, as this chipset is used in the first-party TI-Innovator Hub. The TI-Nspire CX II supports the CH340G chipset in addition to those supported by the TI-Nspire CX. The TI-Nspire CX does *not* support the CH340. The Silabs CP2102 is expected to work on all models, and FTDI chipsets are untested. FTDI may work on the TI-Nspire CX II. Any combinations outside what is listed here is untested. If you have chipset testing results, tell me on [Cemetech](https://www.cemetech.net/forum/viewtopic.php?t=19697)!

To connect this to the calculator, you need a USB On-The-Go cable or adapter. A USB-A female to female adapter will not inform the calculator that it is a USB host, and neither power nor communications occur. I'm using a USB Mini-B male to USB A female adapter I got on Amazon for a few dollars; it's a generic product from China, so it shouldn't be too hard to find. Note that whatever is available these days do not use a proper USB Mini-A connector, but instead use the commodity USB Mini-B connector with the OTG ID pin correctly grounded. This can be a problem for some appliances that have actual USB Mini-A sockets, but the calculators have combination USB Mini-AB sockets and will accept both Mini-A and Mini-B.  

## Wiring
Connect your USB-to-UART breakout, *after programming your Arduino*, as follows: 
* Breakout 5V to Arduino VIN
* Breakout GND to Arduino GND
* Breakout TXD to Arduino RXD (D0)
* Breakout RXD to Arduino TXD (D1)

Connect the LED to pin 5, or redefine the pin number (`LED_PIN` in the code). Be sure to use a decent current-limiting resistor as not much power is available over USB On-The-Go. 

Attach BMP085/BMP180 like any other I2C device; SDA to A4 and SCL to A5 on the Arduino Nano. These connections vary by board. Connect VIN to Arduino VIN, and GND to Arduino GND. 

A debug output is available as RXD on pin 7, TXD on pin 8. Connect RXD of a second USB-to-UART breakout to pin 8 to read the debug output. The debug output is never used to receive data, so connecting RXD is not necessary provided internal pullups are doing their job. 

## Commands
There are a few basic commands, sent with `Send "commandname"` and `Get varname`. Newlines are stripped from the example responses given.  
* `ISTI`: always responds with `TISTEM`.
* `SET LED 0` and `SET LED 1`: sets the state of the LED that is attached by default to D5. Returns the new LED state, fetch it with `Get`. 
* `GET LED`: retrieves the state of the LED, returning 0 if off and 1 if on.
* `GET BAR`: retrieves air pressure from the BMP180 in millibars.
* `GET THM`: retrieves air temperature from the BMP180 in degrees Celsius.

Note that when running `Get` on the calculator, it returns only the most recent line sent. For example, if you run the `GET LED` command followed by `GET BAR`, then perform a `Get` command on the calculator, you retrieve the barometer reading. Thus, you don't need to fetch the LED state after setting it. 
