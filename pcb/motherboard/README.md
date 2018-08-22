### Bill of material ###

```
Date    : 2018-08-22
Doc. ID : CLKV3-SCH-05
Project : Alarm Clock (V3)
Title   : Motherboard

Part #  : CLKV3-PC-05
REV.    : A

Components : 76
```

------------------------------------------------------------------------------------------------------------------------


| Qty | Ref.                       | Description                          | Value      | Part #             | Footprint                       |
|-----|----------------------------|--------------------------------------|------------|--------------------|---------------------------------|
| 1   | J5                         | Micro SD card connector              | Micro_SD   | -                  | conn-io:MOLEX-MICROSD-475710001 |
| 1   | J4                         | Header - Dual Row - 6 Pos            | ICSP-USB   | CES-103-01-T-D     | conn-header:HDR-M-2x03          |
| 1   | J7                         | Header - Dual Row - 6 Pos            | ICSP       | CES-103-01-T-D     | conn-header:HDR-M-2x03          |
| 2   | J11-J12                    | Header - Single Row - 1 Pos          | PSU-GND    | DW-01-08-T-S-300   | conn-header:HDR-M-1x01          |
| 1   | J6                         | Header - Single Row - 2 Pos          | PSU-SIG    | DW-02-08-TM-S-300  | conn-header:HDR-M-1x02          |
| 1   | J10                        | Header - Dual Row - 6 Pos            | PSU        | DW-03-08-T-D-300   | conn-header:HDR-M-2x03          |
| 1   | J3                         | Header - Single Row - 4 Pos          | DB PWR     | DW-04-07-TM-S-200  | conn-header:HDR-M-1x04          |
| 1   | J9                         | Header - Dual Row - 16 Pos           | DB I/O     | DW-08-07-T-D-200   | conn-header:HDR-M-2x08          |
| 1   | J1                         | DC Barrel Jack                       | -          | PJ1-021            | conn-power:CUI-P1J-021-SMT      |
| 1   | J2                         | USB Connector - Type B               | USB-MINI-B | UX60SC-MB-5ST      | conn-io:USB-MB_UX60SC-MB-5ST    |
| 2   | U5, U7                     | Hex Buffer non-inverting             | -          | 74X4050            | smt-sop:SSOP16                  |
| 1   | U6                         | 600mA CMOS LDO Regulator with enable | -          | AP2112K-3.3        | smt-sot:SOT23-5                 |
| 1   | U1                         | 8-bit Atmel Microcontroller          | -          | ATMEGA2560-16AU    | smt-qfp:QFP-14-100              |
| 1   | U3                         | 8-bit Atmel Microcontroller          | -          | ATMEGA8U2-AU       | smt-qfp:QFP-7-32                |
| 1   | U4                         | IEEE 802.11 b/g/n IoT Module         | -          | ATWINC1500-MR210PB | misc:ATWINC1500-MR              |
| 1   | U2                         | I2C Real-Time Clock                  | -          | DS3231M            | smt-soic:SOIC8                  |
| 8   | R10, R12-R18               | Resistor                             | 100k       | -                  | smt:R-0603                      |
| 12  | C3, C5-C6, C9-C11, C22-C27 | Capacitor                            | 100nF      | -                  | smt:C-0603                      |
| 1   | C14                        | Capacitor                            | 100nF      | -                  | smt:R-0603                      |
| 3   | R20-R22                    | Resistor                             | 10k        | -                  | smt:C-0603                      |
| 4   | R5-R6, R9, R11             | Resistor                             | 10k        | -                  | smt:R-0603                      |
| 1   | C15                        | Capacitor                            | 10nF       | -                  | smt:C-0603                      |
| 4   | C16-C18, C21               | Capacitor                            | 10uF       | -                  | smt:C-0805                      |
| 2   | R1-R2                      | Resistor                             | 1k         | -                  | smt:R-0603                      |
| 2   | R7-R8                      | Resistor                             | 1M         | -                  | smt:R-0603                      |
| 1   | D1                         | Diode                                | 1N4148     | -                  | smt:SOD-323                     |
| 4   | C4, C13, C19-C20           | Capacitor                            | 1uF        | -                  | smt:C-0805                      |
| 4   | C1-C2, C7-C8               | Capacitor                            | 20pF       | -                  | smt:C-0603                      |
| 1   | C12                        | Capacitor                            | 22pF       | -                  | smt:C-0603                      |
| 2   | R3-R4                      | Resistor                             | 22         | -                  | smt:R-0603                      |
| 1   | R19                        | Resistor                             | 300        | -                  | smt:C-0603                      |
| 1   | F1                         | Fuse                                 | 500mA      | -                  | smt:FER-0805                    |
| 1   | L1                         | Inductor                             | BLM21      | -                  | smt:FER-0805                    |
| 2   | Z1-Z2                      | ESD Supressor                        | PGB1010603 | -                  | smt:FER-0603                    |
| 1   | B1                         | Battery - 1 cell                     | CR1220     | BC501SM            | conn-power:BC501SM              |
| 2   | Y1-Y2                      | Crystal                              | 16.00 MHz  | ECS-160-20-3X-TR   | crystals:HC49USX                |
| 1   | SW1                        | Momentary SPST switch                | RESET      | B3F-1000           | switches:B3F-10XX               |
