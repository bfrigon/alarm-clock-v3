### Bill of material ###

```
Date    : 2018-08-22
Doc. ID : CLKV3-SCH-04
Project : Alarm Clock (V3)
Title   : Front Panel

Part #  : CLKV3-PC-04
REV.    : B

Components : 23
```

------------------------------------------------------------------------------------------------------------------------


| Qty | Ref.       | Description                      | Value | Part #         | Footprint            |
|-----|------------|----------------------------------|-------|----------------|----------------------|
| 2   | Q1-Q2      | N-Channel MOSFET                 | -     | BSS138         | smt-sot:SOT23        |
| 1   | U4         | 100 mA LDO Regulator w/ Shutdown | -     | LP2981-33DBVT  | smt-sot:SOT23-5      |
| 1   | U2         | DC/DC Converter                  | -     | LT1109CS8-12   | smt-soic:SOIC8       |
| 1   | U3         | Light To Digital Converter (I2C) | -     | TSL2591        | smt-dfn:DFN6         |
| 1   | U1         | 16x2 character OLED display      | -     | ER-OLED1602-4W | misc:LCD-ER-OLED1602 |
| 3   | C3, C6, C8 | Capacitor                        | 100nF | -              | smt:C-0603           |
| 2   | R2-R3      | Resistor                         | 10k   | -              | smt:R-0603           |
| 1   | C4         | Electrolytic capacitor           | 10uF  | -              | smt:CPL-0805         |
| 2   | C1, C9     | Capacitor                        | 1uF   | -              | smt:C-0805           |
| 1   | C7         | Electrolytic capacitor           | 22uF  | -              | smt:CPL-2312         |
| 1   | C10        | Capacitor                        | 3.3uF | -              | smt:C-0805           |
| 2   | R4-R5      | Resistor                         | 4.7k  | -              | smt:R-0603           |
| 1   | C5         | Electrolytic capacitor           | 4.7uF | -              | smt:CPL-0805         |
| 1   | C2         | Electrolytic capacitor           | 4.7uF | -              | smt:CPL-1206         |
| 1   | R1         | Resistor                         | 500k  | -              | smt:R-0603           |
| 1   | L1         | Inductor                         | 33uH  | CR54NP-330LC   | smt:IND-CD54         |
| 1   | D1         | Schottky Diode                   | -     | MBRS120T3      | smt:DO-214-AA        |
