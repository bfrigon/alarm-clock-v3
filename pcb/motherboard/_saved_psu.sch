EESchema Schematic File Version 4
LIBS:motherboard-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 4 4
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L connectors:BARREL_JACK J?
U 1 1 5C12DD41
P 2900 3000
AR Path="/5C12DD41" Ref="J?"  Part="1" 
AR Path="/5B9219B1/5C12DD41" Ref="J?"  Part="1" 
AR Path="/5C119673/5C12DD41" Ref="J?"  Part="1" 
F 0 "J?" H 2800 3200 60  0000 L BNB
F 1 "-" H 2800 2850 40  0001 L TNN
F 2 "conn-power:CUI-P1J-021-SMT" H 2950 3000 60  0001 C CNN
F 3 "" H 2950 3000 60  0001 C CNN
F 4 "PJ1-021-SMT-TR" H 2700 2800 40  0000 L BNN "Part"
F 5 "Connector" H 2800 3400 40  0001 L BNN "Family"
	1    2900 3000
	-1   0    0    -1  
$EndComp
Wire Wire Line
	3100 3100 3250 3100
Wire Wire Line
	3250 3000 3250 3100
Wire Wire Line
	3100 3000 3250 3000
Connection ~ 3250 3100
$Comp
L power:GND #PWR?
U 1 1 5C12DD4C
P 5150 3650
AR Path="/5C12DD4C" Ref="#PWR?"  Part="1" 
AR Path="/5B9219B1/5C12DD4C" Ref="#PWR?"  Part="1" 
AR Path="/5C119673/5C12DD4C" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 5150 3700 30  0001 C CNN
F 1 "GND" H 5150 3525 30  0001 C CNN
F 2 "" H 5150 3650 60  0000 C CNN
F 3 "" H 5150 3650 60  0000 C CNN
	1    5150 3650
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5C12DD52
P 4450 3650
AR Path="/5C12DD52" Ref="#PWR?"  Part="1" 
AR Path="/5B9219B1/5C12DD52" Ref="#PWR?"  Part="1" 
AR Path="/5C119673/5C12DD52" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 4450 3700 30  0001 C CNN
F 1 "GND" H 4450 3525 30  0001 C CNN
F 2 "" H 4450 3650 60  0000 C CNN
F 3 "" H 4450 3650 60  0000 C CNN
	1    4450 3650
	1    0    0    -1  
$EndComp
Text Label 4250 2900 0    50   ~ 0
VIN
Wire Wire Line
	4600 3500 4450 3500
Wire Wire Line
	4450 3500 4450 3650
Wire Wire Line
	5000 3500 5150 3500
Wire Wire Line
	5150 3500 5150 3650
$Comp
L connectors:HEADER-2x03 J?
U 1 1 5C12DD60
P 4800 3400
AR Path="/5C12DD60" Ref="J?"  Part="1" 
AR Path="/5B9219B1/5C12DD60" Ref="J?"  Part="1" 
AR Path="/5C119673/5C12DD60" Ref="J?"  Part="1" 
F 0 "J?" H 4700 3600 60  0000 L BNB
F 1 "PSU" H 4700 3200 40  0000 L TNN
F 2 "conn-header:HDR-M-2x03" H 4800 2200 60  0001 C CNN
F 3 "" H 4800 2200 60  0001 C CNN
F 4 "BCS-103-L-D-TE" H 4700 3700 40  0001 L BNN "Part"
F 5 "Connector" H 4700 3800 40  0001 L BNN "Family"
	1    4800 3400
	1    0    0    -1  
$EndComp
Wire Wire Line
	4450 2900 4450 3300
Wire Wire Line
	5150 3400 5000 3400
Wire Wire Line
	3250 3100 3250 3200
Wire Wire Line
	3500 5400 4050 5400
Wire Wire Line
	3500 5500 4050 5500
Text Label 4050 5400 2    60   ~ 0
LOW_BATT
Text Label 4050 5500 2    60   ~ 0
ON_BATT
$Comp
L power:GND #PWR?
U 1 1 5C12DD79
P 3250 3200
AR Path="/5C12DD79" Ref="#PWR?"  Part="1" 
AR Path="/5B9219B1/5C12DD79" Ref="#PWR?"  Part="1" 
AR Path="/5C119673/5C12DD79" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 3250 3250 30  0001 C CNN
F 1 "GND" H 3250 3075 30  0001 C CNN
F 2 "" H 3250 3200 60  0000 C CNN
F 3 "" H 3250 3200 60  0000 C CNN
	1    3250 3200
	1    0    0    -1  
$EndComp
$Comp
L power:SUP_+5V #PWR?
U 1 1 5C12DD7F
P 5150 3150
AR Path="/5C12DD7F" Ref="#PWR?"  Part="1" 
AR Path="/5C119673/5C12DD7F" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 5150 3375 30  0001 C CNN
F 1 "SUP_+5V" V 5075 3150 30  0001 C CNN
F 2 "" H 5050 3275 60  0001 C CNN
F 3 "" H 5150 3375 60  0001 C CNN
	1    5150 3150
	1    0    0    -1  
$EndComp
Wire Wire Line
	3500 5600 4050 5600
Wire Wire Line
	3500 5700 4050 5700
Text Label 4050 5600 2    60   ~ 0
SDA
Text Label 4050 5700 2    60   ~ 0
SCL
$Comp
L connectors:HEADER-1x04 J?
U 1 1 5C12DD90
P 3350 5550
AR Path="/5C12DD90" Ref="J?"  Part="1" 
AR Path="/5C119673/5C12DD90" Ref="J?"  Part="1" 
F 0 "J?" H 3300 5800 60  0000 L BNB
F 1 "PSU-SIG" V 3500 5550 40  0000 C CNN
F 2 "conn-header:HDR-M-1x04" H 3350 5550 60  0001 C CNN
F 3 "" H 3350 5550 60  0001 C CNN
F 4 "BCS-104-L-S-TE" H 3300 5900 40  0001 L BNN "Part"
F 5 "Connector" H 3300 6000 40  0001 L BNN "Family"
	1    3350 5550
	-1   0    0    -1  
$EndComp
$Comp
L passive:PTC R?
U 1 1 5C12DD99
P 3800 2900
AR Path="/5C12DD99" Ref="R?"  Part="1" 
AR Path="/5C119673/5C12DD99" Ref="R?"  Part="1" 
F 0 "R?" H 3800 3035 60  0000 C CNB
F 1 "PTC" H 3800 2775 40  0000 C CNN
F 2 "" H 3800 2710 40  0001 C CNN
F 3 "" H 4000 3000 60  0001 C CNN
F 4 "-" H 3800 3110 45  0001 C CNN "Part"
F 5 "Passive" H 3800 3175 50  0001 C CNN "Family"
	1    3800 2900
	1    0    0    -1  
$EndComp
Wire Wire Line
	3100 2900 3700 2900
Wire Wire Line
	3900 2900 4450 2900
Wire Wire Line
	4450 3300 4450 3400
Connection ~ 4450 3300
Wire Wire Line
	4450 3400 4600 3400
Wire Wire Line
	4450 3300 4600 3300
Wire Wire Line
	5150 3150 5150 3300
Wire Wire Line
	5000 3300 5150 3300
Connection ~ 5150 3300
Wire Wire Line
	5150 3300 5150 3400
$EndSCHEMATC
