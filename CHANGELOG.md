# Changelog

## 2023.4.1
- breaking: removed show_icons

## 2023.4.0
- new: set_screen_color service to set color for a screen (service & action)
- breaking: all settings and parameters are named more consistant
- changed: center small text instead of left aligned
- removed: select component

## 2023.3.5
- new: show_seconds indicator top left corner
- breaking: removed automatic scaling of images
- new: support 8x32 icons without text
- breaking: added status,display_on,display_off as default service => remove these from your yaml
- breaking: added indicator_on/off as default service => remove these from your yaml
- breaking: added alarm_color,text_color,clock_color as default service => remove these from your yaml
- breaking: gauge is also schown while the clock is displayed but without moving the screen to the right
- breaking: show_icons as default service => remove from yaml

## 2023.3.4

- added: option to not display clock/date #53
- added: dynamic set_show_clock
- added: on_next_clock trigger

## 2023.3.3

- fixed: force_screen skips imediatly to the selected screen
- added: hold_time configurable 

## 2023.3.2

- added: hold_screen for 20 additional seconds

## 2023.3.1

- added: del_screen with wildcards
- changed: maximum icons to 80
- fixed: skip_next
- fixed: show_all_icons on boot

## 2023.3.0

see README.md for features
e.g. Service **display_on** / **display_off**
