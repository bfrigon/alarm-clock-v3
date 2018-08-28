Motherboard pinout
==================


J1 : DC Jack
------------

Pin | Name
----|------------------------
1   | VIN
2   | GND
3   | GND



J10 : PSU board
---------------

Pin | Name    | Pin | Name
----|---------|-----|---------------------------
1   | VIN     | 2   | VIN
3   | +5V     | 4   | +5V
5   | GND     | 6   | GND



J3 : PSU status
-------------------------

Pin | Name
----|------------------------
1   | Low battery signal
2   | On battery



J9 : Daughter board I/O
-----------------------

Pin | Name                   | Pin | Name
----|------------------------|-----|---------------------------
1   | MISO                   | 2   | CMOSI (3.3v)
3   | CSCK (3.3v)            | 4   | XDCS (3.3v)
5   | DREQ                   | 6   | CCS (3.3v)
7   | SDA                    | 8   | SCL
9   | Keypad interrupt       | 10  | RGB pixels data (clock)
11  | RGB pixels data (lamp) | 12  | Codec reset (3.3v)
13  | Amplifier shutdown     | 14  | RGB pixels shutdown
15  | DVDD (3.3v LDO)        | 16  | Alarm switch



J3 : Daughter board power
-------------------------

Pin | Name
----|------------------------
1   | +5V
2   | +5V
3   | GND
4   | GND



J8 : Front panel
----------------

Pin | Name
----|------------------------
1   | +5V
2   | SDA
3   | SCL
4   | OLED reset
5   | ALS interrupt
6   | GND


