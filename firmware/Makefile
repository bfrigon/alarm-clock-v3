ARDUINO_DIR = /opt/arduino
ARDMK_DIR = $(HOME)/.arduino_mk
BOARD_TAG = mega
BOARD_SUB = atmega2560

ARDUINO_LIBS = Wire SPI EEPROM SD Adafruit_NeoPixel Sodaq_DS3231 winc1500_async Adafruit_VS1053_Library

ARDUINO_QUIET = 1

MONITOR_CMD = picocom
MONITOR_BAUDRATE = 115200





include $(ARDMK_DIR)/Arduino.mk

.PHONY : monitor upload verify kill-monitor


upload: | kill-monitor upload
verify: all

kill-monitor:
	-killall picocom
