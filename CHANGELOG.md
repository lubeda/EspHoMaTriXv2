# Changelog

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