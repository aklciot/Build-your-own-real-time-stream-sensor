<img src="https://github.com/aklciot/StreamECSensor/blob/master/InnovateAuckland_small.png" align="middle" height="75"/>

# StreamECSensor
Build your own automated water quality EC sensor



## Compiling and loading
This code can be compiled on a standard Arduino GUI.
### Libraries
The following libraries are required:
- AsyncTCP (can be installed fromzip file in library folder)
- ESPAsyncWebServer (can be installed from zip file in library folder)
- EEPROM (can be installed from zip file in library folder) (N.B. This library has been depreciated and should be replaced by Preferences. Raised as [issue #2](https://github.com/aklciot/TTGO-Gateway-repeater/issues/2))
- Adafruit MQTT
- ESP8266 and ESP32 OLED driver for SSD1306 displays
- RH_RF95.h from RadioHead

## Setup for different gateways
* The node ID, repeater ID etc needs to be set up in `param.h`
This should be configurable by the web interface over WiFi, [Issue #1](https://github.com/aklciot/TTGO-Gateway-repeater/issues/1) raised for this feature
* MQTT setting need to be done in the `param.h1` file
This should also be configurable over the web interface - see [Issue #3](https://github.com/aklciot/TTGO-Gateway-repeater/issues/3)

## Gateway Modes of operation:

The gateway sends data from the sensor to the internet.  To make this work the gateway needs to know your WiFi name and password to make a connection. The gateway uses very little data, about 50MB per month, as much as listening to a few music tracks.  It also contains no personal data, has no microphone or camera and is hack resistant. This is because the gateway has a very low attack surface, it does not listen for any internet traffic (its’ all outbound only).

