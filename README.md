# Z-Wave enabled Radiation, Temperature and Humidity Monitor with LCD Display

## Table of Contents

## Description

The project requires the following hardware components:
  * Radiation monitor - RM-60/70/80 radiation monitor from AWARE Electronics. Connected to the serial interface Serial0.
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


## Schematics, PCB layout

Schematic:

![Schematic](https://github.com/Drolla/ZUNO_RM80_RadiationMonitor/wiki/KiCad_Schematic.png)

PCB layout:

![PCB Layout](https://github.com/Drolla/ZUNO_RM80_RadiationMonitor/wiki/KiCad_Layout.png)

Breadboard:

![Breadboard Drawing](https://github.com/Drolla/ZUNO_RM80_RadiationMonitor/wiki/Fritzing_Breadboard.png)


## Photos

Breadboard:

![Breadboard](https://github.com/Drolla/ZUNO_RM80_RadiationMonitor/wiki/FullCenter_Overview.JPG)

Some display pictures:

![Breadboard](https://github.com/Drolla/ZUNO_RM80_RadiationMonitor/wiki/Display_AllSmall.jpg)
