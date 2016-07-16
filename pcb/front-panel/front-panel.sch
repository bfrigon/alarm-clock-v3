EESchema Schematic File Version 2
LIBS:connectors
LIBS:ic-cpu
LIBS:ic-power
LIBS:ic-misc
LIBS:ic-io
LIBS:passive
LIBS:power
LIBS:switches
LIBS:opto
LIBS:front-panel-cache
EELAYER 25 0
EELAYER END
$Descr USLetter 11000 8500
encoding utf-8
Sheet 1 1
Title "Front panel"
Date "2016-07-11"
Rev "A"
Comp ""
Comment1 "CLKV2-PC-04"
Comment2 "CLKV2-SCH-04"
Comment3 "Alarm clock (V3)"
Comment4 ""
$EndDescr
$Comp
L ER-OLED1602-4W U1
U 1 1 562D37B8
P 4050 4450
F 0 "U1" H 3600 5800 60  0000 L BNB
F 1 "ER-OLED1602-4W" H 3600 3100 40  0000 L TNN
F 2 "Misc:LCD-ER-OLED1602" H 3600 3025 40  0001 L TNN
F 3 "" H 3600 5800 50  0000 C CNN
F 4 "OPTO" H 3600 5900 40  0001 L BNN "Family"
F 5 "ER-OLED1602-4W" H 4050 4450 60  0001 C CNN "Part"
	1    4050 4450
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR12
U 1 1 562D385F
P 3350 5800
F 0 "#PWR12" H 3350 5850 30  0001 C CNN
F 1 "GND" H 3350 5675 30  0001 C CNN
F 2 "" H 3350 5800 60  0000 C CNN
F 3 "" H 3350 5800 60  0000 C CNN
	1    3350 5800
	1    0    0    -1  
$EndComp
$Comp
L RESISTOR R1
U 1 1 562D3A16
P 4950 5650
F 0 "R1" H 4950 5735 60  0000 C CNB
F 1 "500k" H 4950 5575 40  0000 C CNN
F 2 "SMT:R-0603" H 4950 5510 40  0001 C CNN
F 3 "" H 4950 5750 60  0000 C CNN
F 4 "-" H 4950 5810 45  0001 C CNN "Part"
F 5 "Passive" H 5180 5940 50  0001 C CNN "Family"
	1    4950 5650
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR15
U 1 1 562D3A86
P 5200 5800
F 0 "#PWR15" H 5200 5850 30  0001 C CNN
F 1 "GND" H 5200 5675 30  0001 C CNN
F 2 "" H 5200 5800 60  0000 C CNN
F 3 "" H 5200 5800 60  0000 C CNN
	1    5200 5800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR13
U 1 1 562D3B1B
P 4800 4400
F 0 "#PWR13" H 4800 4450 30  0001 C CNN
F 1 "GND" H 4800 4275 30  0001 C CNN
F 2 "" H 4800 4400 60  0000 C CNN
F 3 "" H 4800 4400 60  0000 C CNN
	1    4800 4400
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR14
U 1 1 562D3DB7
P 4800 5300
F 0 "#PWR14" H 4800 5350 30  0001 C CNN
F 1 "GND" H 4800 5175 30  0001 C CNN
F 2 "" H 4800 5300 60  0000 C CNN
F 3 "" H 4800 5300 60  0000 C CNN
	1    4800 5300
	1    0    0    -1  
$EndComp
Text Label 5550 3650 0    50   ~ 0
SCL
Text Label 5550 3750 0    50   ~ 0
SDA
Text Label 2050 1550 2    50   ~ 0
SDA
Text Label 2050 1650 2    50   ~ 0
SCL
Text Label 2050 1750 2    50   ~ 0
RESET
$Comp
L +5V #PWR11
U 1 1 577BF2B2
P 3200 5100
F 0 "#PWR11" H 3200 5325 30  0001 C CNN
F 1 "+5V" V 3125 5100 30  0001 C CNN
F 2 "" H 3200 5100 60  0000 C CNN
F 3 "" H 3200 5100 60  0000 C CNN
	1    3200 5100
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR8
U 1 1 577BF2C9
P 2700 4700
F 0 "#PWR8" H 2700 4750 30  0001 C CNN
F 1 "GND" H 2700 4575 30  0001 C CNN
F 2 "" H 2700 4700 60  0000 C CNN
F 3 "" H 2700 4700 60  0000 C CNN
	1    2700 4700
	1    0    0    -1  
$EndComp
$Comp
L CAP-E C2
U 1 1 577BF367
P 2350 4350
F 0 "C2" H 2380 4440 60  0000 L CNB
F 1 "4.7uF" H 2380 4260 45  0000 L CNN
F 2 "SMT:CPL-1206" H 2385 4195 40  0001 L CNN
F 3 "" H 2350 4450 60  0000 C CNN
F 4 "-" H 2385 4515 45  0001 L CNN "Part"
F 5 "Passive" H 2580 4640 50  0001 C CNN "Family"
	1    2350 4350
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR4
U 1 1 577BF40D
P 2350 4550
F 0 "#PWR4" H 2350 4600 30  0001 C CNN
F 1 "GND" H 2350 4425 30  0001 C CNN
F 2 "" H 2350 4550 60  0000 C CNN
F 3 "" H 2350 4550 60  0000 C CNN
	1    2350 4550
	1    0    0    -1  
$EndComp
$Comp
L CAP C3
U 1 1 577BF6EE
P 2700 3800
F 0 "C3" H 2730 3890 60  0000 L CNB
F 1 "100nF" H 2730 3710 45  0000 L CNN
F 2 "SMT:C-0603" H 2735 3645 40  0001 L CNN
F 3 "" H 2700 3900 60  0001 C CNN
F 4 "-" H 2740 3965 45  0001 L CNN "Part"
F 5 "Passive" H 2930 4090 50  0001 C CNN "Family"
	1    2700 3800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR6
U 1 1 577BF783
P 2700 3900
F 0 "#PWR6" H 2700 3950 30  0001 C CNN
F 1 "GND" H 2700 3775 30  0001 C CNN
F 2 "" H 2700 3900 60  0000 C CNN
F 3 "" H 2700 3900 60  0000 C CNN
	1    2700 3900
	1    0    0    -1  
$EndComp
$Comp
L +12V #PWR7
U 1 1 577BF7EA
P 2700 4300
F 0 "#PWR7" H 2700 4525 30  0001 C CNN
F 1 "+12V" V 2625 4300 30  0001 C CNN
F 2 "" H 2700 4300 60  0000 C CNN
F 3 "" H 2700 4300 60  0000 C CNN
	1    2700 4300
	1    0    0    -1  
$EndComp
$Comp
L CAP-E C5
U 1 1 577C0BD1
P 3050 3800
F 0 "C5" H 3080 3890 60  0000 L CNB
F 1 "10uF" H 3080 3710 45  0000 L CNN
F 2 "SMT:CPL-0805" H 3085 3645 40  0001 L CNN
F 3 "" H 3050 3900 60  0000 C CNN
F 4 "-" H 3085 3965 45  0001 L CNN "Part"
F 5 "Passive" H 3280 4090 50  0001 C CNN "Family"
	1    3050 3800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR9
U 1 1 577C0C15
P 3050 3900
F 0 "#PWR9" H 3050 3950 30  0001 C CNN
F 1 "GND" H 3050 3775 30  0001 C CNN
F 2 "" H 3050 3900 60  0000 C CNN
F 3 "" H 3050 3900 60  0000 C CNN
	1    3050 3900
	1    0    0    -1  
$EndComp
$Comp
L CAP C1
U 1 1 577C0D7D
P 2350 3550
F 0 "C1" H 2380 3640 60  0000 L CNB
F 1 "1uF" H 2380 3460 45  0000 L CNN
F 2 "SMT:C-0805" H 2385 3395 40  0001 L CNN
F 3 "" H 2350 3650 60  0001 C CNN
F 4 "-" H 2390 3715 45  0001 L CNN "Part"
F 5 "Passive" H 2580 3840 50  0001 C CNN "Family"
	1    2350 3550
	1    0    0    -1  
$EndComp
$Comp
L CAP C6
U 1 1 577C0DBE
P 3050 4550
F 0 "C6" H 3080 4640 60  0000 L CNB
F 1 "100nF" H 3080 4460 45  0000 L CNN
F 2 "SMT:C-0603" H 3085 4395 40  0001 L CNN
F 3 "" H 3050 4650 60  0001 C CNN
F 4 "-" H 3090 4715 45  0001 L CNN "Part"
F 5 "Passive" H 3280 4840 50  0001 C CNN "Family"
	1    3050 4550
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR10
U 1 1 577C0F0C
P 3050 4700
F 0 "#PWR10" H 3050 4750 30  0001 C CNN
F 1 "GND" H 3050 4575 30  0001 C CNN
F 2 "" H 3050 4700 60  0000 C CNN
F 3 "" H 3050 4700 60  0000 C CNN
	1    3050 4700
	1    0    0    -1  
$EndComp
$Comp
L CAP-E C4
U 1 1 577C1425
P 2700 4550
F 0 "C4" H 2730 4640 60  0000 L CNB
F 1 "10uF" H 2730 4460 45  0000 L CNN
F 2 "SMT:CPL-0805" H 2735 4395 40  0001 L CNN
F 3 "" H 2700 4650 60  0000 C CNN
F 4 "-" H 2735 4715 45  0001 L CNN "Part"
F 5 "Passive" H 2930 4840 50  0001 C CNN "Family"
	1    2700 4550
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR3
U 1 1 577C147B
P 2350 3750
F 0 "#PWR3" H 2350 3800 30  0001 C CNN
F 1 "GND" H 2350 3625 30  0001 C CNN
F 2 "" H 2350 3750 60  0000 C CNN
F 3 "" H 2350 3750 60  0000 C CNN
	1    2350 3750
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR5
U 1 1 577C33A0
P 2700 3600
F 0 "#PWR5" H 2700 3825 30  0001 C CNN
F 1 "+5V" V 2625 3600 30  0001 C CNN
F 2 "" H 2700 3600 60  0000 C CNN
F 3 "" H 2700 3600 60  0000 C CNN
	1    2700 3600
	1    0    0    -1  
$EndComp
Text Label 5700 5450 2    50   ~ 0
RESET
$Comp
L RESISTOR R3
U 1 1 577C6EFE
P 5800 2950
F 0 "R3" H 5800 3035 60  0000 C CNB
F 1 "0" H 5800 2875 40  0000 C CNN
F 2 "SMT:R-0603" H 5800 2810 40  0001 C CNN
F 3 "" H 5800 3050 60  0000 C CNN
F 4 "-" H 5800 3110 45  0001 C CNN "Part"
F 5 "Passive" H 6030 3240 50  0001 C CNN "Family"
	1    5800 2950
	0    -1   1    0   
$EndComp
$Comp
L RESISTOR R5
U 1 1 577C701A
P 6100 2950
F 0 "R5" H 6100 3035 60  0000 C CNB
F 1 "DNP" H 6100 2875 40  0000 C CNN
F 2 "SMT:R-0603" H 6100 2810 40  0001 C CNN
F 3 "" H 6100 3050 60  0000 C CNN
F 4 "-" H 6100 3110 45  0001 C CNN "Part"
F 5 "Passive" H 6330 3240 50  0001 C CNN "Family"
	1    6100 2950
	0    -1   1    0   
$EndComp
$Comp
L RESISTOR R7
U 1 1 577C7068
P 6400 2950
F 0 "R7" H 6400 3035 60  0000 C CNB
F 1 "0" H 6400 2875 40  0000 C CNN
F 2 "SMT:R-0603" H 6400 2810 40  0001 C CNN
F 3 "" H 6400 3050 60  0000 C CNN
F 4 "-" H 6400 3110 45  0001 C CNN "Part"
F 5 "Passive" H 6630 3240 50  0001 C CNN "Family"
	1    6400 2950
	0    -1   1    0   
$EndComp
$Comp
L RESISTOR R2
U 1 1 577C70B1
P 5800 2350
F 0 "R2" H 5800 2435 60  0000 C CNB
F 1 "DNP" H 5800 2275 40  0000 C CNN
F 2 "SMT:R-0603" H 5800 2210 40  0001 C CNN
F 3 "" H 5800 2450 60  0000 C CNN
F 4 "-" H 5800 2510 45  0001 C CNN "Part"
F 5 "Passive" H 6030 2640 50  0001 C CNN "Family"
	1    5800 2350
	0    -1   1    0   
$EndComp
$Comp
L RESISTOR R4
U 1 1 577C711F
P 6100 2350
F 0 "R4" H 6100 2435 60  0000 C CNB
F 1 "0" H 6100 2275 40  0000 C CNN
F 2 "SMT:R-0603" H 6100 2210 40  0001 C CNN
F 3 "" H 6100 2450 60  0000 C CNN
F 4 "-" H 6100 2510 45  0001 C CNN "Part"
F 5 "Passive" H 6330 2640 50  0001 C CNN "Family"
	1    6100 2350
	0    -1   1    0   
$EndComp
$Comp
L RESISTOR R6
U 1 1 577C717A
P 6400 2350
F 0 "R6" H 6400 2435 60  0000 C CNB
F 1 "DNP" H 6400 2275 40  0000 C CNN
F 2 "SMT:R-0603" H 6400 2210 40  0001 C CNN
F 3 "" H 6400 2450 60  0000 C CNN
F 4 "-" H 6400 2510 45  0001 C CNN "Part"
F 5 "Passive" H 6630 2640 50  0001 C CNN "Family"
	1    6400 2350
	0    -1   1    0   
$EndComp
$Comp
L +5V #PWR20
U 1 1 577C7501
P 6400 2150
F 0 "#PWR20" H 6400 2375 30  0001 C CNN
F 1 "+5V" V 6325 2150 30  0001 C CNN
F 2 "" H 6400 2150 60  0000 C CNN
F 3 "" H 6400 2150 60  0000 C CNN
	1    6400 2150
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR18
U 1 1 577C7549
P 6100 2150
F 0 "#PWR18" H 6100 2375 30  0001 C CNN
F 1 "+5V" V 6025 2150 30  0001 C CNN
F 2 "" H 6100 2150 60  0000 C CNN
F 3 "" H 6100 2150 60  0000 C CNN
	1    6100 2150
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR16
U 1 1 577C759C
P 5800 2150
F 0 "#PWR16" H 5800 2375 30  0001 C CNN
F 1 "+5V" V 5725 2150 30  0001 C CNN
F 2 "" H 5800 2150 60  0000 C CNN
F 3 "" H 5800 2150 60  0000 C CNN
	1    5800 2150
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR17
U 1 1 577C77E7
P 5800 3150
F 0 "#PWR17" H 5800 3200 30  0001 C CNN
F 1 "GND" H 5800 3025 30  0001 C CNN
F 2 "" H 5800 3150 60  0000 C CNN
F 3 "" H 5800 3150 60  0000 C CNN
	1    5800 3150
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR19
U 1 1 577C7828
P 6100 3150
F 0 "#PWR19" H 6100 3200 30  0001 C CNN
F 1 "GND" H 6100 3025 30  0001 C CNN
F 2 "" H 6100 3150 60  0000 C CNN
F 3 "" H 6100 3150 60  0000 C CNN
	1    6100 3150
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR21
U 1 1 577C7869
P 6400 3150
F 0 "#PWR21" H 6400 3200 30  0001 C CNN
F 1 "GND" H 6400 3025 30  0001 C CNN
F 2 "" H 6400 3150 60  0000 C CNN
F 3 "" H 6400 3150 60  0000 C CNN
	1    6400 3150
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR23
U 1 1 5780AC08
P 7450 4000
F 0 "#PWR23" H 7450 4225 30  0001 C CNN
F 1 "+5V" V 7375 4000 30  0001 C CNN
F 2 "" H 7450 4000 60  0000 C CNN
F 3 "" H 7450 4000 60  0000 C CNN
	1    7450 4000
	1    0    0    -1  
$EndComp
$Comp
L INDUCTOR L1
U 1 1 5780BCE4
P 8050 4150
F 0 "L1" H 8050 4240 60  0000 C CNB
F 1 "33uH" H 8050 4080 40  0000 C CNN
F 2 "SMT:IND-CD54" H 8050 4020 40  0001 C CNN
F 3 "" H 8050 4250 60  0000 C CNN
F 4 "CR54NP-330LC" H 8050 4000 40  0000 C CNN "PART"
F 5 "Passive" H 8280 4440 50  0001 C CNN "Family"
	1    8050 4150
	1    0    0    -1  
$EndComp
$Comp
L SCHOTTKY D1
U 1 1 5780BEA6
P 8950 4150
F 0 "D1" H 8950 4260 60  0000 C CNB
F 1 "MBRS120T3" H 8950 4050 40  0000 C CNN
F 2 "SMT:DO-214-AA" H 8950 3990 40  0001 C CNN
F 3 "" H 8950 4270 60  0000 C CNN
F 4 "MBRS120T3" H 8955 4325 45  0001 C CNN "PART"
F 5 "Passive" H 9180 4440 50  0001 C CNN "Family"
	1    8950 4150
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR24
U 1 1 5780CB67
P 7450 5500
F 0 "#PWR24" H 7450 5550 30  0001 C CNN
F 1 "GND" H 7450 5375 30  0001 C CNN
F 2 "" H 7450 5500 60  0000 C CNN
F 3 "" H 7450 5500 60  0000 C CNN
	1    7450 5500
	1    0    0    -1  
$EndComp
$Comp
L SUP_+12V #PWR25
U 1 1 5780D7CE
P 9250 4000
F 0 "#PWR25" H 9250 4225 30  0001 C CNN
F 1 "SUP_+12V" V 9175 4000 30  0001 C CNN
F 2 "" H 9150 4125 60  0001 C CNN
F 3 "" H 9250 4225 60  0001 C CNN
	1    9250 4000
	1    0    0    -1  
$EndComp
Text Label 6650 4950 0    50   ~ 0
VCC_EN
$Comp
L CAP-E C7
U 1 1 57810971
P 9250 5250
F 0 "C7" H 9280 5340 60  0000 L CNB
F 1 "22uF" H 9280 5160 45  0000 L CNN
F 2 "SMT:CPL-2312" H 9285 5095 40  0001 L CNN
F 3 "" H 9250 5350 60  0000 C CNN
F 4 "-" H 9285 5415 45  0001 L CNN "Part"
F 5 "Passive" H 9480 5540 50  0001 C CNN "Family"
	1    9250 5250
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR26
U 1 1 57810AEC
P 9250 5600
F 0 "#PWR26" H 9250 5650 30  0001 C CNN
F 1 "GND" H 9250 5475 30  0001 C CNN
F 2 "" H 9250 5600 60  0000 C CNN
F 3 "" H 9250 5600 60  0000 C CNN
	1    9250 5600
	1    0    0    -1  
$EndComp
$Comp
L SUP_GND #PWR2
U 1 1 5781A10F
P 1650 2050
F 0 "#PWR2" H 1650 2100 30  0001 C CNN
F 1 "SUP_GND" H 1650 1925 30  0001 C CNN
F 2 "" H 1650 2050 60  0000 C CNN
F 3 "" H 1650 2050 60  0000 C CNN
	1    1650 2050
	-1   0    0    -1  
$EndComp
$Comp
L SUP_+5V #PWR1
U 1 1 5781A17B
P 1650 1350
F 0 "#PWR1" H 1650 1575 30  0001 C CNN
F 1 "SUP_+5V" V 1575 1350 30  0001 C CNN
F 2 "" H 1550 1475 60  0001 C CNN
F 3 "" H 1650 1575 60  0001 C CNN
	1    1650 1350
	-1   0    0    -1  
$EndComp
Text Notes 3450 2650 0    100  ~ 0
OLED module
Text Notes 7300 3550 0    100  ~ 0
12V OLED display supply
$Comp
L RESISTOR R12
U 1 1 5782CAD6
P 7100 5200
F 0 "R12" H 7100 5285 60  0000 C CNB
F 1 "10k" H 7100 5125 40  0000 C CNN
F 2 "SMT:R-0603" H 7100 5060 40  0001 C CNN
F 3 "" H 7100 5300 60  0000 C CNN
F 4 "-" H 7100 5360 45  0001 C CNN "Part"
F 5 "Passive" H 7330 5490 50  0001 C CNN "Family"
	1    7100 5200
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR22
U 1 1 5782CD1E
P 7100 5500
F 0 "#PWR22" H 7100 5550 30  0001 C CNN
F 1 "GND" H 7100 5375 30  0001 C CNN
F 2 "" H 7100 5500 60  0000 C CNN
F 3 "" H 7100 5500 60  0000 C CNN
	1    7100 5500
	1    0    0    -1  
$EndComp
NoConn ~ 3500 3850
$Comp
L HEADER-1x01 J2
U 1 1 57844FE7
P 2750 1150
F 0 "J2" H 2700 1250 60  0000 L BNB
F 1 "HEADER-1x01" V 2900 1150 40  0001 C CNN
F 2 "Conn-Wire-Pads:SolderPad-100x500" H 2600 1125 60  0001 C CNN
F 3 "" H 2700 1225 60  0001 C CNN
F 4 "-" H 2700 1350 40  0001 L BNN "Part"
F 5 "Virtual" H 2700 1450 40  0001 L BNN "Family"
	1    2750 1150
	1    0    0    -1  
$EndComp
$Comp
L HEADER-1x01 J3
U 1 1 578469E9
P 2750 1500
F 0 "J3" H 2700 1600 60  0000 L BNB
F 1 "HEADER-1x01" V 2900 1500 40  0001 C CNN
F 2 "Conn-Wire-Pads:SolderPad-100x500" H 2600 1475 60  0001 C CNN
F 3 "" H 2700 1575 60  0001 C CNN
F 4 "-" H 2700 1700 40  0001 L BNN "Part"
F 5 "Virtual" H 2700 1800 40  0001 L BNN "Family"
	1    2750 1500
	1    0    0    -1  
$EndComp
$Comp
L HEADER-1x01 J4
U 1 1 5784E081
P 2750 1850
F 0 "J4" H 2700 1950 60  0000 L BNB
F 1 "HEADER-1x01" V 2900 1850 40  0001 C CNN
F 2 "Conn-Wire-Pads:SolderPad-100x500" H 2600 1825 60  0001 C CNN
F 3 "" H 2700 1925 60  0001 C CNN
F 4 "-" H 2700 2050 40  0001 L BNN "Part"
F 5 "Virtual" H 2700 2150 40  0001 L BNN "Family"
	1    2750 1850
	1    0    0    -1  
$EndComp
Text Label 8300 4150 0    60   ~ 0
SW_12V
$Comp
L HEADER-1x01 J5
U 1 1 57861573
P 2750 2200
F 0 "J5" H 2700 2300 60  0000 L BNB
F 1 "HEADER-1x01" V 2900 2200 40  0001 C CNN
F 2 "Conn-Wire-Pads:SolderPad-100x150" H 2600 2175 60  0001 C CNN
F 3 "" H 2700 2275 60  0001 C CNN
F 4 "-" H 2700 2400 40  0001 L BNN "Part"
F 5 "Virtual" H 2700 2500 40  0001 L BNN "Family"
	1    2750 2200
	1    0    0    -1  
$EndComp
$Comp
L TEST-POINT TP1
U 1 1 57861B17
P 9650 4950
F 0 "TP1" H 9600 5025 60  0000 L BNB
F 1 "12V" H 9600 4850 40  0000 L TNN
F 2 "Conn-Wire-Pads:TEST-POINT-50" H 9500 4925 60  0001 C CNN
F 3 "" H 9600 5025 60  0001 C CNN
F 4 "Virtual" H 9600 5150 40  0001 L BNN "Family"
	1    9650 4950
	1    0    0    -1  
$EndComp
Wire Wire Line
	3350 5650 3500 5650
Wire Wire Line
	3350 5550 3500 5550
Connection ~ 3350 5650
Wire Wire Line
	3350 5450 3500 5450
Connection ~ 3350 5550
Connection ~ 3350 5450
Wire Wire Line
	3350 4650 3350 5800
Wire Wire Line
	3500 4650 3350 4650
Wire Wire Line
	3500 4750 3350 4750
Connection ~ 3350 4750
Wire Wire Line
	3500 4850 3350 4850
Connection ~ 3350 4850
Wire Wire Line
	3500 4950 3350 4950
Connection ~ 3350 4950
Wire Wire Line
	3500 5150 3200 5150
Wire Wire Line
	3200 5100 3200 5250
Wire Wire Line
	3200 5250 3500 5250
Connection ~ 3200 5150
Wire Wire Line
	4850 5650 4600 5650
Wire Wire Line
	5050 5650 5200 5650
Wire Wire Line
	5200 5650 5200 5800
Wire Wire Line
	4800 4350 4600 4350
Wire Wire Line
	4800 3950 4800 4400
Wire Wire Line
	4600 4250 4800 4250
Connection ~ 4800 4350
Wire Wire Line
	4600 4150 4800 4150
Connection ~ 4800 4250
Wire Wire Line
	4600 4050 4800 4050
Connection ~ 4800 4150
Wire Wire Line
	4600 3950 4800 3950
Connection ~ 4800 4050
Wire Wire Line
	4600 3750 5700 3750
Wire Wire Line
	4800 3750 4800 3850
Wire Wire Line
	4800 3850 4600 3850
Connection ~ 4800 3750
Wire Wire Line
	4600 3650 5700 3650
Wire Wire Line
	4600 5450 5700 5450
Wire Wire Line
	4800 5250 4600 5250
Wire Wire Line
	4800 4950 4800 5300
Wire Wire Line
	4600 5150 4800 5150
Connection ~ 4800 5250
Wire Wire Line
	4600 5050 4800 5050
Connection ~ 4800 5150
Wire Wire Line
	1500 1950 1650 1950
Wire Wire Line
	1650 1950 1650 2050
Wire Wire Line
	1500 1450 1650 1450
Wire Wire Line
	1650 1450 1650 1350
Wire Wire Line
	1500 1550 2050 1550
Wire Wire Line
	1500 1650 2050 1650
Wire Wire Line
	1500 1750 2050 1750
Wire Wire Line
	2700 4300 2700 4450
Wire Wire Line
	2700 4650 2700 4700
Wire Wire Line
	3500 4050 2350 4050
Wire Wire Line
	2350 4050 2350 4250
Wire Wire Line
	2350 4450 2350 4550
Wire Wire Line
	3500 3450 3300 3450
Wire Wire Line
	3300 3450 3300 3250
Wire Wire Line
	2350 3250 3500 3250
Wire Wire Line
	2700 3650 3500 3650
Wire Wire Line
	2700 3600 2700 3700
Wire Wire Line
	2700 3900 2700 3900
Connection ~ 2700 4350
Wire Wire Line
	2700 4350 3500 4350
Wire Wire Line
	3050 3900 3050 3900
Wire Wire Line
	3050 3700 3050 3650
Connection ~ 3050 3650
Wire Wire Line
	3050 4350 3050 4450
Connection ~ 3050 4350
Wire Wire Line
	3050 4650 3050 4700
Wire Wire Line
	2350 3250 2350 3450
Connection ~ 3300 3250
Wire Wire Line
	2350 3650 2350 3750
Connection ~ 2700 3650
Wire Wire Line
	4600 4950 4800 4950
Connection ~ 4800 5050
Wire Wire Line
	4600 3250 5100 3250
Wire Wire Line
	5100 3250 5100 2550
Wire Wire Line
	5100 2550 5800 2550
Wire Wire Line
	5800 2450 5800 2850
Wire Wire Line
	4600 3350 5200 3350
Wire Wire Line
	5200 3350 5200 2650
Wire Wire Line
	5200 2650 6100 2650
Wire Wire Line
	6100 2450 6100 2850
Wire Wire Line
	4600 3450 5300 3450
Wire Wire Line
	5300 3450 5300 2750
Wire Wire Line
	5300 2750 6400 2750
Wire Wire Line
	6400 2450 6400 2850
Connection ~ 6400 2750
Connection ~ 6100 2650
Connection ~ 5800 2550
Wire Wire Line
	6400 3050 6400 3150
Wire Wire Line
	6100 3050 6100 3150
Wire Wire Line
	5800 3050 5800 3150
Wire Wire Line
	5800 2150 5800 2250
Wire Wire Line
	6100 2150 6100 2250
Wire Wire Line
	6400 2150 6400 2250
Wire Wire Line
	8500 4650 8650 4650
Wire Wire Line
	8650 4650 8650 4150
Wire Wire Line
	8150 4150 8850 4150
Connection ~ 8650 4150
Wire Wire Line
	9050 4150 9250 4150
Wire Wire Line
	9250 4000 9250 5150
Wire Wire Line
	8500 4950 9500 4950
Wire Wire Line
	7450 4000 7450 4650
Wire Wire Line
	7450 4150 7950 4150
Wire Wire Line
	7450 4650 7600 4650
Connection ~ 7450 4150
Wire Wire Line
	7600 5350 7450 5350
Wire Wire Line
	7450 5350 7450 5500
Connection ~ 9250 4150
Wire Wire Line
	6650 4950 7600 4950
Connection ~ 9250 4950
Wire Wire Line
	9250 5350 9250 5600
Wire Wire Line
	7100 5100 7100 4950
Connection ~ 7100 4950
Wire Wire Line
	7100 5300 7100 5500
$Comp
L TEST-POINT TP2
U 1 1 57861CCA
P 9650 5500
F 0 "TP2" H 9600 5575 60  0000 L BNB
F 1 "GND" H 9600 5400 40  0000 L TNN
F 2 "Conn-Wire-Pads:TEST-POINT-50" H 9500 5475 60  0001 C CNN
F 3 "" H 9600 5575 60  0001 C CNN
F 4 "Virtual" H 9600 5700 40  0001 L BNN "Family"
	1    9650 5500
	1    0    0    -1  
$EndComp
Wire Wire Line
	9500 5500 9250 5500
Connection ~ 9250 5500
NoConn ~ 2600 1150
NoConn ~ 2600 1500
NoConn ~ 2600 1850
NoConn ~ 2600 2200
$Comp
L LT1109CS8 U2
U 1 1 57872A62
P 8050 4750
F 0 "U2" H 7700 5000 60  0000 L BNB
F 1 "LT1109CS8-12" H 7700 4000 40  0000 L TNN
F 2 "SMT-SOIC:SOIC8" H 7700 3900 40  0001 L CNN
F 3 "" H 7700 3850 40  0001 L CNN
F 4 "IC" H 7700 5100 40  0001 L BNN "Family"
F 5 "LT1109CS8-12" H 8050 4750 60  0001 C CNN "Part"
	1    8050 4750
	1    0    0    -1  
$EndComp
$Comp
L HEADER-1x06 J1
U 1 1 578A678A
P 1350 1700
F 0 "J1" H 1300 2050 60  0000 L BNB
F 1 "HEADER-1x06" V 1500 1700 40  0001 C CNN
F 2 "Conn-Wire-Pads:WP-50mil-1x06" H 1350 1700 60  0001 C CNN
F 3 "" H 1350 1700 60  0001 C CNN
F 4 "-" H 1300 2150 40  0001 L BNN "Part"
F 5 "Virtual" H 1300 2250 40  0001 L BNN "Family"
	1    1350 1700
	-1   0    0    -1  
$EndComp
Wire Wire Line
	1500 1850 2050 1850
Text Label 2050 1850 2    50   ~ 0
VCC_EN
$EndSCHEMATC
