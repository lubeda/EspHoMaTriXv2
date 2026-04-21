# EspHoMaTriXv2 Overview

[donation-badge]:https://img.shields.io/badge/PayPal-00457C?style=for-the-badge&logo=paypal&logoColor=white
[donation-url]: https://www.paypal.com/donate/?hosted_button_id=FZDKSLQ46HJTU

![Home Assistant](https://img.shields.io/badge/home%20assistant-%2341BDF5.svg?style=for-the-badge&logo=home-assistant&logoColor=white)
![GitHub](https://img.shields.io/badge/github-%23121011.svg?style=for-the-badge&logo=github&logoColor=white)
[![Wiki](https://github.com/lubeda/EspHoMaTriXv2/actions/workflows/wiki.yaml/badge.svg)](https://github.com/lubeda/EspHoMaTriXv2/wiki)

**EspHoMaTriXv2** is a customizable DIY LED Matrix display built with an 8×32 RGB LED panel and ESPHome.

## Key Points

- Displays clock, date, and up to 24 custom screens
- Control via Home Assistant service calls or ESPHome lambdas
- Supports 8x8 icons and 8x32 GIF animations
- Queue-based system with lifetime for each screen
- Recommended: **ESP32** (ESP8266 has limited RAM)

## Quick Start

## Version Compatibility

| Branch | ESPHome Version |
|--------|-----------------|
| 2026.4.1 | 2026.4.1+ |
| 2025.12.0 | 2025.12.0+ |
| 2025.10.3 | 2025.10.0+ |
| 2025.7.1 | 2025.4.0+ |

**Keep ESPHome updated** - breaking changes may require YAML adjustments.

## Available Services

- `ulanzi_icon_screen` - Display icon with text
- `ulanzi_alert_screen` - Immediate display (no lifetime)
- `ulanzi_rainbow_icon_screen` - Rainbow colored text
- `ulanzi_text_screen` - Text only
- `ulanzi_icon_screen_progress` - Progress bar
- `ulanzi_bitmap_stack` - Multiple icons
- `ulanzi_blank_screen` - Clear display
- `ulanzi_del_screen` - Remove screen from queue
- `ulanzi_force_screen` - Force display specific screen

## Documentation

- [Full README](https://github.com/lubeda/EspHoMaTriXv2/blob/2026.4.1/README.md)
- [Wiki](./wiki)
