<img src="https://github.com/aklciot/StreamECSensor/blob/master/InnovateAuckland_Medium.png" align="middle" height="75"/>

# StreamECSensor
Build your own automated water quality EC sensor



## Compiling and loading
This code can be compiled on a standard Arduino GUI.
### Libraries
The following libraries are required:
- OneWire.h (connects data for the dallas sensor)
- DallasTemperature.h (DS18B20 digital thermometer)
- Adafruit_SleepyDog.h  (Power management)
- RH_RF95.h (Hope radio LoRa driver)
- Ultrasonic.h (driver for the grove ultrasonic sensor)

### PLEASE NOTE:
This sensor is designed to work with the open source LoRa telemetry network and gateway described here:</br>
https://github.com/aklciot/TTGO-Gateway-repeater
</br>

## About this Sensor

### Who should use this guide:

This guide is for anyone interested in creating their own real-time environmental data sensing network.  It explains how to install and use the open source water quality sensor and the data network that makes your sensor data available on the internet.

Some of the concepts in the guide are quite technical, if you’re not sure or need clarification please email us at the address below.

We recommend you have access to people familiar with connecting computer peripherals and WiFi settings.

### What is real-time monitoring?
Low cost Real-time monitoring is an amazing new capability made possible by a collection of technologies commonly referred to as the internet of things (IOT). Most monitoring techniques involve a regular visit to the monitoring location where recordings are made and recorded manually. These visits can be weeks or months apart.  In real time monitoring, measurements are made automatically every few minutes and the information is immediately processed and stored on the internet. This  creates an up to date, long term and detailed record.

### Why is real time monitoring important?

Streams are very sensitive to weather and pollution events. These events can occur over a very short time, often after a few hours or minutes the event is over. Monthly sampling will miss most important events that occur in your stream.
With real time monitoring:

<b>Don’t miss events.</b> Things can change fast in a stream, if you’re not there to see it you’ll miss it, the sensor watches 24/7 and creates a continuous record.</br>
<b>Narrow down the cause.</b> If your stream is monitored at multiple points you can narrow down where and when an event occurred and its possible cause.</br>
<b>Create positive behaviours.</b> We all change how we behave if we know we’re being watched.  With the stream monitor people become aware that dumping and faults are more likely to be detected.</br>
<b>See the effect of restoration work.</b>  The sensor creates long term trend data. The improvements made by restoration work can be shown over time.</br>
<b>You can learn more about your stream and others.</b> Combing your data with the work done by others can lead to new insights previously unavailable.</br>

