Show weather state with icons that match the weather conditions.

**ulanzi.yaml**
```yaml
......
ehmtx:
  icons:
    - id: weather_clear_night
      lameid: 52163
    - id: weather_cloudy
      frame_duration: 192
      lameid: 25991
    - id: weather_fog
      lameid: 52167
    - id: weather_lightingrainy
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

```

**homeassistant automation:**

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