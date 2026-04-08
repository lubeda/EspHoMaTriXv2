# EspHoMaTriXv2 Quick Start Guide

**EspHoMaTriXv2** is a flexible, customizable DIY LED Matrix display built with an 8×32 RGB LED panel and ESPHome.

## Installation

### 1. Quick Install (Ulanzi TC001)

1. Copy `ulanzi-easy.yaml` and `MatrixChunky6.ttf` from the `install` folder to your ESPHome folder
2. Configure your Wi-Fi and OTA password in `secrets.yaml`
3. Define weekdays for your language in the YAML
4. Flash via USB and wait ~30 seconds for time sync

### 2. Add to Your YAML

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/lubeda/EspHoMaTriXv2
      ref: 2025.12.0
    refresh: 60s
    components: [ ehmtxv2 ]
```

## Screen Types

### Icon Screen
```yaml
service: esphome.ulanzi_icon_screen
data:
  icon_name: solar
  text: sun is shining
  lifetime: 2
  screen_time: 10
  r: 200
  g: 200
  b: 200
```

### Alert Screen (immediate display, no lifetime)
```yaml
service: esphome.ulanzi_alert_screen
data:
  icon_name: home_assistant
  text: Important!
  screen_time: 10
  r: 20
  g: 180
  b: 240
```

### Rainbow Icon Screen
```yaml
service: esphome.ulanzi_rainbow_icon_screen
data:
  icon_name: solar
  text: colorful text
  lifetime: 2
  screen_time: 10
```

### Text-Only Screen
```yaml
service: esphome.ulanzi_text_screen
data:
  text: Your message
  lifetime: 2
  screen_time: 10
  r: 200
  g: 200
  b: 200
```

### Progress Screen
```yaml
service: esphome.ulanzi_icon_screen_progress
data:
  icon_name: print3d
  text: 67% done
  progress: 67
  lifetime: 2
  screen_time: 9
```

### Bitmap Stack (multiple icons)
```yaml
service: esphome.ulanzi_bitmap_stack
data:
  icons: solar,power,temp,error
  lifetime: 2
  screen_time: 10
```

### Blank Screen
```yaml
service: esphome.ulanzi_blank_screen
data:
  lifetime: 2
  screen_time: 120
```

## Removing Screens

```yaml
service: esphome.ulanzi_del_screen
data:
  icon_name: "power"
  mode: 5
```

## Forcing a Screen

```yaml
service: esphome.ulanzi_force_screen
data:
  icon_name: "solar"
  mode: 5
```

## Parameters

| Parameter | Description | Default |
|-----------|-------------|---------|
| `lifetime` | Minutes until screen is removed from queue | 2 |
| `screen_time` | Seconds to display the screen | 10 |
| `icon_name` | Icon to display | - |
| `text` | Text to show | - |
| `r`, `g`, `b` | RGB color (0-255) | 200, 200, 200 |

## Available Icons

Pre-installed icons: solar, power, temp, error, home, home_assistant, etc.

See [default icons](https://raw.githubusercontent.com/lubeda/EspHoMaTriXv2/2024.1.0/resources/default_icons.html) for full list.

## Display Elements

- **Alarm**: Upper-right corner (red)
- **Right indicator**: Lower-right corner (yellow)
- **Left indicator**: Lower-left corner (yellow)

## Next Steps

- Add custom icons and animations
- Configure multiple fonts
- Set up advanced triggers and automations

See the full [README.md](./README.md) for detailed documentation.
