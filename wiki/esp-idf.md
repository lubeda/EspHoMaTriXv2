# Using esp-idf instead of arduino framework

**Skill-level:** high

**minimum version** 2025.5.1

For some reasons it might be neccassery to ise the esp idf instead of the arduino framework.

There are some dependencies

change the esp32: section like this

```yaml
esp32:
  board: esp32dev
  framework:
    type: esp-idf
    version: recommended
```

remove the `neopixelbus` light

```yaml
light:
  #removve
  - platform: neopixelbus
```

and replace it with an `esp32_rmt_led_strip`
```yaml
light:
  - platform: esp32_rmt_led_strip
    id: ehmtx_light
    rgb_order: GRB
    pin: $matrix_pin
    num_leds: 256
    chipset: ws2812
    name: "$devicename Light"
    restore_mode: ALWAYS_OFF
```

## Limitations
the fire plugin and the rainbow shimmer text does not work with tthis settings

**Use this hint at your own risk**

Good luck!