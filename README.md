# GoNtroller

This project is based on an ESP8266 (NodeMCU).

NTC is a simple unit to read an NTC sensor, here in a voltage divider with a 47kOhm to ground
Sensor in use is a Sourcing Map 100K Temperature Sensor Probe NTC.
A,B and C have only been roughly calculated as my reference temperature probe was down on battery.

Relay is a simple unit allowing to constantly loop through the setter as switching only gets done
when the relay state is to change and the last changing time is stored and can be called up.

LED uses "gostcai pre-wired round LEDs light, metal round 2 colors signal light, waterproof indicator
light, common anode, 8mm, 3‑6V, for vehicle modification(Red Green)", having a common positive 

Altered version of the ModBusRelay unit to be able to switch battery charger etc. via a
ILS - Modbus RTU 4 Channel Relay Module and a Youmile TTL to RS485 Adapter Module 3.3V 5V.

The Octopus unit is containing parts of the Agile routines. Not sure yet if I complete these routines or remove them.
