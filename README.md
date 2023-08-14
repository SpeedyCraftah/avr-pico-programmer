# avr-pico-programmer
A programmer for the AVR architecture microcontroller, specifically the ATTiny84A by Microchip, for the Raspberry Pi Pico.
- Turns the Raspberry Pi Pico into an AVR programmer via SPI programming mode.
- Instructions are streamed using the serial port to the Raspberry Pi Pico via USB which handles the rest.
- Verifies flashed pages after programming to confirm a successful flash.

## Problem
- Initially, the ATTiny did not respond to any of my SPI programming commands, eventually after a bit of back and fourth with Microchip support, we figured out that the ATTiny does not support programming via 3.3V which the Raspberry Pi Pico uses for GPIO logic; all SPI pins must use a logic voltage of 5V, simply supplying Vcc with 5V is not enough.
  - Luckily I managed to bypass the requirement for a 5V programming device by using a 3V3 5V logic converter which works by converting 3.3V signals to 5V signals, which effectively acts as a translator between the Pico and the ATTiny (PS. Any 3.3V to 5V logic converter PCB with support for at least 4 ports (MISO, MOSI, SCK, RESET) should work).

## How to use

### Flashing software
- You can either use the released `.uf2` binary file and drop it onto the Raspberry Pi Pico which has the compiled source code, or compile it yourself with the [Raspberry Pi Pico SDK for C/C++](https://www.raspberrypi.com/documentation/microcontrollers/c_sdk.html).

### Physical setup
- You simply need to connect all the SPI and other relavent pins from the Raspberry Pi Pico to the logic converter, and from the logic converter to the ATTiny.
- I created a schematic which demonstrates this with a Raspberry Pi Pico and the ATTiny84A, if you're using another AVR controller you should be able to infer which pins to connect from the relavent AVR datasheet:
  ![image](https://github.com/SpeedyCraftah/avr-pico-programmer/assets/45142584/598428f8-867c-4d9e-a480-de1b2b60a3f2)
  In case the schematic is unclear: VBUS-HV&VCC | GND-GND&GND | 3V3_OUT-LV | GP19-LV3-HV3-PA6 | GP18-LV2-HV2-PA4 | GP17-LV1-HV1-PB3 | GP16-LV4-HV4-PA5

### Programming
- The tool is designed so that you can also program firmware using a serial port client, which you can do so by using a tool such as PuTTY and pasting in the hex of each individual byte of the firmware, each being followed by a space (" ") - after all hex bytes have been sent, press enter and the programmer will begin erasing and flashing the AVR controller.
- Alternatively, I have written a client for this in Node.js which takes in a binary file of the firmware and automatically interfaces with the Pico programmer:
![image](https://github.com/SpeedyCraftah/avr-pico-programmer/assets/45142584/97871f71-8e47-403e-9a21-1378b84e81be)

## Motivation
- A while back I got hold of an ATTiny84A but had no idea how to program it, later realising I needed an Arduino or a dedicated AVR programmer, hence decided to try to program it via a Raspberry Pi Pico.
- I started looking through the [ATTiny datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny24A-44A-84A-DataSheet-DS40002269A.pdf) and attempted to program it according to the serial programming section.
- It served as a fun project as I was interested in the AVR architecture and wanted to challenge myself by writing a programmer for it from scratch.
