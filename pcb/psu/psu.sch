EESchema Schematic File Version 4
LIBS:connectors
LIBS:ic-cpu
LIBS:ic-power
LIBS:ic-misc
LIBS:ic-io
LIBS:passive
LIBS:power
LIBS:switches
LIBS:opto
LIBS:psu-cache
EELAYER 26 0
EELAYER END
$Descr USLetter 11000 8500
encoding utf-8
Sheet 1 1
Title "Power supply"
Date ""
Rev "A"
Comp ""
Comment1 "CLKV3-PC-07"
Comment2 "CLKV3-SCH-07"
Comment3 "Alarm Clock (V3)"
Comment4 ""
$EndDescr
$Comp
L connectors:HEADER-2x03 J1
U 1 1 5CA32E71
P 2700 3800
F 0 "J1" H 2600 4000 60  0000 L BNB
F 1 "VIN" H 2650 3600 40  0000 L TNN
F 2 "conn-header:HDR-M-2x03" H 2700 2600 60  0001 C CNN
F 3 "" H 2700 2600 60  0001 C CNN
F 4 "TSW-103-07-F-D" H 2600 4100 40  0001 L BNN "Part"
F 5 "Virtual" H 2600 4200 40  0001 L BNN "Family"
	1    2700 3800
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0102
U 1 1 5CA3375C
P 3000 4050
F 0 "#PWR0102" H 3000 4100 30  0001 C CNN
F 1 "GND" H 3000 3925 30  0001 C CNN
F 2 "" H 3000 4050 60  0000 C CNN
F 3 "" H 3000 4050 60  0000 C CNN
	1    3000 4050
	1    0    0    -1  
$EndComp
Wire Wire Line
	2900 3900 3000 3900
Wire Wire Line
	3000 3900 3000 4050
$Comp
L power:GND #PWR0103
U 1 1 5CA3378E
P 2400 4050
F 0 "#PWR0103" H 2400 4100 30  0001 C CNN
F 1 "GND" H 2400 3925 30  0001 C CNN
F 2 "" H 2400 4050 60  0000 C CNN
F 3 "" H 2400 4050 60  0000 C CNN
	1    2400 4050
	1    0    0    -1  
$EndComp
Wire Wire Line
	2500 3900 2400 3900
Wire Wire Line
	2400 3900 2400 4050
$Comp
L power:+5V #PWR0104
U 1 1 5CA3381B
P 3250 3550
F 0 "#PWR0104" H 3250 3775 30  0001 C CNN
F 1 "+5V" V 3175 3550 30  0001 C CNN
F 2 "" H 3250 3550 60  0000 C CNN
F 3 "" H 3250 3550 60  0000 C CNN
	1    3250 3550
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0105
U 1 1 5CA33835
P 2150 3550
F 0 "#PWR0105" H 2150 3775 30  0001 C CNN
F 1 "+5V" V 2075 3550 30  0001 C CNN
F 2 "" H 2150 3550 60  0000 C CNN
F 3 "" H 2150 3550 60  0000 C CNN
	1    2150 3550
	1    0    0    -1  
$EndComp
Wire Wire Line
	2500 3800 2150 3800
Wire Wire Line
	2150 3800 2150 3550
Wire Wire Line
	2900 3800 3250 3800
Wire Wire Line
	3250 3800 3250 3550
$Comp
L connectors:HEADER-1x04 J2
U 1 1 5BA2EBB6
P 3400 5100
F 0 "J2" H 3350 5350 60  0000 L BNB
F 1 "HEADER-1x04" V 3550 5100 40  0000 C CNN
F 2 "conn-header:HDR-M-1x04" H 3400 5100 60  0001 C CNN
F 3 "" H 3400 5100 60  0001 C CNN
F 4 "TSW-104-07-F-S" H 3350 5450 40  0001 L BNN "Part"
F 5 "Connector" H 3350 5550 40  0001 L BNN "Family"
	1    3400 5100
	-1   0    0    -1  
$EndComp
$Comp
L connectors:HEADER-1x01 J3
U 1 1 5BA3D5A7
P 1650 6600
F 0 "J3" H 1600 6700 60  0000 L BNB
F 1 "HEADER-1x01" V 1800 6600 40  0000 C CNN
F 2 "conn-wire-pads:Hole-Screw-#4" H 1500 6575 60  0001 C CNN
F 3 "" H 1600 6675 60  0001 C CNN
F 4 "-" H 1600 6800 40  0001 L BNN "Part"
F 5 "Connector" H 1600 6900 40  0001 L BNN "Family"
	1    1650 6600
	1    0    0    -1  
$EndComp
$EndSCHEMATC
