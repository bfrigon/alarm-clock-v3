### Bill of material ###

```
Date    : None
Doc. ID : CLKV3-SCH-07
Project : Alarm Clock (V3)
Title   : Power Supply

Part #  : CLKV3-PC-07
REV.    : A

Components : 40
```

------------------------------------------------------------------------------------------------------------------------


| Qty | Ref.             | Description                                      | Value    | Part #           | Footprint               |
|-----|------------------|--------------------------------------------------|----------|------------------|-------------------------|
| 1   | J3               | Header - Single Row - 2 Pos                      | BATTERY  | S2B-PH-K-S       | conn-jst:S2B-PH-K       |
| 1   | J1               | Header - Dual Row - 6 Pos                        | VIN      | TSW-103-07-F-D   | conn-header:HDR-M-2x03  |
| 1   | J2               | Header - Single Row - 4 Pos                      | SIGNAL   | TSW-104-07-F-S   | conn-header:HDR-M-1x04  |
| 2   | Q1-Q2            | 40V NPN small signal transistor                  | MMST3904 | -                | smt-sot:SOT23           |
| 1   | U2               | Li-Ion Battery charger and Power-Path management | -        | BQ24075          | smt-qfn:QFN16-EP-3x3    |
| 1   | U1               | System-side Impedance Track Fuel Gauge           | -        | BQ27441-G1       | smt-son:PDSO-N12        |
| 1   | U3               | 2V to 16V Buck-Boost converter                   | -        | TPS630701        | smt-qfn:VQFN15N         |
| 1   | R2               | Resistor                                         | 0.01 1%  | -                | smt:R-1206              |
| 1   | R10              | Resistor                                         | 0        | -                | smt:R-0603              |
| 1   | R9               | Resistor                                         | 1.1k     | -                | smt:R-0603              |
| 5   | R3-R6, R12       | Resistor                                         | 100k     | -                | smt:R-0603              |
| 1   | C4               | Capacitor                                        | 100nF    | -                | smt:C-0603              |
| 6   | R1, R11, R13-R16 | Resistor                                         | 10k      | -                | smt:R-0603              |
| 2   | C3, C5           | Capacitor                                        | 10uF     | -                | smt:C-0603              |
| 5   | C1-C2, C11-C13   | Capacitor                                        | 10uF     | -                | smt:C-0805              |
| 1   | C10              | Capacitor                                        | 1uF      | -                | smt:C-0603              |
| 3   | C6-C8            | Capacitor                                        | 22uF     | -                | smt:C-0805              |
| 1   | C9               | Capacitor                                        | 470nF    | -                | smt:C-0603              |
| 1   | R8               | Resistor                                         | 5.6k     | -                | smt:R-0603              |
| 1   | R17              | Resistor                                         | 680      | -                | smt:R-0603              |
| 1   | R7               | Resistor                                         | 787k     | -                | smt:R-0603              |
| 1   | D1               | Led                                              | RED      | 150080RS75000    | smt:LED-0805            |
| 1   | L1               | Inductor                                         | 1.5uH    | SPM5030T-1R5M-HZ | smt-inductor:SPM5030-HZ |
