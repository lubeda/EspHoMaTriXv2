# Introduction

**Skill-level:** medium

Show weather state with icons that match the weather conditions.

## step 1: add icons to **ulanzi.yaml**

```yaml
......
ehmtx:
  icons:
    - id: weather_clear_night
      lameid: 53383
    - id: weather_cloudy
      frame_duration: 192
      lameid: 25991
    - id: weather_exceptional
      lameid: 16754
    - id: weather_fog
      lameid: 52167
    - id: weather_hail
      lameid: 53288
    - id: weather_lightning
      lameid: 23713
    - id: weather_lightning_rainy
      lameid: 49299
    - id: weather_partlycloudy
      frame_duration: 210
      lameid: 22160
    - id: weather_pouring
      lameid: 49300
    - id: weather_rainy
      lameid: 26565
    - id: weather_snowy
      lameid: 2289
    - id: weather_snowy_rainy
      lameid: 49301
    - id: weather_sunny
      lameid: 11201
    - id: weather_windy
      lameid: 15618
    - id: weather_windy_variant
      lameid: 15618
    - id: weather_cloudy_night
      lameid: 12195

```

## Step 2: enter **home assistant automation:**

```yaml
alias: EHMTXv2 Weather
description: shows the local weatherdata with the icon based on cloud service
trigger:
  - platform: state
    entity_id: weather.athome
action:
  - service: esphome.ulanzi_del_screen
    data:
      icon_name: weather_*
  - service: esphome.ulanzi_add_screen
    data:
      icon_name: weather_{{ trigger.to_state.state | replace("-","_")}}
      text: >-
        {{ states("sensor.92bc04_actual_temperature") }}°C/{{
        states("sensor.92bc04_wind_speed")}}km/h/{{
        states("sensor.92bc04_humidity") }}%
```

## Alternate Step 2: **Home Assistant Automation: Temperature & Icon (with OpenWeatherMap integration)**

```yaml
  - if:
      - condition: or
        conditions:
          - condition: state
            entity_id: sensor.openweathermap_condition
            state: cloudy
          - condition: state
            entity_id: sensor.openweathermap_condition
            state: partlycloudy
      - condition: or
        conditions:
          - condition: sun
            after: sunset
          - condition: sun
            before: sunrise
    then:
      - service: esphome.pixelclock_icon_screen
        data:
          default_font: true
          icon_name: cloudy_night
          text: "{{ states('sensor.openweathermap_temperature')|round(1) }}°"
          lifetime: 5
          screen_time: 3
          r: 192
          g: 192
          b: 192
    else:
      - service: esphome.pixelclock_icon_screen
        data:
          default_font: true
          icon_name: >-
            weather_{{ states('sensor.openweathermap_condition')|replace("-",
            "_") }}
          text: "{{ states('sensor.openweathermap_temperature')|round(1) }}°"
          lifetime: 5
          screen_time: 3
          r: 192
          g: 192
          b: 192
```

### Add error-checking to sensors which may go offline (note the continue_on_error bit too!)

```
      - service: esphome.pixelclock_icon_screen
        data:
          default_font: true
          icon_name: >-
            weather_{{ states('sensor.openweathermap_condition')|replace("-",
            "_") }}
          text: >
            {% set state = states('sensor.openweathermap_temperature') %} {% if
            is_number(state) %}
              {{ state|round(1) }}°
            {% else %}
              {{ "error" }}
            {% endif %}
          lifetime: 5
          screen_time: 3
          r: 192
          g: 192
          b: 192
        continue_on_error: true
```
