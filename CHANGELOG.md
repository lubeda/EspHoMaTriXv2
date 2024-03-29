# Changelog

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

## 2023.6.3

- introduced `default_clock_font: true`  
- changed *_interval to be set only on compile time
- change scroll_count to be set only on compile time
- change font_offsets to be set only on compile time
- change date/time_format to be set only on compile time
- remove unused frame_interval

## 2023.6.2

- set rtl mode at compile time
- changed clock to default font

## 2023.6.1

- fixed show_date, added show_date to the docmentation
- new small logo

## 2023.6.0

- first release of v2

## 2023.5.0

- beta release
