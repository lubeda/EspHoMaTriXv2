# Setup

## setup a timer helper named *eier_timer*.

## How does it work?

**home assistant automation:**

triggered by the timer start, the timer display is refreshed every second


```yaml
alias: Display Timer on ulanzi
trigger:
  - platform: state
    entity_id:
      - timer.eier_timer
    to: active
condition:
  - condition: state
    entity_id: timer.eier_timer
    state: active
action:
  - service: esphome.ulanzi_icon_screen
    data:
      default_font: true
      icon_name: timer
      screen_time: 10
      lifetime: 1
      r: 200
      b: 200
      g: 50
      text: >-
        {{ (state_attr('timer.eier_timer', 'finishes_at') | as_datetime -
        now()).total_seconds() | int(0) }}
  - service: esphome.ulanzi_force_screen
    data:
      icon_name: timer
  - delay:
      hours: 0
      minutes: 0
      seconds: 1
      milliseconds: 0
  - service: automation.trigger
    data:
      skip_condition: false
    target:
      entity_id: automation.display_timer_on_ulanzi
mode: restart
```