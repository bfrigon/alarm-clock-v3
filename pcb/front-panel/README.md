### Bill of material ###

```
Date    : None
Doc. ID : CLKV2-SCH-04
Project : Alarm Clock (V3)
Title   : Front Panel

Part #  : CLKV2-PC-04
REV.    : A

Components : 31
```

------------------------------------------------------------------------------------------------------------------------


| Qty | Ref.        | Description                      | Value          | Part #    | Footprint                    |
|-----|-------------|----------------------------------|----------------|-----------|------------------------------|
| 1   | J1          | Header - Single Row - 5 Pos      | HEADER-1x05    | -         | Conn-Wire-Pads:WP-50mil-1x05 |
| 2   | Q1-Q2       | N-Channel MOSFET                 | -              | BSS138    | SOT23                        |
| 1   | U4          | 100 mA LDO Regulator w/ Shutdown | -              | LP2981    | SOT23-5                      |
| 1   | U3          | DC/DC Converter                  | -              | LT1109CS8 | SOIC-8                       |
| 1   | U2          | Light To Digital Converter (I2C) | -              | TSL2561   | SMT-DFN:DFN6                 |
| 1   | U1          | 16x2 character OLED display      | ER-OLED1602-4W | -         | Misc:LCD-ER-OLED1602         |
| 3   | R3-R4, R7   | Resistor                         | 0              | -         | SMT-0603                     |
| 3   | C3, C6, C10 | Capacitor                        | 100nF          | -         | SMT-0603                     |
| 5   | R8-R12      | Resistor                         | 10k            | -         | SMT-0603                     |
| 2   | C4-C5       | Electrolytic capacitor           | 10uF           | -         | SMT-0805                     |
| 2   | C1, C8      | Capacitor                        | 1uF            | -         | SMT-0805                     |
| 1   | C7          | Electrolytic capacitor           | 22uF           | -         |                              |
| 1   | C9          | Capacitor                        | 3.3uF          | -         | SMT-0805                     |
| 1   | L1          | Inductor                         | 33uH           | -         |                              |
| 1   | C2          | Electrolytic capacitor           | 4.7uF          | -         | SMT-1206                     |
| 1   | R1          | Resistor                         | 500k           | -         | SMT-0603                     |
| 1   | D1          | Schottky Diode                   | MBRS120T3      | -         |                              |
| 3   | R2, R5-R6   | Resistor                         | N/A            | -         | SMT-0603                     |
