MeterClock
==========

Arduino code for Dan's Meter Clock.
http://maniacallabs.com/2014/07/08/meter-clock-pt1

**This project is still 'In Progress' and code is for reference only**

Meter Connection
----------------
See this image for details of the meter control circuit for the Seconds and Minutes meters: https://github.com/ManiacalLabs/MeterClock/blob/master/MeterTransistorCircuitSchematic.jpg

Note that the resistor between 5V and the meter is actually 2 1/4 watt 100 Ohm resistors in parallel for an effective resistance of 50 Ohms. The two in parallel was necessary as 5V x 0.06A = 0.3W (more than 0.25 that a single 1/4W resistor can handle safely).

Pin Connections:
----------------
-1uF Cap between 5V and GND

-Digital Pin 13 -> LPD8806 Strip Clock

-Digital Pin 11 -> LPD8806 Strip Data

-Digital Pin 10 -> 'Minutes' meter transistor Base (through 1K resisitor)

-Digital Pin 9 -> 'Hours' meter (through 330 resisitor)

-Digital Pin 6 -> 'Seconds' meter transistor Base (through 1K resistor)

-Analog Pin 5 -> ChronoDot SCL pin

-Analog Pin 4 -> ChronoDot SDA pin

