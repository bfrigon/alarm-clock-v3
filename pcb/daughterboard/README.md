### Bill of material ###

```
Date    : 2018-09-19
Doc. ID : CLKV3-SCH-06
Project : Alarm Clock (V3)
Title   : Daughterboard

Part #  : CLKV3-PC-06
REV.    : A

Components : 70
```

------------------------------------------------------------------------------------------------------------------------


| Qty | Ref.                     | Description                            | Value      | Part #                | Footprint                 |
|-----|--------------------------|----------------------------------------|------------|-----------------------|---------------------------|
| 1   | J7                       | Header - Single Row - 2 Pos            | ALARM SW   | DF3A-2P-2DSA          | conn-header:DF3A-2P-2DSA  |
| 1   | J1                       | Header - Single Row - 3 Pos            | LAMP       | DF3A-3P-2DSA          | conn-header:DF3A-3P-2DSA  |
| 1   | J3                       | Header - Single Row - 4 Pos            | SPKR       | DF3A-4P-2DSA          | conn-header:DF3A-4P-2DSA  |
| 1   | J2                       | Header - Single Row - 8 Pos            | DISPLAY    | DF3A-8P-2DSA          | conn-header:DF3A-8P-2DSA  |
| 1   | U2                       | Single inverter                        | -          | 74AHC1G04SE-7         | smt-sot:SOT-353           |
| 1   | U3                       | Dual 150mA LDO Regulator               | -          | AP7312-1833W6         | smt-sot:SOT23-6           |
| 1   | Q1                       | Single P-Channel MOSFET 20V 10A        | NTF6P02    | NTF6P02T3G            | smt-sot:SOT-223           |
| 1   | U5                       | Dual non-inverting buffer              | -          | SN74LVC2G34DBV        | smt-sot:SOT23-6           |
| 1   | U4                       | 2.8W/Ch Stereo Class-D Audio Amplifier | -          | TPA2016D2             | smt-qfn:QFN20-EP-4x4      |
| 1   | U1                       | MP3/AAC/MIDI Audio Codec               | -          | VS1053B               | smt-qfp:QFP-7-48          |
| 10  | R4, R6, R12-R19          | Resistor                               | 100k       | -                     | smt:R-0603                |
| 14  | C1-C10, C26-C27, C29-C30 | Capacitor                              | 100nF      | -                     | smt:C-0603                |
| 3   | R2, R7, R11              | Resistor                               | 10k        | -                     | smt:R-0603                |
| 2   | C18-C19                  | Capacitor                              | 10nF       | -                     | smt:C-0603                |
| 1   | R8                       | Resistor                               | 10         | -                     | smt:R-0603                |
| 3   | C11, C14, C16            | Electrolytic capacitor                 | 10uF 6V3   | -                     | smt:CPL-0805              |
| 3   | C24-C25, C28             | Capacitor                              | 10uF       | -                     | smt:C-0805                |
| 1   | R5                       | Resistor                               | 1M         | -                     | smt:R-0603                |
| 4   | C31-C34                  | Capacitor                              | 1nF        | -                     | smt:C-0603                |
| 5   | C15, C20-C23             | Capacitor                              | 1uF        | -                     | smt:C-0805                |
| 2   | C12-C13                  | Capacitor                              | 20pF       | -                     | smt:C-0603                |
| 2   | R9-R10                   | Resistor                               | 20         | -                     | smt:R-0603                |
| 2   | R1, R3                   | Resistor                               | 470        | -                     | smt:R-0603                |
| 1   | C17                      | Capacitor                              | 47nF       | -                     | smt:C-0603                |
| 2   | L1-L2                    | Ferrite                                | -          | 2508053007Y0          | smt:FER-0805              |
| 1   | Y1                       | Crystal                                | 12.288 MHz | ECS-122.8-20-3X-EN-TR | crystals:smt-CSM-3X-7x4mm |
| 4   | L3-L6                    | Ferrite                                | -          | MPZ1608S221A          | smt:FER-0603              |
