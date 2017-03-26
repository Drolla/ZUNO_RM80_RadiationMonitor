# Z-Wave enabled Radiation, Temperature and Humidity Monitor with LCD Display

## Table of Contents

* [Description](#description)
* [Hardware required](#hardware-required)
* [Schematics, PCB layout](#schematics-pcb-layout)
* [Photos](#photos)
* [Release history](#release-history)
* [License](#license)


## Description

The project requires the following hardware components:
  * Z-Uno (Z-Wave) development board from Z-Wave.Me
  * Radiation monitor - RM-60/70/80 radiation monitor from AWARE Electronics. Connected to the external interrupt ZUNO_EXT_ZEROX.
  * Temperature/humidity sensor - DHT22 temp/hum sensor, 10kOhm resistor. Controlled via a single pin using the ZUNO_DHT module.
  * LCD display - Adafruit Sharp 1.3" Memory LCD Display. Controlled via the SPI interface.
  * Control buttons - 2 simple press buttons connected to 2 DIOs.

In addition to the functions related to the above hardware components, the application provides the following features:
  * Radiation data, temperature and humidity can be reported every minutes via Z-Wave
  * Radiation events can be indicated via the Z-Uno user LED
  * Status information can be reported via the USB attached serial interface
  * Configuration options: Z-Wave reporting, LED activity and status reporting via serial interface can individually be enabled or disabled. Configurations are stored in the EEPROM and restored after power cycling.


## Hardware required

  * Z-Uno board
  * RM-60/70/80 radiation monitor from AWARE Electronics
  * Sensor DHT11
  * LCD display - Adafruit Sharp 1.3" Memory LCD Display
  * 1 resistor1 10kOhm
  * 2 press buttons
  * Breadboard, wires


-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

## Schematics, PCB layout

Schematic:

![Schematic](https://github.com/Drolla/ZUNO_RM80_RadiationMonitor/wiki/KiCad_Schematic.png)

PCB layout:

![PCB Layout](https://github.com/Drolla/ZUNO_RM80_RadiationMonitor/wiki/KiCad_Layout.png)

Breadboard:

![Breadboard Drawing](https://github.com/Drolla/ZUNO_RM80_RadiationMonitor/wiki/Fritzing_Breadboard.png)


-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

## Photos

Breadboard:

![Breadboard](https://github.com/Drolla/ZUNO_RM80_RadiationMonitor/wiki/FullCenter_Overview.JPG)

Some display pictures:

![Breadboard](https://github.com/Drolla/ZUNO_RM80_RadiationMonitor/wiki/Display_AllSmall.jpg)


-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

## Release history

* 0.1
    * Initial version, verified with Z-Uno firmware revision 2.0.7. Since no external interrupts are supported in this firmware revision, the RM-80 radiation monitor has to be connected to the serial interface Serial0.
* 0.2
    * Version adapted for Z-Uno firmware version 2.0.9 that supports external interrupts. The RM-80 radiation monitor has to be connected to the external interrupt 'ZUNO_EXT_ZEROX'.
	 * Only valid mean/average values are reported via Z-Wave (e.g. 1-hour average value will not be reported the 1st hour after booting)


-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

## License

This project is licensed under the MIT License - see the _LICENSE_ file for details.