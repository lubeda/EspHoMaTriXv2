# Introduction

todo => adapt to 2023.8.0

**Skill-level:** low

EspHoMaTriXv2 is a pixel clock. Without any configuration it displays only the time and date. These are "build in", but can be widely configured [format](time-date-format). If you don't need the date-display, you can disable it with the parameter `show_date: false` in the YAML. Or if you don't want either time or date add `allow_empty_screen: true` to your yaml.

During runtime, you can remove them from the queue with:

```yaml
service: esphome.ulanzi_del_screen
data:
  icon_name: "*"
  mode: 3
```

```yaml
service: esphome.ulanzi_del_screen
data:
  icon_name: "*"
  mode: 2
```

and add them with:

```yaml
service: esphome.ulanzi_clock_screen
data:
  default_font: true
  lifetime: 1440
  screen_time: 10
  r: 192
  g: 192
  b: 192
```

and
```yaml
service: esphome.ulanzi_date_screen
data:
  default_font: true
  lifetime: 1440
  screen_time: 5
  r: 192
  g: 192
  b: 192
```