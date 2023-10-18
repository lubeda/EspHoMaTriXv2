# Introduction

I'm using EspHoMaTriXv2 on a Ulanzi TC001. I want to display the time for 55 seconds and the date for 5 seconds in a loop. I don't understand, how I can configure the display times. I tried clock_interval: 55 but it doesn't seem to help.

What's the correct way?

## new Method with lambda:

add this to your config:

```yaml
  on_start_running:
    then:  
      lambda: |-
        ESP_LOGD(TAG, "removing date screen and setting a new one with new screen time");
        id(rgb8x32)->del_screen("*", 3);
        id(rgb8x32)->date_screen(1440, 5, true, 192, 192, 192);
        ESP_LOGD(TAG, "removing default clock screen and setting a new one with new screen time");
        id(rgb8x32)->del_screen("*", 2);
        id(rgb8x32)->clock_screen(1440, 55, true, 192, 192, 192);
```

## old Method:

**Skill-level:** easy

Set up a timer helper named *eier_timer*

### step 1: **home assistant automation:**

I think the easiest way is by a home assistant script like this:

```yaml
alias: ulanzi 55-5 clock-date
sequence:
  - service: esphome.ulanzi_del_screen
    data:
      icon_name: "*"
      mode: 2
  - service: esphome.ulanzi_clock_screen
    data:
      default_font: false
      lifetime: 1440
      screen_time: 55
      r: 192
      g: 192
      b: 192
  - service: esphome.ulanzi_del_screen
    data:
      icon_name: "*"
      mode: 3
  - service: esphome.ulanzi_date_screen
    data:
      default_font: false
      lifetime: 1440
      screen_time: 5
      r: 192
      g: 192
      b: 192
mode: single
icon: mdi:clock-digital
```

This deletes the default date and clock display and fills the loop with a 55 sec clock and a 5 sec date screen. The lifetime is one day, so the script must run once a day or on boot.

This is also possible in the esphome YAML, with lambdas.
