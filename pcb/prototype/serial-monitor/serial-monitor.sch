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
LIBS:serial-monitor-cache
EELAYER 26 0
EELAYER END
$Descr USLetter 11000 8500
encoding utf-8
Sheet 1 1
Title "Title"
Date ""
Rev "A"
Comp ""
Comment1 "??-PC-00"
Comment2 "??-SCH-00"
Comment3 "Project Title"
Comment4 ""
$EndDescr
$Comp
L opto:LED D2
U 1 1 5C66BCE1
P 6450 3400
F 0 "D2" H 6450 3510 60  0000 C CNB
F 1 "LED" H 6450 3300 40  0000 C CNN
F 2 "smt:LED-0805" H 6450 3240 40  0001 C CNN
F 3 "" H 6450 3520 60  0000 C CNN
F 4 "-" H 6455 3575 45  0001 C CNN "Part"
F 5 "Passive" H 6450 3700 50  0001 C CNN "Family"
	1    6450 3400
	0    -1   1    0   
$EndComp
$Comp
L opto:LED D1
U 1 1 5C66BD61
P 4700 3400
F 0 "D1" H 4700 3510 60  0000 C CNB
F 1 "LED" H 4700 3300 40  0000 C CNN
F 2 "smt:LED-0805" H 4700 3240 40  0001 C CNN
F 3 "" H 4700 3520 60  0000 C CNN
F 4 "-" H 4705 3575 45  0001 C CNN "Part"
F 5 "Passive" H 4700 3700 50  0001 C CNN "Family"
	1    4700 3400
	0    -1   1    0   
$EndComp
$Comp
L passive:RESISTOR R2
U 1 1 5C66BE4E
P 6450 3800
F 0 "R2" H 6450 3885 60  0000 C CNB
F 1 "680" H 6450 3725 40  0000 C CNN
F 2 "smt:R-0603" H 6450 3660 40  0001 C CNN
F 3 "" H 6450 3900 60  0000 C CNN
F 4 "-" H 6450 3960 45  0001 C CNN "Part"
F 5 "Passive" H 6680 4090 50  0001 C CNN "Family"
	1    6450 3800
	0    -1   1    0   
$EndComp
$Comp
L passive:RESISTOR R1
U 1 1 5C66BE86
P 4700 3800
F 0 "R1" H 4700 3885 60  0000 C CNB
F 1 "680" H 4700 3725 40  0000 C CNN
F 2 "smt:R-0603" H 4700 3660 40  0001 C CNN
F 3 "" H 4700 3900 60  0000 C CNN
F 4 "-" H 4700 3960 45  0001 C CNN "Part"
F 5 "Passive" H 4930 4090 50  0001 C CNN "Family"
	1    4700 3800
	0    -1   1    0   
$EndComp
Wire Wire Line
	4700 3500 4700 3700
Wire Wire Line
	6450 3500 6450 3700
$Comp
L connectors:HEADER-2x03 J1
U 1 1 5C66C204
P 5450 4150
F 0 "J1" H 5350 4350 60  0000 L BNB
F 1 "ICSP" H 5350 3950 40  0000 L TNN
F 2 "conn-header:HDR-M-2x03" H 5450 2950 60  0001 C CNN
F 3 "" H 5450 2950 60  0001 C CNN
F 4 "-" H 5350 4450 40  0001 L BNN "Part"
F 5 "Connector" H 5350 4550 40  0001 L BNN "Family"
	1    5450 4150
	1    0    0    -1  
$EndComp
$Comp
L power:SUP_+5V #PWR0101
U 1 1 5C66C3A1
P 5900 3900
F 0 "#PWR0101" H 5900 4125 30  0001 C CNN
F 1 "SUP_+5V" V 5825 3900 30  0001 C CNN
F 2 "" H 5800 4025 60  0001 C CNN
F 3 "" H 5900 4125 60  0001 C CNN
	1    5900 3900
	1    0    0    -1  
$EndComp
$Comp
L power:SUP_GND #PWR0102
U 1 1 5C66C44F
P 5900 4450
F 0 "#PWR0102" H 5900 4500 30  0001 C CNN
F 1 "SUP_GND" H 5900 4325 30  0001 C CNN
F 2 "" H 5900 4450 60  0000 C CNN
F 3 "" H 5900 4450 60  0000 C CNN
	1    5900 4450
	1    0    0    -1  
$EndComp
Wire Wire Line
	5650 4250 5900 4250
Wire Wire Line
	5900 4250 5900 4450
Wire Wire Line
	5650 4050 5900 4050
Wire Wire Line
	5900 4050 5900 3900
Wire Wire Line
	6450 3900 6450 4150
Wire Wire Line
	6450 4150 5650 4150
Wire Wire Line
	4700 3900 4700 4050
Wire Wire Line
	4700 4050 5250 4050
$Comp
L power:+5V #PWR0103
U 1 1 5C66CA23
P 4700 3200
F 0 "#PWR0103" H 4700 3425 30  0001 C CNN
F 1 "+5V" V 4625 3200 30  0001 C CNN
F 2 "" H 4700 3200 60  0000 C CNN
F 3 "" H 4700 3200 60  0000 C CNN
	1    4700 3200
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0104
U 1 1 5C66CA3F
P 6450 3200
F 0 "#PWR0104" H 6450 3425 30  0001 C CNN
F 1 "+5V" V 6375 3200 30  0001 C CNN
F 2 "" H 6450 3200 60  0000 C CNN
F 3 "" H 6450 3200 60  0000 C CNN
	1    6450 3200
	1    0    0    -1  
$EndComp
Wire Wire Line
	6450 3200 6450 3300
Wire Wire Line
	4700 3200 4700 3300
$Comp
L switches:MOM-SPST SW1
U 1 1 5C66CF86
P 4300 4250
F 0 "SW1" H 4200 4350 60  0000 L BNB
F 1 "MOM-SPST" H 4200 4200 40  0000 L TNN
F 2 "switches:B3F-10XX" H 4200 4125 40  0001 L TNN
F 3 "" H 4225 4025 60  0001 C CNN
F 4 "-" H 4200 4450 40  0001 L BNN "Part"
F 5 "Switch" H 4200 4550 40  0001 L BNN "Family"
	1    4300 4250
	1    0    0    -1  
$EndComp
Wire Wire Line
	5250 4250 4400 4250
$Comp
L power:GND #PWR0105
U 1 1 5C66D120
P 4050 4450
F 0 "#PWR0105" H 4050 4500 30  0001 C CNN
F 1 "GND" H 4050 4325 30  0001 C CNN
F 2 "" H 4050 4450 60  0000 C CNN
F 3 "" H 4050 4450 60  0000 C CNN
	1    4050 4450
	1    0    0    -1  
$EndComp
Wire Wire Line
	4200 4250 4050 4250
Wire Wire Line
	4050 4250 4050 4450
$EndSCHEMATC
