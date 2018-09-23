### Bill of material ###

```
Date    : 2018-09-23
Doc. ID : CLKV3-SCH-05
Project : Alarm Clock (V3)
Title   : Motherboard

Part #  : CLKV3-PC-05
REV.    : A

Components : 86
```

------------------------------------------------------------------------------------------------------------------------


| Qty | Ref.                         | Description                                | Value      | Part #             | Footprint                       |
|-----|------------------------------|--------------------------------------------|------------|--------------------|---------------------------------|
| 1   | J5                           | Micro SD card connector                    | Micro_SD   | 475710001          | conn-io:MOLEX-MICROSD-475710001 |
| 1   | J10                          | Header - Dual Row - 6 Pos                  | PSU        | BCS-103-L-D-TE     | conn-header:HDR-M-2x03          |
| 1   | J6                           | Header - Single Row - 4 Pos                | PSU-SIG    | BCS-104-L-S-TE     | conn-header:HDR-M-1x04          |
| 1   | J4                           | Header - Dual Row - 6 Pos                  | ICSP-0     | CES-103-01-T-D     | conn-header:HDR-M-2x03          |
| 1   | J7                           | Header - Dual Row - 6 Pos                  | ICSP-1     | CES-103-01-T-D     | conn-header:HDR-M-2x03          |
| 1   | J18                          | Header - Single Row - 2 Pos                | DB-C       | DW-02-07-T-S-200   | conn-header:HDR-M-1x02          |
| 1   | J3                           | Header - Single Row - 4 Pos                | DB-B       | DW-04-07-TM-S-200  | conn-header:HDR-M-1x04          |
| 1   | J9                           | Header - Dual Row - 16 Pos                 | DB-A       | DW-08-07-T-D-200   | conn-header:HDR-M-2x08          |
| 1   | J1                           | DC Barrel Jack                             | -          | PJ1-021-SMT-TR     | conn-power:CUI-P1J-021-SMT      |
| 1   | J2                           | USB Connector - Type B                     | USB-MINI-B | UX60SC-MB-5ST      | conn-io:USB-MB_UX60SC-MB-5ST    |
| 1   | U9                           | Octal Bus Transceiver With 3-State Outputs | -          | 74AHC1G126SE-7     | smt-sot:SOT-353                 |
| 1   | U8                           | Dual Bus Buffer With 3-State Outputs       | -          | 74LVC2G126DC       | smt-sop:VSSOP8                  |
| 1   | U6                           | 600mA CMOS LDO Regulator with enable       | -          | AP2112K-3.3        | smt-sot:SOT23-5                 |
| 1   | U3                           | 8-bit Atmel Microcontroller                | -          | ATMEGA16U2-AU      | smt-qfp:QFP-7-32                |
| 1   | U1                           | 8-bit Atmel Microcontroller                | -          | ATMEGA2560-16AU    | smt-qfp:QFP-14-100              |
| 1   | U4                           | IEEE 802.11 b/g/n IoT Module               | -          | ATWINC1500-MR210PB | misc:ATWINC1500-MR              |
| 2   | U5, U7                       | Hex Buffer non-inverting                   | -          | CD74HC4050PWR      | smt-sop:TSSOP16                 |
| 1   | U2                           | I2C Real-Time Clock                        | -          | DS3231MZ           | smt-soic:SOIC8                  |
| 2   | R24, R26                     | Resistor                                   | 0          | -                  | smt:R-0805                      |
| 10  | R2, R10, R12-R18, R31        | Resistor                                   | 100k       | -                  | smt:R-0603                      |
| 13  | C5-C6, C9-C11, C22-C29       | Capacitor                                  | 100nF      | -                  | smt:C-0603                      |
| 1   | C14                          | Capacitor                                  | 100nF      | -                  | smt:R-0603                      |
| 3   | R20-R22                      | Resistor                                   | 10k        | -                  | smt:C-0603                      |
| 7   | R1, R5-R6, R9, R11, R27, R30 | Resistor                                   | 10k        | -                  | smt:R-0603                      |
| 1   | C15                          | Capacitor                                  | 10nF       | -                  | smt:C-0603                      |
| 4   | C16-C18, C21                 | Capacitor                                  | 10uF       | -                  | smt:C-0805                      |
| 3   | R19, R23, R33                | Resistor                                   | 1k         | -                  | smt:R-0603                      |
| 2   | R7-R8                        | Resistor                                   | 1M         | -                  | smt:R-0603                      |
| 1   | C13                          | Capacitor                                  | 1uF        | -                  | smt:C-0603                      |
| 3   | C4, C19-C20                  | Capacitor                                  | 1uF        | -                  | smt:C-0805                      |
| 4   | C1-C2, C7-C8                 | Capacitor                                  | 20pF       | -                  | smt:C-0603                      |
| 1   | C12                          | Capacitor                                  | 22pF       | -                  | smt:C-0603                      |
| 2   | R3-R4                        | Resistor                                   | 22         | -                  | smt:R-0603                      |
| 1   | F1                           | Fuse                                       | 500mA      | 1206L050/15YR      | smt:FER-1206                    |
| 1   | L1                           | Inductor                                   | BLM21      | BLM21BB050SZ1D     | smt:FER-0805                    |
| 2   | D1-D2                        | Diode                                      | -          | CD1206-S01575      | smt:D-1206                      |
| 2   | Z1-Z2                        | ESD Supressor                              | -          | CG0603MLC-05E      | smt:FER-0603                    |
| 2   | Y1-Y2                        | Crystal                                    | 16.00 MHz  | ECS-160-20-3X-TR   | crystals:smt-CSM-3X-7x4mm       |
| 1   | B1                           | Battery - 1 cell                           | CR1220     | S8411-45R          | conn-power:S8411-45R            |
| 1   | SW1                          | Momentary SPST switch                      | RESET      | B3F-1000           | switches:B3F-10XX               |
