EESchema Schematic File Version 4
LIBS:motherboard-cache
EELAYER 26 0
EELAYER END
$Descr USLetter 11000 8500
encoding utf-8
Sheet 1 3
Title "Motherboard"
Date "2018-10-29"
Rev "B"
Comp ""
Comment1 "CLKV3-PC-05"
Comment2 "CLKV3-SCH-05"
Comment3 "Alarm clock (V3)"
Comment4 ""
$EndDescr
$Comp
L connectors:HEADER-2x03 J7
U 1 1 56F57836
P 9800 2450
F 0 "J7" H 9700 2650 60  0000 L BNB
F 1 "ICSP" H 9800 2250 40  0000 C TNN
F 2 "conn-header:HDR-M-2x03" H 9800 1250 60  0001 C CNN
F 3 "" H 9800 1250 60  0001 C CNN
F 4 "CES-103-01-T-D" H 9700 2750 40  0001 L BNN "Part"
F 5 "Connector" H 9700 2850 40  0001 L BNN "Family"
	1    9800 2450
	1    0    0    -1  
$EndComp
$Comp
L ic-cpu:ATMEGA2560-16AU U1
U 1 1 578A55F4
P 6150 4200
F 0 "U1" H 5200 7300 60  0000 L BNB
F 1 "ATMEGA2560-16AU" H 5200 1100 40  0000 L TNN
F 2 "smt-qfp:QFP-14-100" H 5200 1000 40  0001 L CNN
F 3 "A" H 5200 7475 40  0001 L BNN
F 4 "IC" H 5200 7400 40  0001 L BNN "Family"
	1    6150 4200
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR05
U 1 1 578B44D2
P 4950 7250
F 0 "#PWR05" H 4950 7300 30  0001 C CNN
F 1 "GND" H 4950 7125 30  0001 C CNN
F 2 "" H 4950 7250 60  0000 C CNN
F 3 "" H 4950 7250 60  0000 C CNN
	1    4950 7250
	1    0    0    -1  
$EndComp
Wire Wire Line
	4950 7150 5050 7150
Wire Wire Line
	4950 6650 4950 6750
Wire Wire Line
	5050 6950 4950 6950
Connection ~ 4950 7150
Wire Wire Line
	5050 6850 4950 6850
Connection ~ 4950 6950
Wire Wire Line
	5050 6750 4950 6750
Connection ~ 4950 6850
Wire Wire Line
	5050 6650 4950 6650
Connection ~ 4950 6750
Wire Wire Line
	9600 2350 9300 2350
$Comp
L power:+5V #PWR06
U 1 1 578B4D84
P 10050 2250
F 0 "#PWR06" H 10050 2475 30  0001 C CNN
F 1 "+5V" V 9975 2250 30  0001 C CNN
F 2 "" H 10050 2250 60  0000 C CNN
F 3 "" H 10050 2250 60  0000 C CNN
	1    10050 2250
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR07
U 1 1 578B4E94
P 10050 2700
F 0 "#PWR07" H 10050 2750 30  0001 C CNN
F 1 "GND" H 10050 2575 30  0001 C CNN
F 2 "" H 10050 2700 60  0000 C CNN
F 3 "" H 10050 2700 60  0000 C CNN
	1    10050 2700
	1    0    0    -1  
$EndComp
Wire Wire Line
	10000 2550 10050 2550
Wire Wire Line
	10050 2550 10050 2700
Wire Wire Line
	10000 2350 10050 2350
Wire Wire Line
	10050 2350 10050 2250
Wire Wire Line
	10000 2450 10250 2450
Text Label 10250 2450 2    50   ~ 0
MOSI
Wire Wire Line
	9600 2450 9300 2450
Wire Wire Line
	9600 2550 9300 2550
Text Label 9300 2550 0    50   ~ 0
RESET
Text Label 9300 2450 0    50   ~ 0
SCK
Text Label 9300 2350 0    50   ~ 0
MISO
Wire Wire Line
	7250 2750 8100 2750
Wire Wire Line
	7250 2650 8100 2650
Wire Wire Line
	7250 2550 8100 2550
Text Label 8100 2550 2    60   ~ 0
MISO
Text Label 8100 2650 2    60   ~ 0
MOSI
Text Label 8100 2750 2    60   ~ 0
SCK
$Sheet
S 550  3750 700  650 
U 578C5B51
F0 "USB Serial" 50
F1 "usb-serial.sch" 50
F2 "RXD" I R 1250 3850 60 
F3 "TXD" O R 1250 3950 60 
F4 "RESET" U R 1250 4300 60 
$EndSheet
$Comp
L connectors:HEADER-1x01 H5
U 1 1 5798DC1A
P 850 7050
F 0 "H5" H 800 7150 60  0000 L BNB
F 1 "HEADER-1x01" V 1000 7050 40  0001 C CNN
F 2 "conn-wire-pads:SolderPad-100x500" H 700 7025 60  0001 C CNN
F 3 "" H 800 7125 60  0001 C CNN
F 4 "-" H 800 7250 40  0001 L BNN "Part"
F 5 "Virtual" H 800 7350 40  0001 L BNN "Family"
	1    850  7050
	1    0    0    -1  
$EndComp
$Comp
L connectors:HEADER-1x01 H4
U 1 1 5798E823
P 850 7350
F 0 "H4" H 800 7450 60  0000 L BNB
F 1 "HEADER-1x01" V 1000 7350 40  0001 C CNN
F 2 "conn-wire-pads:SolderPad-100x500" H 700 7325 60  0001 C CNN
F 3 "" H 800 7425 60  0001 C CNN
F 4 "-" H 800 7550 40  0001 L BNN "Part"
F 5 "Virtual" H 800 7650 40  0001 L BNN "Family"
	1    850  7350
	1    0    0    -1  
$EndComp
$Comp
L passive:RESISTOR R8
U 1 1 57990D77
P 4900 2550
F 0 "R8" H 4900 2635 60  0000 C CNB
F 1 "1M" H 4900 2475 40  0000 C CNN
F 2 "smt:R-0603" H 4900 2410 40  0001 C CNN
F 3 "" H 4900 2650 60  0000 C CNN
F 4 "-" H 4900 2710 45  0001 C CNN "Part"
F 5 "Passive" H 5130 2840 50  0001 C CNN "Family"
	1    4900 2550
	0    -1   1    0   
$EndComp
Wire Wire Line
	4200 2350 4550 2350
Wire Wire Line
	4900 2350 4900 2450
Wire Wire Line
	4900 2750 4900 2650
Wire Wire Line
	4200 2750 4550 2750
$Comp
L passive:CRYSTAL Y2
U 1 1 57990FA9
P 4550 2550
F 0 "Y2" H 4550 2660 60  0000 C CNB
F 1 "16.00 MHz" H 4550 2450 40  0000 C CNN
F 2 "crystals:smt-CSM-3X-7x4mm" H 4550 2390 40  0001 C CNN
F 3 "" H 4550 2670 60  0000 C CNN
F 4 "ECS-160-20-3X-TR" V 4800 2450 45  0001 C CNN "Part"
F 5 "Passive" H 4780 2840 50  0001 C CNN "Family"
	1    4550 2550
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4550 2350 4550 2450
Connection ~ 4900 2350
Wire Wire Line
	4550 2650 4550 2750
Connection ~ 4900 2750
$Comp
L passive:CAP C7
U 1 1 579910B6
P 4100 2350
F 0 "C7" H 4130 2440 60  0000 L CNB
F 1 "20pF" H 4130 2260 45  0000 L CNN
F 2 "smt:C-0603" H 4135 2195 40  0001 L CNN
F 3 "" H 4100 2450 60  0001 C CNN
F 4 "-" H 4140 2515 45  0001 L CNN "Part"
F 5 "Passive" H 4330 2640 50  0001 C CNN "Family"
	1    4100 2350
	0    -1   -1   0   
$EndComp
$Comp
L passive:CAP C8
U 1 1 579911C1
P 4100 2750
F 0 "C8" H 4130 2840 60  0000 L CNB
F 1 "20pF" H 4130 2660 45  0000 L CNN
F 2 "smt:C-0603" H 4135 2595 40  0001 L CNN
F 3 "" H 4100 2850 60  0001 C CNN
F 4 "-" H 4140 2915 45  0001 L CNN "Part"
F 5 "Passive" H 4330 3040 50  0001 C CNN "Family"
	1    4100 2750
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR011
U 1 1 57991215
P 3900 2950
F 0 "#PWR011" H 3900 3000 30  0001 C CNN
F 1 "GND" H 3900 2825 30  0001 C CNN
F 2 "" H 3900 2950 60  0000 C CNN
F 3 "" H 3900 2950 60  0000 C CNN
	1    3900 2950
	1    0    0    -1  
$EndComp
Wire Wire Line
	3900 2350 3900 2750
Wire Wire Line
	3900 2750 4000 2750
Wire Wire Line
	3900 2350 4000 2350
Connection ~ 3900 2750
Connection ~ 4550 2350
Connection ~ 4550 2750
Wire Wire Line
	7250 4550 8100 4550
Wire Wire Line
	7250 4650 8100 4650
Text Label 8100 4550 2    60   ~ 0
SDA
Text Label 8100 4650 2    60   ~ 0
SCL
$Comp
L passive:CAP C10
U 1 1 58428E2A
P 4250 1450
F 0 "C10" H 4280 1540 60  0000 L CNB
F 1 "100nF" H 4280 1360 45  0000 L CNN
F 2 "smt:C-0603" H 4285 1295 40  0001 L CNN
F 3 "" H 4250 1550 60  0001 C CNN
F 4 "-" H 4290 1615 45  0001 L CNN "Part"
F 5 "Passive" H 4480 1740 50  0001 C CNN "Family"
	1    4250 1450
	1    0    0    -1  
$EndComp
$Comp
L switches:MOM-SPST SW1
U 1 1 5B37258B
P 2500 5200
F 0 "SW1" H 2400 5300 60  0000 L BNB
F 1 "RESET" H 2400 5150 40  0000 L TNN
F 2 "switches:B3F-10XX" H 2400 5075 40  0001 L TNN
F 3 "" H 2425 4975 60  0001 C CNN
F 4 "B3F-1000" H 2575 5400 40  0001 L BNN "Part"
F 5 "Switch" H 2575 5500 40  0001 L BNN "Family"
	1    2500 5200
	-1   0    0    -1  
$EndComp
$Comp
L connectors:HEADER-1x01 H6
U 1 1 5B7555B7
P 850 6700
F 0 "H6" H 800 6800 60  0000 L BNB
F 1 "HEADER-1x01" V 1000 6700 40  0001 C CNN
F 2 "conn-wire-pads:SolderPad-100x300" H 700 6675 60  0001 C CNN
F 3 "" H 800 6775 60  0001 C CNN
F 4 "-" H 800 6900 40  0001 L BNN "Part"
F 5 "Virtual" H 800 7000 40  0001 L BNN "Family"
	1    850  6700
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR016
U 1 1 5B780AB6
P 2300 5300
F 0 "#PWR016" H 2300 5350 30  0001 C CNN
F 1 "GND" H 2300 5175 30  0001 C CNN
F 2 "" H 2300 5300 60  0000 C CNN
F 3 "" H 2300 5300 60  0000 C CNN
	1    2300 5300
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2400 5200 2300 5200
Wire Wire Line
	2300 5200 2300 5300
Wire Wire Line
	3000 5200 2800 5200
$Comp
L passive:CAP C13
U 1 1 5B795B49
P 2800 4750
F 0 "C13" H 2830 4840 60  0000 L CNB
F 1 "1uF" H 2830 4660 45  0000 L CNN
F 2 "smt:C-0603" H 2835 4595 40  0001 L CNN
F 3 "" H 2800 4850 60  0001 C CNN
F 4 "-" H 2840 4915 45  0001 L CNN "Part"
F 5 "Passive" H 3030 5040 50  0001 C CNN "Family"
	1    2800 4750
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2800 4850 2800 5200
Connection ~ 2800 5200
$Comp
L power:+5V #PWR019
U 1 1 5B795C89
P 2800 3700
F 0 "#PWR019" H 2800 3925 30  0001 C CNN
F 1 "+5V" V 2725 3700 30  0001 C CNN
F 2 "" H 2800 3700 60  0000 C CNN
F 3 "" H 2800 3700 60  0000 C CNN
	1    2800 3700
	-1   0    0    -1  
$EndComp
$Comp
L passive:RESISTOR R9
U 1 1 5B795DFE
P 2800 3950
F 0 "R9" H 2800 4035 60  0000 C CNB
F 1 "10k" H 2800 3875 40  0000 C CNN
F 2 "smt:R-0603" H 2800 3810 40  0001 C CNN
F 3 "" H 2800 4050 60  0000 C CNN
F 4 "-" H 2800 4110 45  0001 C CNN "Part"
F 5 "Passive" H 3030 4240 50  0001 C CNN "Family"
	1    2800 3950
	0    1    -1   0   
$EndComp
Wire Wire Line
	2800 3700 2800 3750
$Comp
L passive:RESISTOR R6
U 1 1 5B7977F3
P 3350 4950
F 0 "R6" H 3350 5035 60  0000 C CNB
F 1 "10k" H 3350 4875 40  0000 C CNN
F 2 "smt:R-0603" H 3350 4810 40  0001 C CNN
F 3 "" H 3350 5050 60  0000 C CNN
F 4 "-" H 3350 5110 45  0001 C CNN "Part"
F 5 "Passive" H 3580 5240 50  0001 C CNN "Family"
	1    3350 4950
	0    1    -1   0   
$EndComp
$Comp
L power:+5V #PWR020
U 1 1 5B7978C5
P 3350 4800
F 0 "#PWR020" H 3350 5025 30  0001 C CNN
F 1 "+5V" V 3275 4800 30  0001 C CNN
F 2 "" H 3350 4800 60  0000 C CNN
F 3 "" H 3350 4800 60  0000 C CNN
	1    3350 4800
	-1   0    0    -1  
$EndComp
Wire Wire Line
	3350 4800 3350 4850
Wire Wire Line
	3350 5050 3350 5200
$Comp
L passive:CAP C12
U 1 1 5B79A2B9
P 2350 4550
F 0 "C12" H 2380 4640 60  0000 L CNB
F 1 "22pF" H 2380 4460 45  0000 L CNN
F 2 "smt:C-0603" H 2385 4395 40  0001 L CNN
F 3 "" H 2350 4650 60  0001 C CNN
F 4 "-" H 2390 4715 45  0001 L CNN "Part"
F 5 "Passive" H 2580 4840 50  0001 C CNN "Family"
	1    2350 4550
	-1   0    0    -1  
$EndComp
$Comp
L power:GND #PWR021
U 1 1 5B79A9CE
P 2350 4700
F 0 "#PWR021" H 2350 4750 30  0001 C CNN
F 1 "GND" H 2350 4575 30  0001 C CNN
F 2 "" H 2350 4700 60  0000 C CNN
F 3 "" H 2350 4700 60  0000 C CNN
	1    2350 4700
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2350 4650 2350 4700
Wire Wire Line
	2350 4450 2350 4300
Text Label 4200 6150 0    60   ~ 0
CFG\~RST
Wire Wire Line
	4950 7150 4950 7250
Wire Wire Line
	4950 6950 4950 7150
Wire Wire Line
	4950 6850 4950 6950
Wire Wire Line
	4950 6750 4950 6850
Wire Wire Line
	4900 2350 5050 2350
Wire Wire Line
	4900 2750 5050 2750
Wire Wire Line
	3900 2750 3900 2950
Wire Wire Line
	4550 2350 4900 2350
Wire Wire Line
	4550 2750 4900 2750
Wire Wire Line
	2800 5200 2600 5200
Wire Wire Line
	7250 4450 8100 4450
Text Label 8100 5150 2    60   ~ 0
RTC_INT
Wire Wire Line
	2800 4050 2800 4300
Wire Wire Line
	2800 4300 2350 4300
Connection ~ 2800 4300
Wire Wire Line
	2800 4300 2800 4650
Connection ~ 2350 4300
Wire Wire Line
	1250 3950 1700 3950
Wire Wire Line
	1250 3850 1700 3850
Text Label 1700 3850 2    60   ~ 0
PE1
Text Label 1700 3950 2    60   ~ 0
PE0
Wire Wire Line
	7250 5450 8100 5450
Wire Wire Line
	7250 5550 8100 5550
Text Label 8100 5550 2    60   ~ 0
PE0
Text Label 8100 5450 2    60   ~ 0
PE1
Wire Wire Line
	7250 2250 8100 2250
Text Label 8100 2250 2    60   ~ 0
WEN
Wire Wire Line
	7250 5150 8100 5150
Text Label 8100 4450 2    60   ~ 0
WIRQ
Wire Wire Line
	7250 2350 8100 2350
Text Label 8100 2350 2    60   ~ 0
WRST
Wire Wire Line
	7250 2150 8100 2150
Text Label 8100 2150 2    60   ~ 0
WCS
Wire Wire Line
	7250 2450 8100 2450
Text Label 8100 2450 2    60   ~ 0
WWAKE
$Comp
L power:+5V #PWR0117
U 1 1 5B9002F3
P 4850 1000
F 0 "#PWR0117" H 4850 1225 30  0001 C CNN
F 1 "+5V" V 4775 1000 30  0001 C CNN
F 2 "" H 4850 1000 60  0000 C CNN
F 3 "" H 4850 1000 60  0000 C CNN
	1    4850 1000
	1    0    0    -1  
$EndComp
Wire Wire Line
	4850 1000 4850 1250
Wire Wire Line
	4850 1250 5050 1250
Wire Wire Line
	4850 1250 4850 1350
Wire Wire Line
	4850 1350 5050 1350
Connection ~ 4850 1250
Wire Wire Line
	4850 1350 4850 1450
Wire Wire Line
	4850 1450 5050 1450
Connection ~ 4850 1350
Wire Wire Line
	4850 1450 4850 1550
Wire Wire Line
	4850 1550 5050 1550
Connection ~ 4850 1450
$Comp
L connectors:Hole H1
U 1 1 5B7CBD41
P 1450 6950
AR Path="/5B7CBD41" Ref="H1"  Part="1" 
AR Path="/5B9219B1/5B7CBD41" Ref="H?"  Part="1" 
F 0 "H1" H 1400 7050 60  0000 L BNB
F 1 "Hole" H 1500 6850 40  0001 L TNN
F 2 "conn-wire-pads:Hole-Screw-#4" H 1500 6775 40  0001 L TNN
F 3 "" H 1400 7025 60  0001 C CNN
F 4 "Virtual" H 1400 7175 40  0001 L BNN "Family"
F 5 "-" H 0   50  50  0001 C CNN "Part"
	1    1450 6950
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR09
U 1 1 5B7CBD48
P 1450 7150
AR Path="/5B7CBD48" Ref="#PWR09"  Part="1" 
AR Path="/5B9219B1/5B7CBD48" Ref="#PWR?"  Part="1" 
F 0 "#PWR09" H 1450 7200 30  0001 C CNN
F 1 "GND" H 1450 7025 30  0001 C CNN
F 2 "" H 1450 7150 60  0000 C CNN
F 3 "" H 1450 7150 60  0000 C CNN
	1    1450 7150
	1    0    0    -1  
$EndComp
Wire Wire Line
	1450 7100 1450 7150
$Comp
L connectors:Hole H2
U 1 1 5B7CBD50
P 1750 6950
AR Path="/5B7CBD50" Ref="H2"  Part="1" 
AR Path="/5B9219B1/5B7CBD50" Ref="H?"  Part="1" 
F 0 "H2" H 1700 7050 60  0000 L BNB
F 1 "Hole" H 1800 6850 40  0001 L TNN
F 2 "conn-wire-pads:Hole-Screw-#4" H 1800 6775 40  0001 L TNN
F 3 "" H 1700 7025 60  0001 C CNN
F 4 "Virtual" H 1700 7175 40  0001 L BNN "Family"
F 5 "-" H 0   50  50  0001 C CNN "Part"
	1    1750 6950
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR010
U 1 1 5B7CBD57
P 1750 7150
AR Path="/5B7CBD57" Ref="#PWR010"  Part="1" 
AR Path="/5B9219B1/5B7CBD57" Ref="#PWR?"  Part="1" 
F 0 "#PWR010" H 1750 7200 30  0001 C CNN
F 1 "GND" H 1750 7025 30  0001 C CNN
F 2 "" H 1750 7150 60  0000 C CNN
F 3 "" H 1750 7150 60  0000 C CNN
	1    1750 7150
	1    0    0    -1  
$EndComp
Wire Wire Line
	1750 7100 1750 7150
$Comp
L connectors:Hole H3
U 1 1 5B7CBD5F
P 2050 6950
AR Path="/5B7CBD5F" Ref="H3"  Part="1" 
AR Path="/5B9219B1/5B7CBD5F" Ref="H?"  Part="1" 
F 0 "H3" H 2000 7050 60  0000 L BNB
F 1 "Hole" H 2100 6850 40  0001 L TNN
F 2 "conn-wire-pads:Hole-Screw-#4" H 2100 6775 40  0001 L TNN
F 3 "" H 2000 7025 60  0001 C CNN
F 4 "Virtual" H 2000 7175 40  0001 L BNN "Family"
F 5 "-" H 0   50  50  0001 C CNN "Part"
	1    2050 6950
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR012
U 1 1 5B7CBD66
P 2050 7150
AR Path="/5B7CBD66" Ref="#PWR012"  Part="1" 
AR Path="/5B9219B1/5B7CBD66" Ref="#PWR?"  Part="1" 
F 0 "#PWR012" H 2050 7200 30  0001 C CNN
F 1 "GND" H 2050 7025 30  0001 C CNN
F 2 "" H 2050 7150 60  0000 C CNN
F 3 "" H 2050 7150 60  0000 C CNN
	1    2050 7150
	1    0    0    -1  
$EndComp
Wire Wire Line
	2050 7100 2050 7150
Wire Wire Line
	9300 3350 8650 3350
Wire Wire Line
	9300 3450 8650 3450
Wire Wire Line
	9300 3550 8650 3550
Wire Wire Line
	9300 3750 8650 3750
Wire Wire Line
	9300 3850 8650 3850
Wire Wire Line
	9300 3950 8650 3950
Wire Wire Line
	9300 4050 8650 4050
Wire Wire Line
	9300 4150 8650 4150
Wire Wire Line
	9300 4350 8650 4350
Wire Wire Line
	9300 4550 8650 4550
Wire Wire Line
	9300 4650 8650 4650
Wire Wire Line
	9300 4750 8650 4750
Wire Wire Line
	9300 4850 8650 4850
Wire Wire Line
	9300 4950 8650 4950
Wire Wire Line
	9300 5150 8650 5150
Wire Wire Line
	9300 5250 8650 5250
Wire Wire Line
	9300 5350 8650 5350
Wire Wire Line
	9300 5650 8650 5650
Wire Wire Line
	9300 5750 8650 5750
Wire Wire Line
	9300 5950 8650 5950
Wire Wire Line
	9300 6050 8650 6050
Text Label 8650 3350 0    60   ~ 0
MOSI
Text Label 8650 3450 0    60   ~ 0
MISO
Text Label 8650 3550 0    60   ~ 0
SCK
Text Label 8650 3750 0    60   ~ 0
SDCS
Text Label 8650 3850 0    60   ~ 0
CRST
Text Label 8650 3950 0    60   ~ 0
DREQ
Text Label 8650 4050 0    60   ~ 0
CCS
Text Label 8650 4150 0    60   ~ 0
XDCS
Text Label 8650 4350 0    60   ~ 0
A_SHDN
Text Label 8650 4550 0    60   ~ 0
WEN
Text Label 8650 4650 0    60   ~ 0
WRST
Text Label 8650 4750 0    60   ~ 0
WCS
Text Label 8650 4850 0    60   ~ 0
WWAKE
Text Label 8650 4950 0    60   ~ 0
WIRQ
Text Label 8650 5150 0    60   ~ 0
PIX_CLK
Text Label 8650 5250 0    60   ~ 0
PIX_LAMP
Text Label 8650 5350 0    60   ~ 0
PIX_SHDN
Text Label 8650 5650 0    60   ~ 0
SDA
Text Label 8650 5750 0    60   ~ 0
SCL
Text Label 8650 5950 0    60   ~ 0
KCHG
Text Label 8650 6050 0    60   ~ 0
RTC_INT
Wire Notes Line
	3700 5900 1850 5900
Wire Notes Line
	3700 3450 3700 5900
Text Notes 3250 3400 2    70   ~ 14
RESET / FACTORY CONFIG
Wire Wire Line
	5050 6350 4200 6350
Text Label 4200 6350 0    60   ~ 0
CCS
Wire Wire Line
	5050 5950 4200 5950
Text Label 4200 5950 0    60   ~ 0
ALS_INT
Wire Wire Line
	7250 6650 8100 6650
Text Label 8100 6650 2    60   ~ 0
SDCS
Wire Wire Line
	5050 5850 4200 5850
Text Label 4200 5850 0    60   ~ 0
CRST
Wire Wire Line
	7250 5050 8100 5050
Text Label 8100 4350 2    60   ~ 0
DREQ
Wire Wire Line
	7250 4350 8100 4350
Text Label 8100 5050 2    60   ~ 0
KCHG
Wire Wire Line
	7250 1350 8100 1350
Text Label 8100 1350 2    60   ~ 0
A_SHDN
Text Label 8100 3750 2    60   ~ 0
PIX_CLK
Wire Wire Line
	7250 3550 8100 3550
Text Label 8100 3550 2    60   ~ 0
PIX_LAMP
Wire Wire Line
	7250 3050 8100 3050
Text Label 8100 3050 2    60   ~ 0
PIX_SHDN
$Comp
L passive:RESISTOR R20
U 1 1 5BC4AFBA
P 10000 1250
F 0 "R20" H 10000 1335 60  0000 C CNB
F 1 "10k" H 10000 1175 40  0000 C CNN
F 2 "smt:R-0603" H 10000 1110 40  0001 C CNN
F 3 "" H 10000 1350 60  0000 C CNN
F 4 "-" H 10000 1410 45  0001 C CNN "Part"
F 5 "Passive" H 10230 1540 50  0001 C CNN "Family"
	1    10000 1250
	0    -1   -1   0   
$EndComp
$Comp
L passive:RESISTOR R21
U 1 1 5BC4B1D0
P 10300 1250
F 0 "R21" H 10300 1335 60  0000 C CNB
F 1 "10k" H 10300 1175 40  0000 C CNN
F 2 "smt:R-0603" H 10300 1110 40  0001 C CNN
F 3 "" H 10300 1350 60  0000 C CNN
F 4 "-" H 10300 1410 45  0001 C CNN "Part"
F 5 "Passive" H 10530 1540 50  0001 C CNN "Family"
	1    10300 1250
	0    -1   -1   0   
$EndComp
$Comp
L power:+5V #PWR0129
U 1 1 5BC4B263
P 10000 1100
F 0 "#PWR0129" H 10000 1325 30  0001 C CNN
F 1 "+5V" V 9925 1100 30  0001 C CNN
F 2 "" H 10000 1100 60  0000 C CNN
F 3 "" H 10000 1100 60  0000 C CNN
	1    10000 1100
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0130
U 1 1 5BC4B2AD
P 10300 1100
F 0 "#PWR0130" H 10300 1325 30  0001 C CNN
F 1 "+5V" V 10225 1100 30  0001 C CNN
F 2 "" H 10300 1100 60  0000 C CNN
F 3 "" H 10300 1100 60  0000 C CNN
	1    10300 1100
	1    0    0    -1  
$EndComp
Wire Wire Line
	10000 1100 10000 1150
Wire Wire Line
	10300 1100 10300 1150
Wire Wire Line
	10000 1350 10000 1450
Wire Wire Line
	10000 1450 9600 1450
Wire Wire Line
	10300 1350 10300 1550
Wire Wire Line
	10300 1550 9600 1550
Text Label 9600 1450 0    60   ~ 0
SDA
Text Label 9600 1550 0    60   ~ 0
SCL
$Sheet
S 9300 3250 1000 3350
U 579732DD
F0 "Peripherals" 60
F1 "peripherals.sch" 60
F2 "RTC_INT" O L 9300 6050 60 
F3 "SCL" I L 9300 5750 60 
F4 "SDA" T L 9300 5650 60 
F5 "CCS" I L 9300 4050 60 
F6 "XDCS" I L 9300 4150 60 
F7 "CRST" I L 9300 3850 60 
F8 "SDCS" I L 9300 3750 60 
F9 "WEN" I L 9300 4550 60 
F10 "WRST" I L 9300 4650 60 
F11 "SCK" I L 9300 3550 60 
F12 "MISO" O L 9300 3450 60 
F13 "WCS" I L 9300 4750 60 
F14 "MOSI" I L 9300 3350 60 
F15 "WWAKE" I L 9300 4850 60 
F16 "WIRQ" O L 9300 4950 60 
F17 "KCHG" O L 9300 5950 60 
F18 "PIX_LAMP" I L 9300 5250 60 
F19 "A_SHDN" I L 9300 4350 60 
F20 "PIX_CLK" I L 9300 5150 60 
F21 "DREQ" O L 9300 3950 60 
F22 "PIX_SHDN" I L 9300 5350 60 
F23 "ALRM_SW" U L 9300 6450 60 
F24 "SD_DET" O L 9300 4250 60 
F25 "OLRST" I L 9300 6150 60 
F26 "ALS_INT" O L 9300 6250 60 
$EndSheet
Text Label 8650 6450 0    60   ~ 0
ALRM_SW
NoConn ~ 7250 4950
NoConn ~ 7250 4850
NoConn ~ 7250 4050
NoConn ~ 7250 4150
NoConn ~ 7250 4250
NoConn ~ 5050 4850
NoConn ~ 5050 4950
NoConn ~ 5050 5050
NoConn ~ 5050 5250
NoConn ~ 5050 5350
$Comp
L passive:CAP C24
U 1 1 5BD2BF68
P 4400 950
F 0 "C24" H 4430 1040 60  0000 L CNB
F 1 "100nF" H 4430 860 45  0000 L CNN
F 2 "smt:C-0603" H 4435 795 40  0001 L CNN
F 3 "" H 4400 1050 60  0001 C CNN
F 4 "-" H 4440 1115 45  0001 L CNN "Part"
F 5 "Passive" H 4630 1240 50  0001 C CNN "Family"
	1    4400 950 
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0133
U 1 1 5BD2C40C
P 2850 1850
F 0 "#PWR0133" H 2850 1900 30  0001 C CNN
F 1 "GND" H 2850 1725 30  0001 C CNN
F 2 "" H 2850 1850 60  0000 C CNN
F 3 "" H 2850 1850 60  0000 C CNN
	1    2850 1850
	1    0    0    -1  
$EndComp
Wire Wire Line
	5050 1750 4600 1750
Wire Wire Line
	4850 1550 4850 1850
Wire Wire Line
	4850 1850 5050 1850
Connection ~ 4850 1550
$Comp
L passive:CAP C23
U 1 1 5BD49EE5
P 3900 1450
F 0 "C23" H 3930 1540 60  0000 L CNB
F 1 "100nF" H 3930 1360 45  0000 L CNN
F 2 "smt:C-0603" H 3935 1295 40  0001 L CNN
F 3 "" H 3900 1550 60  0001 C CNN
F 4 "-" H 3940 1615 45  0001 L CNN "Part"
F 5 "Passive" H 4130 1740 50  0001 C CNN "Family"
	1    3900 1450
	1    0    0    -1  
$EndComp
$Comp
L passive:CAP C22
U 1 1 5BD49F41
P 3550 1450
F 0 "C22" H 3580 1540 60  0000 L CNB
F 1 "100nF" H 3580 1360 45  0000 L CNN
F 2 "smt:C-0603" H 3585 1295 40  0001 L CNN
F 3 "" H 3550 1550 60  0001 C CNN
F 4 "-" H 3590 1615 45  0001 L CNN "Part"
F 5 "Passive" H 3780 1740 50  0001 C CNN "Family"
	1    3550 1450
	1    0    0    -1  
$EndComp
Wire Wire Line
	4850 1250 4250 1250
Wire Wire Line
	4250 1250 4250 1350
Wire Wire Line
	4250 1250 3900 1250
Wire Wire Line
	3900 1250 3900 1350
Connection ~ 4250 1250
Wire Wire Line
	3900 1250 3550 1250
Wire Wire Line
	3550 1250 3550 1350
Connection ~ 3900 1250
Wire Wire Line
	4250 1550 4250 1750
NoConn ~ 7250 5350
Wire Wire Line
	7250 5250 8100 5250
Text Label 8100 5250 2    60   ~ 0
OLRST
$Comp
L passive:CAP C25
U 1 1 5BD9D1D4
P 3200 1450
F 0 "C25" H 3230 1540 60  0000 L CNB
F 1 "100nF" H 3230 1360 45  0000 L CNN
F 2 "smt:C-0603" H 3235 1295 40  0001 L CNN
F 3 "" H 3200 1550 60  0001 C CNN
F 4 "-" H 3240 1615 45  0001 L CNN "Part"
F 5 "Passive" H 3430 1740 50  0001 C CNN "Family"
	1    3200 1450
	1    0    0    -1  
$EndComp
Wire Wire Line
	3550 1250 3200 1250
Wire Wire Line
	3200 1250 3200 1350
Connection ~ 3550 1250
Wire Wire Line
	7250 1250 8100 1250
Text Label 8100 1250 2    60   ~ 0
XDCS
$Comp
L passive:CAP C26
U 1 1 5BE45E12
P 2850 1450
F 0 "C26" H 2880 1540 60  0000 L CNB
F 1 "100nF" H 2880 1360 45  0000 L CNN
F 2 "smt:C-0603" H 2885 1295 40  0001 L CNN
F 3 "" H 2850 1550 60  0001 C CNN
F 4 "-" H 2890 1615 45  0001 L CNN "Part"
F 5 "Passive" H 3080 1740 50  0001 C CNN "Family"
	1    2850 1450
	1    0    0    -1  
$EndComp
Wire Wire Line
	3200 1250 2850 1250
Wire Wire Line
	2850 1250 2850 1350
Connection ~ 3200 1250
Wire Wire Line
	2850 1750 2850 1550
Wire Wire Line
	3200 1750 3200 1550
Wire Wire Line
	3550 1550 3550 1750
Connection ~ 3550 1750
Wire Wire Line
	3900 1550 3900 1750
Connection ~ 3900 1750
Wire Wire Line
	3550 1750 3900 1750
Wire Wire Line
	3900 1750 4250 1750
Wire Wire Line
	3550 1750 3200 1750
Wire Wire Line
	3200 1750 2850 1750
Connection ~ 3200 1750
Wire Wire Line
	2850 1750 2850 1850
Connection ~ 2850 1750
Wire Wire Line
	7250 1650 8100 1650
Text Label 8100 1650 2    60   ~ 0
ALRM_SW
Wire Wire Line
	5050 4650 4200 4650
Text Label 4200 4650 0    60   ~ 0
SYSOFF
Wire Wire Line
	2800 4300 3450 4300
Wire Wire Line
	9300 4250 8650 4250
Text Label 8650 4250 0    60   ~ 0
SD_DET
Wire Wire Line
	8650 6450 9300 6450
Wire Wire Line
	7250 3350 8100 3350
Text Label 8100 3350 2    60   ~ 0
SD_DET
Wire Wire Line
	5050 3150 4750 3150
Wire Wire Line
	5050 3050 4750 3050
Text Label 5000 3050 2    60   ~ 0
P42
Text Label 5000 3150 2    60   ~ 0
P43
Wire Wire Line
	4750 3250 5050 3250
Text Label 5000 3250 2    60   ~ 0
P44
NoConn ~ 5050 3550
NoConn ~ 5050 3650
NoConn ~ 5050 3750
$Comp
L connectors:HEADER-1x05 J14
U 1 1 5C09E810
P 4600 3250
F 0 "J14" H 4550 3550 60  0000 L BNB
F 1 "EXT" V 4750 3250 40  0000 C CNN
F 2 "conn-wire-pads:WP-40mil-1x05" H 4600 3250 60  0001 C CNN
F 3 "" H 4600 3250 60  0001 C CNN
F 4 "-" H 4550 3650 40  0001 L BNN "Part"
F 5 "Virtual" H 4550 3750 40  0001 L BNN "Family"
	1    4600 3250
	-1   0    0    -1  
$EndComp
Wire Wire Line
	4750 3350 5050 3350
Wire Wire Line
	5050 3450 4750 3450
Text Label 5000 3350 2    60   ~ 0
P45
Text Label 5000 3450 2    60   ~ 0
P46
$Comp
L connectors:HEADER-1x02 J15
U 1 1 5C0C02B9
P 3350 7500
F 0 "J15" H 3300 7650 60  0000 L BNB
F 1 "EXT-PW" V 3500 7500 40  0000 C CNN
F 2 "conn-wire-pads:WP-50mil-1x02" H 3350 7500 60  0001 C CNN
F 3 "" H 3350 7500 60  0001 C CNN
F 4 "-" H 3300 7750 40  0001 L BNN "Part"
F 5 "Virtual" H 3300 7850 40  0001 L BNN "Family"
	1    3350 7500
	-1   0    0    -1  
$EndComp
$Comp
L power:+5V #PWR014
U 1 1 5C0C03CF
P 3750 7350
F 0 "#PWR014" H 3750 7575 30  0001 C CNN
F 1 "+5V" V 3675 7350 30  0001 C CNN
F 2 "" H 3750 7350 60  0000 C CNN
F 3 "" H 3750 7350 60  0000 C CNN
	1    3750 7350
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR015
U 1 1 5C0C0420
P 3750 7650
F 0 "#PWR015" H 3750 7700 30  0001 C CNN
F 1 "GND" H 3750 7525 30  0001 C CNN
F 2 "" H 3750 7650 60  0000 C CNN
F 3 "" H 3750 7650 60  0000 C CNN
	1    3750 7650
	1    0    0    -1  
$EndComp
Wire Wire Line
	3500 7450 3750 7450
Wire Wire Line
	3750 7450 3750 7350
Wire Wire Line
	3500 7550 3750 7550
Wire Wire Line
	3750 7550 3750 7650
$Comp
L connectors:HEADER-1x02 J16
U 1 1 5C0D95B2
P 3350 6850
F 0 "J16" H 3300 7000 60  0000 L BNB
F 1 "EXT-I2C" V 3500 6850 40  0000 C CNN
F 2 "conn-wire-pads:WP-50mil-1x02" H 3350 6850 60  0001 C CNN
F 3 "" H 3350 6850 60  0001 C CNN
F 4 "-" H 3300 7100 40  0001 L BNN "Part"
F 5 "Virtual" H 3300 7200 40  0001 L BNN "Family"
	1    3350 6850
	-1   0    0    -1  
$EndComp
Wire Wire Line
	3500 6800 3850 6800
Wire Wire Line
	3500 6900 3850 6900
Text Label 3850 6800 2    60   ~ 0
SDA
Text Label 3850 6900 2    60   ~ 0
SCL
NoConn ~ 5050 6450
NoConn ~ 5050 6250
NoConn ~ 5050 6050
NoConn ~ 5050 5750
NoConn ~ 5050 5550
NoConn ~ 7250 6450
NoConn ~ 7250 6350
NoConn ~ 7250 6250
NoConn ~ 7250 6150
NoConn ~ 7250 6050
NoConn ~ 7250 5950
NoConn ~ 7250 5850
NoConn ~ 7250 5750
NoConn ~ 5050 3950
NoConn ~ 5050 4350
NoConn ~ 5050 4150
NoConn ~ 5050 4250
NoConn ~ 5050 4450
NoConn ~ 5050 4550
NoConn ~ 7250 3150
NoConn ~ 7250 3250
NoConn ~ 7250 3450
NoConn ~ 7250 3650
NoConn ~ 7250 2850
NoConn ~ 7250 1750
NoConn ~ 7250 1850
NoConn ~ 7250 1950
NoConn ~ 7250 3950
NoConn ~ 5050 5450
NoConn ~ 700  6700
NoConn ~ 700  7050
NoConn ~ 700  7350
NoConn ~ 7250 6750
NoConn ~ 7250 6850
NoConn ~ 7250 6950
NoConn ~ 7250 7050
NoConn ~ 7250 7150
Text Notes 7350 1250 0    60   ~ 0
P29
Text Notes 7350 1350 0    60   ~ 0
P28
Text Notes 4950 4650 2    60   ~ 0
A8
Text Notes 7350 1650 0    60   ~ 0
P25
Text Notes 7350 2150 0    60   ~ 0
P13
Text Notes 7350 2250 0    60   ~ 0
P12
Text Notes 7350 2350 0    60   ~ 0
P11
Text Notes 7350 2450 0    60   ~ 0
P10
Text Notes 7350 2550 0    60   ~ 0
P50
Text Notes 7350 2650 0    60   ~ 0
P51
Text Notes 7350 2750 0    60   ~ 0
P52
Text Notes 7350 3050 0    60   ~ 0
P30
Text Notes 7350 3350 0    60   ~ 0
P33
Text Notes 7350 3550 0    60   ~ 0
P35
Text Notes 7350 3750 0    60   ~ 0
P37
Text Notes 7350 4350 0    60   ~ 0
P18
Text Notes 7350 4450 0    60   ~ 0
P19
Text Notes 7350 4550 0    60   ~ 0
P20
Text Notes 7350 4650 0    60   ~ 0
P21
Text Notes 7350 5050 0    60   ~ 0
P3
Text Notes 7350 5150 0    60   ~ 0
P2
Text Notes 7350 5250 0    60   ~ 0
P5
Text Notes 7350 5450 0    60   ~ 0
P1
Text Notes 7350 5550 0    60   ~ 0
P0
Text Notes 7350 6650 0    60   ~ 0
P4
Text Notes 4950 5850 2    60   ~ 0
P9
Text Notes 4950 5950 2    60   ~ 0
P8
Text Notes 4950 6150 2    60   ~ 0
P6
Text Notes 4950 6350 2    60   ~ 0
P16
Text Notes 9650 2000 0    70   ~ 14
ICSP
Wire Notes Line
	3700 3450 1850 3450
Wire Notes Line
	1850 3450 1850 5900
Connection ~ 3350 5200
Wire Wire Line
	3350 5200 3200 5200
$Comp
L passive:RESISTOR R23
U 1 1 5BD0160F
P 3100 5200
F 0 "R23" H 3100 5285 60  0000 C CNB
F 1 "1k" H 3100 5125 40  0000 C CNN
F 2 "smt:R-0603" H 3100 5060 40  0001 C CNN
F 3 "" H 3100 5300 60  0000 C CNN
F 4 "-" H 3100 5360 45  0001 C CNN "Part"
F 5 "Passive" H 3330 5490 50  0001 C CNN "Family"
	1    3100 5200
	-1   0    0    -1  
$EndComp
NoConn ~ 7250 1450
$Comp
L passive:DIODE D1
U 1 1 5E5438A4
P 2350 3950
F 0 "D1" H 2350 4060 60  0000 C CNB
F 1 "CD1206-S01575" H 2350 3850 40  0000 C CNN
F 2 "smt:D-1206" H 2350 3790 40  0001 C CNN
F 3 "" H 2350 4070 60  0000 C CNN
F 4 "CD1206-S01575" H 2355 4125 45  0001 C CNN "Part"
F 5 "Passive" H 2580 4240 50  0001 C CNN "Family"
	1    2350 3950
	0    1    -1   0   
$EndComp
Wire Wire Line
	2350 4050 2350 4300
Wire Wire Line
	2800 3750 2350 3750
Wire Wire Line
	2350 3750 2350 3850
Connection ~ 2800 3750
Wire Wire Line
	2800 3750 2800 3850
$Comp
L connectors:TEST-POINT TP1
U 1 1 5E5A3C61
P 4100 650
F 0 "TP1" H 4050 725 60  0000 L BNB
F 1 "AREF" H 4050 550 40  0000 L TNN
F 2 "conn-wire-pads:TEST-POINT-50" H 3950 625 60  0001 C CNN
F 3 "" H 4050 725 60  0001 C CNN
F 4 "Virtual" H 4050 850 40  0001 L BNN "Family"
	1    4100 650 
	-1   0    0    -1  
$EndComp
Wire Wire Line
	4600 950  4500 950 
Wire Wire Line
	4250 650  4600 650 
Wire Wire Line
	4600 650  4600 950 
$Comp
L power:GND #PWR0138
U 1 1 5E621168
P 4250 1000
F 0 "#PWR0138" H 4250 1050 30  0001 C CNN
F 1 "GND" H 4250 875 30  0001 C CNN
F 2 "" H 4250 1000 60  0000 C CNN
F 3 "" H 4250 1000 60  0000 C CNN
	1    4250 1000
	1    0    0    -1  
$EndComp
Wire Wire Line
	4300 950  4250 950 
Wire Wire Line
	4250 950  4250 1000
$Comp
L connectors:HEADER-1x01 J11
U 1 1 5E669EC3
P 2450 5700
F 0 "J11" H 2400 5800 60  0000 L BNB
F 1 "RESET-SW" V 2650 5700 40  0000 C CNN
F 2 "conn-wire-pads:WP-50mil-1x01" H 2300 5675 60  0001 C CNN
F 3 "" H 2400 5775 60  0001 C CNN
F 4 "-" H 2400 5900 40  0001 L BNN "Part"
F 5 "Virtual" H 2400 6000 40  0001 L BNN "Family"
	1    2450 5700
	-1   0    0    -1  
$EndComp
$Comp
L passive:CAP C6
U 1 1 5E8E40BC
P 1650 4300
F 0 "C6" H 1680 4390 60  0000 L CNB
F 1 "100nF" H 1680 4210 45  0000 L CNN
F 2 "smt:C-0603" H 1680 4140 40  0001 L CNN
F 3 "" H 1680 4060 45  0001 L CNN
F 4 "-" H 1690 4465 45  0001 L CNN "Part"
F 5 "Passive" H 1690 4520 50  0001 L CNN "Family"
	1    1650 4300
	0    -1   -1   0   
$EndComp
Wire Wire Line
	1550 4300 1250 4300
Wire Wire Line
	1750 4300 2350 4300
Wire Wire Line
	2600 5700 2800 5700
Wire Wire Line
	2800 5200 2800 5700
Wire Wire Line
	3450 2050 3450 2600
Wire Wire Line
	3450 2050 5050 2050
Wire Wire Line
	3900 6150 3900 5200
Wire Wire Line
	3900 6150 5050 6150
Wire Wire Line
	3900 5200 3350 5200
Text Label 4500 2050 0    60   ~ 0
RESET
Wire Wire Line
	7250 3750 8100 3750
$Comp
L connectors:TEST-POINT TP2
U 1 1 5BA42DFD
P 3100 2600
F 0 "TP2" H 3050 2675 60  0000 L BNB
F 1 "RESET" H 3050 2500 40  0000 L TNN
F 2 "conn-wire-pads:TEST-POINT-50" H 2950 2575 60  0001 C CNN
F 3 "" H 3050 2675 60  0001 C CNN
F 4 "Virtual" H 3050 2800 40  0001 L BNN "Family"
	1    3100 2600
	-1   0    0    -1  
$EndComp
Wire Wire Line
	3250 2600 3450 2600
Connection ~ 3450 2600
Wire Wire Line
	3450 2600 3450 4300
Wire Wire Line
	5050 4050 4200 4050
Text Label 4200 4050 0    60   ~ 0
ON_BATT
Text Notes 4950 4050 2    60   ~ 0
A14
NoConn ~ 7250 1550
Wire Wire Line
	9300 6150 8650 6150
Wire Wire Line
	9300 6250 8650 6250
Text Label 8650 6150 0    60   ~ 0
OLRST
Text Label 8650 6250 0    60   ~ 0
ALS_INT
$Comp
L connectors:HEADER-1x04 J6
U 1 1 5BA694FC
P 950 2550
F 0 "J6" H 900 2800 60  0000 L BNB
F 1 "PSU-SIG" V 1100 2550 40  0000 C CNN
F 2 "conn-header:HDR-M-1x04" H 950 2550 60  0001 C CNN
F 3 "" H 950 2550 60  0001 C CNN
F 4 "BCS-104-L-S-TE" H 900 2900 40  0001 L BNN "Part"
F 5 "Connector" H 900 3000 40  0001 L BNN "Family"
	1    950  2550
	-1   0    0    -1  
$EndComp
Text Label 1650 2700 2    60   ~ 0
SCL
Text Label 1650 2600 2    60   ~ 0
SDA
Wire Wire Line
	1100 2700 1650 2700
Wire Wire Line
	1100 2600 1650 2600
Text Notes 1450 900  0    80   ~ 16
PSU
Wire Notes Line
	600  2950 600  950 
Wire Notes Line
	2600 2950 600  2950
Wire Notes Line
	2600 950  2600 2950
Wire Notes Line
	600  950  2600 950 
$Comp
L power:SUP_+5V #PWR0113
U 1 1 5C5FF363
P 2350 1500
F 0 "#PWR0113" H 2350 1725 30  0001 C CNN
F 1 "SUP_+5V" V 2275 1500 30  0001 C CNN
F 2 "" H 2250 1625 60  0001 C CNN
F 3 "" H 2350 1725 60  0001 C CNN
	1    2350 1500
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0110
U 1 1 5C5FE9DC
P 1350 1600
AR Path="/5C5FE9DC" Ref="#PWR0110"  Part="1" 
AR Path="/5B9219B1/5C5FE9DC" Ref="#PWR?"  Part="1" 
F 0 "#PWR0110" H 1350 1650 30  0001 C CNN
F 1 "GND" H 1350 1475 30  0001 C CNN
F 2 "" H 1350 1600 60  0000 C CNN
F 3 "" H 1350 1600 60  0000 C CNN
	1    1350 1600
	1    0    0    -1  
$EndComp
Text Label 1650 2500 2    60   ~ 0
ON_BATT
Text Label 1650 2400 2    60   ~ 0
SYSOFF
Wire Wire Line
	1100 2500 1650 2500
Wire Wire Line
	1100 2400 1650 2400
Wire Wire Line
	2350 1800 2300 1800
Wire Wire Line
	1850 1700 1900 1700
$Comp
L connectors:HEADER-2x03 J10
U 1 1 5B7CBD03
P 2100 1800
AR Path="/5B7CBD03" Ref="J10"  Part="1" 
AR Path="/5B9219B1/5B7CBD03" Ref="J?"  Part="1" 
F 0 "J10" H 2000 2000 60  0000 L BNB
F 1 "PSU" H 2000 1600 40  0000 L TNN
F 2 "conn-header:HDR-M-2x03" H 2100 600 60  0001 C CNN
F 3 "" H 2100 600 60  0001 C CNN
F 4 "BCS-103-L-D-TE" H 2000 2100 40  0001 L BNN "Part"
F 5 "Connector" H 2000 2200 40  0001 L BNN "Family"
	1    2100 1800
	1    0    0    -1  
$EndComp
Wire Wire Line
	2350 1900 2350 2050
Wire Wire Line
	2300 1900 2350 1900
Wire Wire Line
	1850 1900 1850 2050
Wire Wire Line
	1900 1900 1850 1900
Text Label 1300 1300 0    50   ~ 0
VIN
$Comp
L power:GND #PWR02
U 1 1 5B7CBCEF
P 1850 2050
AR Path="/5B7CBCEF" Ref="#PWR02"  Part="1" 
AR Path="/5B9219B1/5B7CBCEF" Ref="#PWR?"  Part="1" 
F 0 "#PWR02" H 1850 2100 30  0001 C CNN
F 1 "GND" H 1850 1925 30  0001 C CNN
F 2 "" H 1850 2050 60  0000 C CNN
F 3 "" H 1850 2050 60  0000 C CNN
	1    1850 2050
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR03
U 1 1 5B7CBCE9
P 2350 2050
AR Path="/5B7CBCE9" Ref="#PWR03"  Part="1" 
AR Path="/5B9219B1/5B7CBCE9" Ref="#PWR?"  Part="1" 
F 0 "#PWR03" H 2350 2100 30  0001 C CNN
F 1 "GND" H 2350 1925 30  0001 C CNN
F 2 "" H 2350 2050 60  0000 C CNN
F 3 "" H 2350 2050 60  0000 C CNN
	1    2350 2050
	1    0    0    -1  
$EndComp
Wire Wire Line
	1200 1400 1350 1400
Wire Wire Line
	1350 1500 1350 1600
Wire Wire Line
	1350 1400 1350 1500
Connection ~ 1350 1500
Wire Wire Line
	1200 1500 1350 1500
$Comp
L connectors:BARREL_JACK J1
U 1 1 5B7CBCD8
P 1000 1400
AR Path="/5B7CBCD8" Ref="J1"  Part="1" 
AR Path="/5B9219B1/5B7CBCD8" Ref="J?"  Part="1" 
F 0 "J1" H 900 1600 60  0000 L BNB
F 1 "-" H 900 1250 40  0001 L TNN
F 2 "conn-power:CUI-P1J-021-SMT" H 1050 1400 60  0001 C CNN
F 3 "" H 1050 1400 60  0001 C CNN
F 4 "PJ1-021-SMT-TR" H 800 1200 40  0000 L BNN "Part"
F 5 "Connector" H 900 1800 40  0001 L BNN "Family"
	1    1000 1400
	-1   0    0    -1  
$EndComp
Connection ~ 4600 950 
Wire Wire Line
	4600 950  4600 1750
Wire Wire Line
	1850 1300 1850 1700
Connection ~ 1850 1700
Wire Wire Line
	1850 1700 1850 1800
Wire Wire Line
	1850 1800 1900 1800
Wire Wire Line
	2300 1700 2350 1700
Wire Wire Line
	2350 1500 2350 1700
Connection ~ 2350 1700
Wire Wire Line
	2350 1700 2350 1800
Wire Wire Line
	1200 1300 1850 1300
$EndSCHEMATC
