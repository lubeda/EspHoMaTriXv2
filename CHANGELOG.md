# Changelog

## 2024.1.1

cleaned up docu

## 2023.9.1
- Added night mode
  - `night_mode_on`
  - `night_mode_off`
  - New Yaml option `night_mode_screens: [2, 3, 16]` - list of screens to be displayed in night mode.
  - `on_night_mode` trigger
- Added `on_show_display` trigger
- Added the ability to specify a screen ID - `icon_name|screen_id`
- Added icon and date output screen ![icon_clock](images/icon_clock.png)
  - `icon_date(iconname, lifetime, screen_time, default_font, r, g, b)`
- If pass a screen identifier with the value `[day, weekday]` like `icon_name|day`, and a backing icon to `icon_clock` or `icon_date`, it will display text. [Example](https://github.com/lubeda/EspHoMaTriXv2/issues/92#issuecomment-1750870290)
- New YAML option `weekdays: "SUMOTUWETHFRSA"` (**7 or 14** characters) and new function to customize the info text over the icon:
  - `set_infotext_color(200,100,100,100,100,200,false,2);` 
  - `set_infotext_color("left_r", "left_g", "left_b", "right_r", "right_g", "right_b", "default_font", "y_offset");`
  - `weekdays` - the order of the days of the week, from Sunday to Saturday.
- Example for **weekdays**:
  - `weekdays: "일월화수목금토"`
  - `weekdays: "НДПНВТСРЧТПТСБ"`
- Added a screen with the ability to display a progress bar, progress value `(-100..100)` [Example](https://github.com/lubeda/EspHoMaTriXv2/issues/91#issuecomment-1759487850)
  - `icon_screen_progress(iconname, text, progress, lifetime, screen_time, default_font, r, g, b)`
- Added `replace_time_date_to:` and `replace_time_date_from:` which allow replacing the system date & time text (anything!) [More info](https://github.com/lubeda/EspHoMaTriXv2/pull/104)
- Added `bitmap_small` and `rainbow_bitmap_small` screen.
- Added a pseudo-icon `blank` - empty icon, no display.
- Added screen with scroll icon along with long text, `icon_text_screen`, `rainbow_icon_text_screen`. [Example](https://github.com/lubeda/EspHoMaTriXv2/issues/123#issuecomment-1784137055)
- Added `bitmap_stack`screen. Screen ![bitmap_stack](images/bitmap_stack.png)  that allows you to display from 1 to 64 icons described in the configuration. [Example](https://github.com/lubeda/EspHoMaTriXv2/issues/139#issuecomment-1803606627)
- Added a screen with the ability to display a progress bar, text, progress value `(-100..100)` [More info](https://github.com/lubeda/EspHoMaTriXv2/issues/143)
  - `text_screen_progress(text, value, progress, lifetime, screen_time, default_font, value_color_as_progress, r, g, b)`
- Added `set_weekday_accent_on` and `set_weekday_accent_off` Turn on / off small days (accent) of the week when brightness is insufficient. [Example](https://github.com/lubeda/EspHoMaTriXv2/pull/151#issuecomment-1813089680)
- Added `icon_prognosis_screen` and `icon_prognosis_screen_rgb` Displays an icon, text, and a prognosis bar consisting of 24 dots of specified colors. [More info](https://github.com/lubeda/EspHoMaTriXv2/issues/149)
- Added `vertical_scroll` to ehmtxv2 config.
- Added Advanced clock mode `advanced_clock`, [More info](https://github.com/lubeda/EspHoMaTriXv2/issues/164)
- Added `icon_indicator`, Shows the indicator ![indicator](images/icon_indicator.png) in the Icons area on the specified screens, in the specified color and at the specified vertical position. [Example](https://github.com/lubeda/EspHoMaTriXv2/pull/170#issuecomment-1836539171)
- Added `rainbow_alert_screen`, show the specified icon with text in rainbow color, screen forced and lifetime = screen_time
- Added a pseudo-icon `solid` - icon as square filled with solid color, see `set_solid_color`.
- Added a pseudo-icon `calendar` - calendar icon with default whitecolor, and header as color from `set_calendar_color`.
- Added Advanced Bitmap mode `advanced_bitmap`, [More info](#advanced-bitmap-mode)

## 2023.9.0
- Added the ability to display graph as defined in the YAML file

## 2023.8.0
- Added icon and time output screen
  - `icon_clock(iconname, lifetime, screen_time, default_font, r, g, b)`
- Added alert screen
  - `alert_screen(iconname, text, screen_time, default_font, r, g, b)`

## 2023.8.0

A lot of users asked for special behavior, e.g. display date but no time or vice versa. Display nothing at all e.g. during night. I like to implement a lot of the requests but it's hard to keep the known default behaviour. In this version i will provide a YAML with the default behaviour but there will be less hardcoded by me, instead you have your freedom to do  all in your YAML.

For the old default behavior use this trigger in your YAML:

```yaml
  on_empty_queue:
    then:
      lambda: |-
        id(rgb8x32)->clock_screen(10,10);
        id(rgb8x32)->date_screen(10,5);
  on_start_running:
     then:
       lambda: |-
          id(rgb8x32)->bitmap_screen("[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,63519,63519,63519,63519,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,63519,0,0,0,0,2016,0,0,0,0,0,0,0,0,0,0,31,0,0,0,0,0,0,0,0,0,63488,0,63488,0,0,0,63519,0,0,0,0,2016,2016,0,0,0,65514,0,65514,0,0,0,31,0,0,0,64512,0,0,64512,0,63488,63488,0,63488,63488,0,0,63519,63519,63519,0,0,2016,0,2016,0,65514,0,65514,0,65514,0,31,31,31,0,0,0,64512,64512,0,0,63488,63488,63488,63488,63488,0,0,63519,0,0,0,0,2016,0,2016,0,65514,0,65514,0,65514,0,0,31,0,0,0,0,64512,64512,0,0,0,63488,63488,63488,0,0,0,63519,63519,63519,63519,0,2016,0,2016,0,65514,0,65514,0,65514,0,0,0,31,31,0,64512,0,0,64512,0,0,0,63488,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]",1,10);
          id(rgb8x32)->bitmap_small("[2016,0,0,0,2016,0,0,0,2016,0,0,0,2016,0,0,0,2016,0,0,0,2016,0,0,0,0,2016,0,2016,0,31,31,0,0,0,2016,0,31,0,0,31,0,0,0,0,0,0,31,0,0,0,0,0,0,31,0,0,0,0,0,0,31,31,31,31]", "2023.x.y", 1, 10);
          id(rgb8x32)->clock_screen(10,10);
          id(rgb8x32)->date_screen(10,5);
```

The `on_start_running`-trigger is called after the device boot once. The `on_empty_queue`-trigger is called when there is nothing in the queue. With the examples you will always have a clock and date displayed. If you don't like or want other colors e.t.c. feel free the change your YAML and have fun.

If you don't add this trigger you have a blank display until your hosts add screens via service calls.

## 2023.8.0

- added on_empty_queue trigger
- added bitmap-convert.html to help creating rgb565 arrays

## 2023.7.1

- reintroduced set_clock_color

## 2023.7.0

- added always_show_rl_indicators boolean
- added on_start_running trigger to allow for additional configuration when 
  matrix starts display
- icontype rgb565 array as string
- fixed scroll small text
- introduced mateine.ttf and mateineThin.ttf from @dbuezas [samples](https://github.com/lubeda/EspHoMaTriXv2/issues/63)
- fixed hold_screen [issue](https://github.com/lubeda/EspHoMaTriXv2/issues/58)

## 2023.6.5

- introduced `blend_steps: 16`
- introduced service color_gauge
- fixed bitmap_small with gauge
- fixed del_screen with "*"
- improved blueprint selection @andrew-codechimp