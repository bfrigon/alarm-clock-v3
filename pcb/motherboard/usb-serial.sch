EESchema Schematic File Version 4
LIBS:motherboard-cache
EELAYER 26 0
EELAYER END
$Descr USLetter 11000 8500
encoding utf-8
Sheet 2 3
Title "Motherboard\\nUSB Serial"
Date "2018-10-29"
Rev "B"
Comp ""
Comment1 "CLKV3-PC-05"
Comment2 "CLKV3-SCH-05"
Comment3 "Alarm clock (V3)"
Comment4 ""
$EndDescr
$Comp
L connectors:USB-MINI-B J2
U 1 1 578C6CC0
P 1050 4900
F 0 "J2" H 800 5250 60  0000 L BNB
F 1 "USB-MINI-B" H 800 4450 40  0000 L TNN
F 2 "conn-io:USB-MB_UX60SC-MB-5ST" H 800 4375 40  0001 L TNN
F 3 "" H 800 4300 40  0001 L TNN
F 4 "UX60SC-MB-5ST" H 800 5350 40  0001 L BNN "Part"
F 5 "Connector" H 800 5450 40  0001 L BNN "Family"
	1    1050 4900
	-1   0    0    -1  
$EndComp
$Comp
L ic-cpu:ATMEGA8U2 U3
U 1 1 578C6E49
P 6600 4300
F 0 "U3" H 5650 5550 60  0000 L BNB
F 1 "ATMEGA16U2-AU" H 5650 3050 40  0000 L TNN
F 2 "smt-qfp:QFP-7-32" H 5650 2950 40  0001 L CNN
F 3 "" H 5650 5725 40  0001 L BNN
F 4 "IC" H 5650 5650 40  0001 L BNN "Family"
	1    6600 4300
	1    0    0    -1  
$EndComp
$Comp
L connectors:HEADER-2x03 J4
U 1 1 578C8B09
P 9500 3050
F 0 "J4" H 9400 3250 60  0000 L BNB
F 1 "ICSP" H 9350 2850 40  0000 L TNN
F 2 "conn-header:HDR-M-2x03" H 9500 1850 60  0001 C CNN
F 3 "" H 9500 1850 60  0001 C CNN
F 4 "CES-103-01-T-D" H 9400 3350 40  0001 L BNN "Part"
F 5 "Connector" H 9400 3450 40  0001 L BNN "Family"
	1    9500 3050
	1    0    0    -1  
$EndComp
$Comp
L passive:CRYSTAL Y1
U 1 1 578CB69A
P 5100 4100
F 0 "Y1" H 5100 4210 60  0000 C CNB
F 1 "16.00 MHz" H 5100 4000 40  0000 C CNN
F 2 "crystals:smt-CSM-3X-7x4mm" H 5100 3940 40  0001 C CNN
F 3 "" H 5100 4220 60  0000 C CNN
F 4 "ECS-160-20-3X-TR" V 5350 3900 45  0001 C CNN "Part"
F 5 "Passive" H 5330 4390 50  0001 C CNN "Family"
	1    5100 4100
	0    -1   -1   0   
$EndComp
$Comp
L passive:RESISTOR R3
U 1 1 578CBB56
P 3200 4800
F 0 "R3" H 3050 4850 60  0000 C CNB
F 1 "22" H 3350 4850 40  0000 C CNN
F 2 "smt:R-0603" H 3200 4660 40  0001 C CNN
F 3 "" H 3200 4900 60  0000 C CNN
F 4 "-" H 3200 4960 45  0001 C CNN "Part"
F 5 "Passive" H 3430 5090 50  0001 C CNN "Family"
	1    3200 4800
	1    0    0    -1  
$EndComp
$Comp
L passive:RESISTOR R4
U 1 1 578CBBCB
P 3200 4900
F 0 "R4" H 3050 4850 60  0000 C CNB
F 1 "22" H 3350 4850 40  0000 C CNN
F 2 "smt:R-0603" H 3200 4760 40  0001 C CNN
F 3 "" H 3200 5000 60  0000 C CNN
F 4 "-" H 3200 5060 45  0001 C CNN "Part"
F 5 "Passive" H 3430 5190 50  0001 C CNN "Family"
	1    3200 4900
	1    0    0    -1  
$EndComp
$Comp
L passive:CAP C1
U 1 1 5797C4BC
P 4700 3900
F 0 "C1" H 4730 3990 60  0000 L CNB
F 1 "20pF" H 4730 3810 45  0000 L CNN
F 2 "smt:C-0603" H 4735 3745 40  0001 L CNN
F 3 "" H 4700 4000 60  0001 C CNN
F 4 "-" H 4740 4065 45  0001 L CNN "Part"
F 5 "Passive" H 4930 4190 50  0001 C CNN "Family"
	1    4700 3900
	0    -1   -1   0   
$EndComp
$Comp
L passive:CAP C2
U 1 1 5797C663
P 4700 4300
F 0 "C2" H 4730 4390 60  0000 L CNB
F 1 "20pF" H 4730 4210 45  0000 L CNN
F 2 "smt:C-0603" H 4735 4145 40  0001 L CNN
F 3 "" H 4700 4400 60  0001 C CNN
F 4 "-" H 4740 4465 45  0001 L CNN "Part"
F 5 "Passive" H 4930 4590 50  0001 C CNN "Family"
	1    4700 4300
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR024
U 1 1 5797C71A
P 4450 4350
F 0 "#PWR024" H 4450 4400 30  0001 C CNN
F 1 "GND" H 4450 4225 30  0001 C CNN
F 2 "" H 4450 4350 60  0000 C CNN
F 3 "" H 4450 4350 60  0000 C CNN
	1    4450 4350
	1    0    0    -1  
$EndComp
$Comp
L passive:RESISTOR R5
U 1 1 5797D0D6
P 8800 2200
F 0 "R5" H 8800 2285 60  0000 C CNB
F 1 "10k" H 8800 2125 40  0000 C CNN
F 2 "smt:R-0603" H 8800 2060 40  0001 C CNN
F 3 "" H 8800 2300 60  0000 C CNN
F 4 "-" H 8800 2360 45  0001 C CNN "Part"
F 5 "Passive" H 9030 2490 50  0001 C CNN "Family"
	1    8800 2200
	0    -1   -1   0   
$EndComp
$Comp
L passive:CAP C4
U 1 1 5797D212
P 4050 5350
F 0 "C4" H 4080 5440 60  0000 L CNB
F 1 "1uF" H 4080 5260 45  0000 L CNN
F 2 "smt:C-0805" H 4085 5195 40  0001 L CNN
F 3 "" H 4050 5450 60  0001 C CNN
F 4 "-" H 4090 5515 45  0001 L CNN "Part"
F 5 "Passive" H 4280 5640 50  0001 C CNN "Family"
	1    4050 5350
	1    0    0    -1  
$EndComp
Text Label 5000 3600 0    50   ~ 0
RESET-USB
$Comp
L power:GND #PWR027
U 1 1 5797FF55
P 9850 3350
F 0 "#PWR027" H 9850 3400 30  0001 C CNN
F 1 "GND" H 9850 3225 30  0001 C CNN
F 2 "" H 9850 3350 60  0000 C CNN
F 3 "" H 9850 3350 60  0000 C CNN
	1    9850 3350
	1    0    0    -1  
$EndComp
Text Label 8200 2550 0    50   ~ 0
RESET-USB
Text Label 8850 2950 2    50   ~ 0
MISO-USB
Text Label 8850 3050 2    50   ~ 0
SCK-USB
Text Label 9750 3050 0    50   ~ 0
MOSI-USB
$Comp
L passive:CAP C5
U 1 1 57981E3C
P 4450 5350
F 0 "C5" H 4480 5440 60  0000 L CNB
F 1 "100nF" H 4480 5260 45  0000 L CNN
F 2 "smt:C-0603" H 4485 5195 40  0001 L CNN
F 3 "" H 4450 5450 60  0001 C CNN
F 4 "-" H 4490 5515 45  0001 L CNN "Part"
F 5 "Passive" H 4680 5640 50  0001 C CNN "Family"
	1    4450 5350
	1    0    0    -1  
$EndComp
Text Label 5400 5000 2    50   ~ 0
UGND
Text Label 5400 4800 2    50   ~ 0
D-
Text Label 5400 4900 2    50   ~ 0
D+
Text Label 1450 4800 0    50   ~ 0
D_IN-
Text Label 1450 4900 0    50   ~ 0
D_IN+
Wire Wire Line
	4800 3400 5400 3400
Wire Wire Line
	1400 4800 2950 4800
Wire Wire Line
	4800 3900 5100 3900
Wire Wire Line
	5100 3900 5100 4000
Wire Wire Line
	5100 4200 5100 4300
Connection ~ 5100 3900
Wire Wire Line
	4600 4300 4450 4300
Wire Wire Line
	4450 3900 4450 4300
Wire Wire Line
	4600 3900 4450 3900
Connection ~ 4450 4300
Wire Wire Line
	8800 1900 8800 2000
Wire Wire Line
	7700 3600 8050 3600
Wire Wire Line
	8050 3600 8050 2950
Wire Wire Line
	8050 2950 9300 2950
Wire Wire Line
	9300 3050 8150 3050
Wire Wire Line
	8150 3050 8150 3800
Wire Wire Line
	8150 3800 7700 3800
Wire Wire Line
	7700 3700 10200 3700
Wire Wire Line
	10200 3700 10200 3050
Wire Wire Line
	10200 3050 9700 3050
Wire Wire Line
	9700 3150 9850 3150
Wire Wire Line
	9850 3150 9850 3350
Wire Wire Line
	8200 2550 8800 2550
Wire Wire Line
	9850 2750 9850 2950
Wire Wire Line
	9850 2950 9700 2950
Wire Wire Line
	8800 2300 8800 2550
Wire Wire Line
	1400 5200 1900 5200
Connection ~ 4450 4700
Wire Wire Line
	4450 4700 4450 5250
Wire Wire Line
	5500 4600 4050 4600
Wire Wire Line
	4050 4600 4050 5250
Wire Wire Line
	4050 5900 4050 5450
$Comp
L passive:RESISTOR R7
U 1 1 57988047
P 5450 4100
F 0 "R7" H 5450 4185 60  0000 C CNB
F 1 "1M" H 5450 4025 40  0000 C CNN
F 2 "smt:R-0603" H 5450 3960 40  0001 C CNN
F 3 "" H 5450 4200 60  0000 C CNN
F 4 "-" H 5450 4260 45  0001 C CNN "Part"
F 5 "Passive" H 5680 4390 50  0001 C CNN "Family"
	1    5450 4100
	0    -1   1    0   
$EndComp
Wire Wire Line
	5450 4000 5450 3900
Connection ~ 5450 3900
Wire Wire Line
	5450 4300 5450 4200
$Comp
L passive:CAP C11
U 1 1 5B7819D2
P 4700 3400
F 0 "C11" H 4730 3490 60  0000 L CNB
F 1 "100nF" H 4730 3310 45  0000 L CNN
F 2 "smt:C-0603" H 4735 3245 40  0001 L CNN
F 3 "" H 4700 3500 60  0001 C CNN
F 4 "-" H 4740 3565 45  0001 L CNN "Part"
F 5 "Passive" H 4930 3690 50  0001 C CNN "Family"
	1    4700 3400
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR030
U 1 1 5B7819D8
P 4500 3400
F 0 "#PWR030" H 4500 3450 30  0001 C CNN
F 1 "GND" H 4500 3275 30  0001 C CNN
F 2 "" H 4500 3400 60  0000 C CNN
F 3 "" H 4500 3400 60  0000 C CNN
	1    4500 3400
	1    0    0    -1  
$EndComp
Wire Wire Line
	4600 3400 4500 3400
Wire Wire Line
	7700 5000 8300 5000
Wire Wire Line
	4800 4300 5100 4300
Connection ~ 5100 4300
Connection ~ 5450 4300
Wire Wire Line
	5100 3900 5450 3900
Wire Wire Line
	4450 4300 4450 4350
Wire Wire Line
	4450 4700 5500 4700
Wire Wire Line
	5450 3900 5500 3900
Wire Wire Line
	5100 4300 5450 4300
Wire Wire Line
	5450 4300 5500 4300
NoConn ~ 7700 3200
NoConn ~ 7700 3300
NoConn ~ 7700 3400
NoConn ~ 7700 3900
NoConn ~ 7700 3500
NoConn ~ 7700 4200
NoConn ~ 7700 4300
NoConn ~ 7700 4400
NoConn ~ 7700 4500
NoConn ~ 7700 4800
NoConn ~ 7700 5300
NoConn ~ 7700 5400
NoConn ~ 1400 5000
Text Label 7850 4900 0    60   ~ 0
LED_TX
Text Label 7850 5000 0    60   ~ 0
LED_RX
Wire Wire Line
	4450 5900 4050 5900
Wire Wire Line
	4450 5450 4450 5900
$Comp
L passive:DIODE D2
U 1 1 5E643712
P 9200 2200
F 0 "D2" H 9200 2310 60  0000 C CNB
F 1 "CD1206-S01575" H 9200 2100 40  0000 C CNN
F 2 "smt:D-1206" H 9200 2040 40  0001 C CNN
F 3 "" H 9200 2320 60  0000 C CNN
F 4 "CD1206-S01575" H 9205 2375 45  0001 C CNN "Part"
F 5 "Passive" H 9430 2490 50  0001 C CNN "Family"
	1    9200 2200
	0    -1   -1   0   
$EndComp
Wire Wire Line
	9200 2550 9200 3150
Wire Wire Line
	8800 2550 9200 2550
Connection ~ 8800 2550
Wire Wire Line
	9200 3150 9300 3150
Wire Wire Line
	9200 2300 9200 2550
Connection ~ 9200 2550
Wire Wire Line
	9200 2100 9200 2000
Wire Wire Line
	9200 2000 8800 2000
Connection ~ 8800 2000
Wire Wire Line
	8800 2000 8800 2100
Wire Wire Line
	5400 5400 5400 5550
$Comp
L power:GND #PWR022
U 1 1 578C70F4
P 5400 5550
F 0 "#PWR022" H 5400 5600 30  0001 C CNN
F 1 "GND" H 5400 5425 30  0001 C CNN
F 2 "" H 5400 5550 60  0000 C CNN
F 3 "" H 5400 5550 60  0000 C CNN
	1    5400 5550
	1    0    0    -1  
$EndComp
Wire Wire Line
	8300 5000 8300 6300
Wire Wire Line
	8300 6300 8900 6300
Wire Wire Line
	8400 4900 8400 6200
Wire Wire Line
	8400 6200 8900 6200
Wire Wire Line
	7700 4900 8400 4900
Wire Wire Line
	8800 6000 8800 6100
Wire Wire Line
	8800 6100 8900 6100
Wire Wire Line
	1400 4700 1900 4700
Text HLabel 9550 4700 2    60   Output ~ 0
RESET
$Comp
L passive:RESISTOR R24
U 1 1 5E8D21DA
P 9200 5300
F 0 "R24" H 9200 5385 60  0000 C CNB
F 1 "1k" H 9200 5225 40  0000 C CNN
F 2 "smt:R-0603" H 9200 5160 40  0001 C CNN
F 3 "" H 9200 5400 60  0000 C CNN
F 4 "-" H 9200 5460 45  0001 C CNN "Part"
F 5 "Passive" H 9430 5590 50  0001 C CNN "Family"
	1    9200 5300
	1    0    0    -1  
$EndComp
$Comp
L passive:RESISTOR R19
U 1 1 5BAF646D
P 9200 5000
F 0 "R19" H 9200 5085 60  0000 C CNB
F 1 "1k" H 9200 4925 40  0000 C CNN
F 2 "smt:R-0603" H 9200 4860 40  0001 C CNN
F 3 "" H 9200 5100 60  0000 C CNN
F 4 "-" H 9200 5160 45  0001 C CNN "Part"
F 5 "Passive" H 9430 5290 50  0001 C CNN "Family"
	1    9200 5000
	1    0    0    -1  
$EndComp
Wire Wire Line
	5400 2950 5400 3200
$Comp
L power:+5V #PWR0139
U 1 1 5BB96942
P 5400 2950
F 0 "#PWR0139" H 5400 3175 30  0001 C CNN
F 1 "+5V" V 5325 2950 30  0001 C CNN
F 2 "" H 5400 2950 60  0000 C CNN
F 3 "" H 5400 2950 60  0000 C CNN
	1    5400 2950
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0140
U 1 1 5BB96D7E
P 8800 1900
F 0 "#PWR0140" H 8800 2125 30  0001 C CNN
F 1 "+5V" V 8725 1900 30  0001 C CNN
F 2 "" H 8800 1900 60  0000 C CNN
F 3 "" H 8800 1900 60  0000 C CNN
	1    8800 1900
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0141
U 1 1 5BB971BA
P 9850 2750
F 0 "#PWR0141" H 9850 2975 30  0001 C CNN
F 1 "+5V" V 9775 2750 30  0001 C CNN
F 2 "" H 9850 2750 60  0000 C CNN
F 3 "" H 9850 2750 60  0000 C CNN
	1    9850 2750
	1    0    0    -1  
$EndComp
Wire Wire Line
	5500 5400 5400 5400
Connection ~ 5400 3200
Connection ~ 5400 3400
Wire Wire Line
	5400 3200 5500 3200
Wire Wire Line
	5400 3400 5500 3400
Wire Wire Line
	5400 3200 5400 3400
Wire Wire Line
	4900 5000 4900 5900
Wire Wire Line
	4900 5900 4450 5900
Connection ~ 4450 5900
Wire Wire Line
	4900 5000 5500 5000
$Comp
L power:GND #PWR0142
U 1 1 5BCBFEBA
P 1900 6050
F 0 "#PWR0142" H 1900 6100 30  0001 C CNN
F 1 "GND" H 1900 5925 30  0001 C CNN
F 2 "" H 1900 6050 60  0000 C CNN
F 3 "" H 1900 6050 60  0000 C CNN
	1    1900 6050
	1    0    0    -1  
$EndComp
Wire Wire Line
	1900 5900 1900 6050
Wire Wire Line
	1400 5100 1900 5100
Wire Wire Line
	1900 5200 1900 5100
$Comp
L passive:SR05 DA1
U 1 1 5BDE7513
P 2500 5450
F 0 "DA1" H 2250 5750 60  0000 L BNB
F 1 "SR05" H 2250 5150 40  0000 L TNN
F 2 "smt-sot:SOT-143" H 2250 5075 40  0001 L TNN
F 3 "http://www.semtech.com/images/datasheet/sr05.pdf" H 2350 5570 60  0001 C CNN
F 4 "Passive" H 2730 5740 50  0001 C CNN "Family"
	1    2500 5450
	1    0    0    -1  
$EndComp
Wire Wire Line
	1400 4900 2100 4900
Wire Wire Line
	2500 5800 2500 5900
Wire Wire Line
	2150 5450 2100 5450
Wire Wire Line
	2100 5450 2100 4900
Wire Wire Line
	2850 5450 2950 5450
Wire Wire Line
	2950 5450 2950 4800
Connection ~ 2950 4800
Connection ~ 2100 4900
Wire Wire Line
	3300 4900 5500 4900
Wire Wire Line
	3300 4800 5500 4800
Wire Wire Line
	2100 4900 3100 4900
Wire Wire Line
	2950 4800 3100 4800
Connection ~ 1900 5200
Connection ~ 1900 5900
Connection ~ 2500 5900
Connection ~ 4050 5900
Wire Wire Line
	2500 5900 4050 5900
Wire Wire Line
	1900 5900 2500 5900
Connection ~ 2500 4400
Wire Wire Line
	2500 4400 2500 5100
Wire Wire Line
	1900 4700 1900 4400
Wire Wire Line
	2500 4400 3550 4400
Wire Wire Line
	3550 4700 4450 4700
Wire Wire Line
	3550 4400 3550 4700
Wire Wire Line
	1900 5200 1900 5900
Wire Wire Line
	1900 4400 2100 4400
$Comp
L passive:PTC F1
U 1 1 5BFD4F57
P 2200 4400
F 0 "F1" H 2200 4535 60  0000 C CNB
F 1 "500mA" H 2200 4275 40  0000 C CNN
F 2 "smt:R-1206" H 2200 4210 40  0001 C CNN
F 3 "" H 2400 4500 60  0001 C CNN
F 4 "-" H 2200 4610 45  0001 C CNN "Part"
F 5 "Passive" H 2200 4675 50  0001 C CNN "Family"
	1    2200 4400
	1    0    0    -1  
$EndComp
Wire Wire Line
	2300 4400 2500 4400
Wire Wire Line
	9100 5000 8950 5000
Wire Wire Line
	8950 5000 8950 5100
Wire Wire Line
	7700 5100 8950 5100
Wire Wire Line
	8950 5200 8950 5300
Wire Wire Line
	8950 5300 9100 5300
Wire Wire Line
	7700 5200 8950 5200
Wire Wire Line
	9300 5000 9550 5000
Wire Wire Line
	9300 5300 9550 5300
Text HLabel 9550 5000 2    60   Output ~ 0
TXD
Text HLabel 9550 5300 2    60   Input ~ 0
RXD
$Comp
L power:+5V #PWR0145
U 1 1 5C1023C7
P 8800 6000
F 0 "#PWR0145" H 8800 6225 30  0001 C CNN
F 1 "+5V" V 8725 6000 30  0001 C CNN
F 2 "" H 8800 6000 60  0000 C CNN
F 3 "" H 8800 6000 60  0000 C CNN
	1    8800 6000
	1    0    0    -1  
$EndComp
Wire Wire Line
	7700 4700 9550 4700
Text Label 1450 4700 0    50   ~ 0
UVCC_IN
Wire Wire Line
	5500 3600 5000 3600
Text Label 5400 4700 2    50   ~ 0
UVCC
$Comp
L transistor:MMST3904 Q1
U 1 1 5C29FCBC
P 3200 2800
F 0 "Q1" H 3250 2925 60  0000 L CNB
F 1 "MMST3904" H 3250 2825 45  0000 L CNN
F 2 "smt-sot:SOT23" H 3350 2600 45  0001 L CNN
F 3 "" H 3350 2525 45  0001 L CNN
F 4 "-" H 3250 3000 45  0001 L CNN "Part"
F 5 "IC" H 3250 3075 50  0001 L CNN "Family"
	1    3200 2800
	1    0    0    -1  
$EndComp
$Comp
L passive:RESISTOR R1
U 1 1 5C29FDF8
P 2750 2800
F 0 "R1" H 2750 2885 60  0000 C CNB
F 1 "10k" H 2750 2725 40  0000 C CNN
F 2 "smt:R-0603" H 2750 2660 40  0001 C CNN
F 3 "" H 2750 2900 60  0000 C CNN
F 4 "-" H 2750 2960 45  0001 C CNN "Part"
F 5 "Passive" H 2980 3090 50  0001 C CNN "Family"
	1    2750 2800
	1    0    0    -1  
$EndComp
Wire Wire Line
	2650 2800 2500 2800
Wire Wire Line
	2500 2800 2500 4400
Wire Wire Line
	2850 2800 3050 2800
$Comp
L power:GND #PWR0143
U 1 1 5C2A6100
P 3200 3050
F 0 "#PWR0143" H 3200 3100 30  0001 C CNN
F 1 "GND" H 3200 2925 30  0001 C CNN
F 2 "" H 3200 3050 60  0000 C CNN
F 3 "" H 3200 3050 60  0000 C CNN
	1    3200 3050
	1    0    0    -1  
$EndComp
Wire Wire Line
	3200 2950 3200 3050
$Comp
L passive:RESISTOR R2
U 1 1 5C2A94A9
P 3200 2350
F 0 "R2" H 3200 2435 60  0000 C CNB
F 1 "10k" H 3200 2275 40  0000 C CNN
F 2 "smt:R-0603" H 3200 2210 40  0001 C CNN
F 3 "" H 3200 2450 60  0000 C CNN
F 4 "-" H 3200 2510 45  0001 C CNN "Part"
F 5 "Passive" H 3430 2640 50  0001 C CNN "Family"
	1    3200 2350
	0    -1   1    0   
$EndComp
Wire Wire Line
	3200 2450 3200 2550
$Comp
L power:+5V #PWR0144
U 1 1 5C2AC9DA
P 3200 2050
F 0 "#PWR0144" H 3200 2275 30  0001 C CNN
F 1 "+5V" V 3125 2050 30  0001 C CNN
F 2 "" H 3200 2050 60  0000 C CNN
F 3 "" H 3200 2050 60  0000 C CNN
	1    3200 2050
	1    0    0    -1  
$EndComp
Wire Wire Line
	3200 2050 3200 2250
Connection ~ 3200 2550
Wire Wire Line
	3200 2550 3200 2650
Text Label 3900 2550 2    60   ~ 0
VUSB-DET
Wire Wire Line
	3200 2550 7950 2550
Text Label 7850 5100 0    60   ~ 0
UTXD
Text Label 7850 5200 0    60   ~ 0
URXD
$Comp
L connectors:HEADER-1x04 J13
U 1 1 5BE73E06
P 9050 6250
F 0 "J13" H 9000 6500 60  0000 L BNB
F 1 "USB-LED" V 9200 6250 40  0000 C CNN
F 2 "conn-wire-pads:WP-50mil-1x04" H 9050 6250 60  0001 C CNN
F 3 "" H 9050 6250 60  0001 C CNN
F 4 "-" H 9000 6600 40  0001 L BNN "Part"
F 5 "Virtual" H 9000 6700 40  0001 L BNN "Family"
	1    9050 6250
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0148
U 1 1 5BE73F09
P 8800 6550
F 0 "#PWR0148" H 8800 6600 30  0001 C CNN
F 1 "GND" H 8800 6425 30  0001 C CNN
F 2 "" H 8800 6550 60  0000 C CNN
F 3 "" H 8800 6550 60  0000 C CNN
	1    8800 6550
	1    0    0    -1  
$EndComp
Wire Wire Line
	8900 6400 8800 6400
Wire Wire Line
	8800 6400 8800 6550
Wire Wire Line
	7950 4100 7700 4100
Wire Wire Line
	7950 2550 7950 4100
$EndSCHEMATC
