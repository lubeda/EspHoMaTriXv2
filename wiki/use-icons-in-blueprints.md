# Introduction

**Skill-level:** advanced

When working with EspHoMaTriXv2, you will grow the variety of icons you use.

If you like to use the comfort of the blueprints, you have to manually synchronize your icons to the blueprints.

## Step 1

compile your YAML and copy a portion of the compilation result:

![screenshot](https://github.com/lubeda/EspHoMaTriXv2/blob/main/wiki/compile-icons.png?raw=true)

```yaml
["error","github","precipitation","nina","waschmaschine","lamp","headphone","print3d","internet","speaker","alien","temp","garage","door","wind","rain","phone","fire","tv","frost","muell","cookit","nature","work","bike","amazon","post","power","solar","yoga","startrek","energy","sun","diesel","benzine10","vacuum","rainprecip","iss","thunder","nina_warning","mic","birthday","firework","coffee","lightning","xmastree","sauna","trash_grey","trash_blue","trash_yell","trash_brow","weather_clear_night","weather_cloudy","weather_fog","weather_lightingrainy","weather_partlycloudy","weather_pouring","weather_rainy","weather_snowy","weather_snowy_rainy","weather_sunny","f_mario","f_onair","f_starter","mops","girl","timer","lasticon","lightbulb","home_assistant",]
```

## Step 2

copy this part to your blueprints, e.g., `EHMTX_extended_state.yaml`

find the provided icon-list:

```yaml
  icon_name:
      name: The icon associated to this state screen?
      description: If you use additional icons in your YAML, you have to add them to this blueprint.
      selector:
        select:
          mode: dropdown
          options:
            [
              "error",
              "home_assistant",
              "temperature",
              "lightbulb",
              "music",
              "phone",
              "car",
              "sleep8x32",
            ]
```

and replace it with your copied version

```yaml
  icon_name:
      name: The icon associated to this state screen?
      description: If you use additional icons in your YAML, you have to add them to this blueprint.
      selector:
        select:
          mode: dropdown
          options:
            ["error","github","precipitation","nina","waschmaschine","lamp","headphone","print3d","internet","speaker","alien","temp","garage","door","wind","rain","phone","fire","tv","frost","muell","cookit","nature","work","bike","amazon","post","power","solar","yoga","startrek","energy","sun","diesel","benzine10","vacuum","rainprecip","iss","thunder","nina_warning","mic","birthday","firework","coffee","lightning","xmastree","sauna","trash_grey","trash_blue","trash_yell","trash_brow","weather_clear_night","weather_cloudy","weather_fog","weather_lightingrainy","weather_partlycloudy","weather_pouring","weather_rainy","weather_snowy","weather_snowy_rainy","weather_sunny","f_mario","f_onair","f_starter","mops","girl","timer","lasticon","lightbulb","home_assistant",]
```

Take care that the indentation is correct

## step 3

Save the changed file and reload your automations

## step 4

After checking if it is working, repeat the procedure for all blueprints using the icon dropdown

## sample for manual automation

this automation example display states with icons and a rainbow colored text for state changes of multiple icons.
The icon is selected by the `trigger.id`, the text is dynamicaly build by the triggered state.

```yaml
alias: EHMTX State display
trigger:
  - platform: state
    entity_id: sensor.001cac04_wind_speed
    id: wind
  - platform: state
    entity_id: sensor.tankerkoenig_super
    id: supere10
condition: []
action:
  - service: esphome.ulanzi_rainbow_icon_screen
    data:
      icon_name: "{{trigger.id}}"
      screen_time: 12
      lifetime: 2
      text: '{{trigger.to_state.attributes.friendly_name}}: {{trigger.to_state.state}}
          {{trigger.to_state.attributes.unit_of_measurement}}|replace ("€","¼")| replace ("°C","¬")|replace ("℃","¬")| replace ("°F","¯")|replace ("℉","¯")|replace ("₴","¾")|replace ("₿","¨")|replace ("₩","½")|replace ("Ξ","¸")'
      default_font: true
mode: parallel
max: 2
```

## warnings

- If you remove icons from your list, old automations will probably use these icons, resulting in displaying the default icon!
- If you don't use the correct indentation, your automations will fail. **No Backup, no compassion!**
- The default icon must exist in your list.
