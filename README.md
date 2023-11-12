# ModularControlPanel
Mechanics, PCB and Software for my modular control panel that is used with my cnc machine driven by LinuxCnc


This bboard delivers:
- 12 Key inputs
- 8 LED outputs driven by 2 boards of these: https://aliexpress.com/item/1005003772569293.html
- 3 ADC inputs
- directly routed inputs for connection to the control cabinet (connected via 15 Pin cable):
  - POWER_KEY or the standby Variant POWER_KEY_MAN, both enable POWER_EN 
  - START_KEY, STOP_KEY, BREAK_KEY
  - input for the ESTOP_LED
  - ESTOP connector
  - 3 Reserve lines
- I2C extension connectors for 3.3 and 5V I2C
- ws28 smart LED connector
- one connector for a MIST System (see https://github.com/TheBlueManCoding/WemosMosfet)

all switches are connected by:
- https://aliexpress.com/item/4000502453714.html
- https://aliexpress.com/item/1005001317075834.html
