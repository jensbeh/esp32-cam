# ESP32-CAM with motiton detector

This code turns the ESP32-CAM into a security system with motion detection:

The ESP hosts a WebServer that provides the camera stream and a WebSocketServer that is used to communicate commands between the [Android app](https://github.com/jensbeh/esp32-cam-android-app) and the Esp. It also sends a notification to the [Android app](https://github.com/jensbeh/esp32-cam-android-app) with an image when it detects motion.

## Usage
As soon as the ESP has been flashed with the firmware, it sets up its own "ESP camera" wifi network. A wifi-enabled PC must now connect to this network. Once connected, the address http://192.168.4.1/ can be loaded in the browser and the ESP lists all available networks and you can choose one of them to connect to.
When the ESP is connected, this is visually displayed in the browser and the ESP automatically restarts and connects to the wifi.
Now the [Android app](https://github.com/jensbeh/esp32-cam-android-app) can be installed and started. All further instructions can be found in the app.

## Controls
* Framesize (1600 x 1200, 1280 x 1024, 1024 x 768, 800 x 600, 640 x 480, 352 x 288, 320 x 240, 240 x 176, 160 x 120)
* Quality (10 to 63)
* Brightness (-2 to 2)
* Contrast (-2 to 2)
* Saturation (-2 to 2)
* Special Effects (No Effect, Negative, Grayscale, Red Tint, Green Tint, Blue Tint, Sepia)
* Auto White Balance (On/Off)
* Auto White Balance Gain (On/Off)
* White Balance Mode (Auto, Sunny, Cloudy, Office, Home) (only if Auto White Balance Gain is on)
* Exposure Control (On/Off)
* Aec value (0 to 1200) (only if Exposure Control is off)
* Aec2 (On/Off)
* Ae level (-2 to 2)
* Agc Control (On/Off)
* Agc Gain (0 to 30) (only if Agc Control is off)
* Gain Ceiling (0 to 6) (only if Agc Control is on)
* Bpc (On/Off)
* Wpc (On/Off)
* Raw Gma (On/Off)
* Lens Correction (On/Off)
* H-Mirror (On/Off)
* V-Flip (On/Off)
* Colorbar (On/Off)
* Flashlight (On/Off)


## Hardware
### Components used for this project:

* ESP32-CAM - https://www.banggood.com/de/ESP32-CAM-MB-WiFi-MICRO-USB-ESP32-Serial-to-WiFi-ESP32-CAM-Development-Board-CH340G-5V-Bluetooth+OV2640-Camera+2_4G-Antenna-IPX-p-1847153.html?rmmds=myorder&cur_warehouse=CZ&ID=566571
* HC-SR501 - https://www.amazon.de/gp/product/B07CNBYRQ7/ref=ppx_yo_dt_b_asin_title_o01_s00?ie=UTF8&th=1


### Motion sensor connection (Motion sensor -> ESP32-CAM):
* +Power -> 5V
* GND -> GND
* High / Low Output -> GPIO2 (you can define your own pin in constants.h if you want)

# Compiling and uploading

This project is set up for [PlatformIO](https://platformio.org).<br/>
E.g.: you can use [Visual Studio Code](https://code.visualstudio.com/) with [PlatformIO Extentsion](https://marketplace.visualstudio.com/items?itemName=platformio.platformio-ide)
* open this project in your IDE
* download/import the extension in PlatformIO [WebSocket from Github](https://github.com/Links2004/arduinoWebSockets)
* adjust `platformio.ini` as needed, e.g.:
    ```
    platform = espressif32
    board = esp32cam
    framework = arduino
    monitor_speed = 115200
    monitor_rts = 0
    monitor_dtr = 0
    lib_deps = links2004/WebSockets@^2.3.6
    ```
* build and upload

# Bug report
Found a bug? Please post on [GitHub](https://github.com/jensbeh/esp32-cam/issues).