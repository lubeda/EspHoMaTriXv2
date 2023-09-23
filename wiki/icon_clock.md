# Introduction

**Skill-level:** medium

**minimum version** 2023.8.0

Assume you have got a weather sensor in home assistant that returns the current weather condition. E.G. "cloudy" with this code in your yaml you will display the time and the icon according to your weather. Therefore you need an icon for every possible condition with the id: *weather_***condition**

```yaml
text_sensor:
  - platform: homeassistant
    id: current_icon
    entity_id: weather.buienradar
    filters:
      prepend: "weather_"
    on_value:
      then:
        - lambda:
             id(rgb8x32).icon_clock(x,120,7);
```