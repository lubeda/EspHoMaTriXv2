# Introduction

**Skill-level:** medium

Assuming you are using a matrix with connected small sides like [this](https://github-production-user-asset-6210df.s3.amazonaws.com/18141890/243566536-eba5cc3b-3a4b-467f-8775-d291bcb61179.mp4) e.g. using this [case](https://www.printables.com/model/217568-led-matrix-lamp).

Since 2023.6.4 you can set this option:

```yaml
ehmtxv2:
  ...
  scroll_small_text: true
  ...
```

to display text thas is scrolled around the display even if it is short and normally should be centered.

To display time you can use this automation:

```yaml
service: esphome.m5mic_rainbow_text_screen
data:
  default_font: false
  screen_time: 15
  lifetime: 1440
  text: |-
          {{ states("sensor.time") }}
```

or date

```yaml
service: esphome.m5mic_rainbow_text_screen
data:
  default_font: false
  screen_time: 15
  lifetime: 1440
  text: |-
          {{  now().strftime("%d.%m.%Y") }}
```

to remove the normal date and time display

```yaml
service: esphome.m5mic_del_screen
data:
  icon_name: "*"
  mode: 3
```

```yaml
service: esphome.m5mic_del_screen
data:
  icon_name: "*"
  mode: 2
```