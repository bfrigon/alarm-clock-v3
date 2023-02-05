alarm-clock (V3)
================

Project page [here][link-project]

Specs
-----

- Arduino based design using the ATMEGA2560 MCU.
- Plays MP3/WAV files from a SD card.
- WiFi connectivity (802.11 b/g/n)
- Automatic clock synchronization using NTP
- Send alarm state to Home Assistant via MQTT
- Large RGB seven segment display
- 16x2 character LCD display
- Night lamp
- Touch keypad
- 24h battery backup


---------------------------------------------------

![alt tag](/pictures/final-assembly/assembly-final-front.jpg "Front view")
![alt tag](/pictures/final-assembly/rear-view.jpg "Rear view")

Home Assistant
--------------
The clock can send the alarm switch state and next alarm timestamp to Home Assistant via MQTT. This allows to run home automations based on the alarm time. It also sends other data such as WiFi signal strength, power state, battery voltage and remaining capacity.


![alt tag](/pictures/ha-sensors.jpg "Home Assistant sensors")



[link-project]:https://www.frigon.info/posts/projects/alarm-clock-v3/
