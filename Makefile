#TARGET?=Mega2560
TARGET?=ESP32
PORT?=/dev/ttyUSB0
#ESP32_FILESYSTEM=littlefs
#ESP32_PSRAM=enabled
ESP32_FILESYSTEM=spiffs
ESP32_FILESYSTEM_PART=spiffs
ESP32_PARTSCHEME=min_spiffs
ESP32_FLASHSIZE=4MB
GITHUB_REPOS= \
reeltwo/Reeltwo \
adafruit/Adafruit_NeoPixel \
FastLED/FastLED \
DFRobot/DFRobotDFPlayerMini

include ../Arduino.mk
