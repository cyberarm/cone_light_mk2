# Cone Light mk.II Firmware

ESP32-C6 MCU connected to an OLED display, 6 WS2812b leds, and an active beeper.

## Features

- Glow to illuminate cone
- Sync with clusters to play led animations and/or chiptunes
- (theoretical) RF based line of sight beam break detection with other nodes

## Arduino Libraries

- Tween
- FastLED
- Adafruit_SSD1306
  - Adafruit_GFX
- AsyncTCP
- ESPAsyncWebServer

## Development

### Arduino IDE

> IMPORTANT: set 'partition scheme' under tools menu to 'No OTA' (2MB/2MB) since compiled firmware is to large to fit with space reserved for Over-The-Air updates.

Manually compile with arduino-cli when compilation output gets cut off

```
arduino-cli compile --fqbn esp32:esp32:XIAO_ESP32C6 .
```

### PlatformIO IDE

Have fun. :face_holding_back_tears:
