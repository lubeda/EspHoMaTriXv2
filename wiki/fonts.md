# fonts

**Skill-level:** all

Due to limitations of the display there are only some font useful here are some examples.

## EHMTXv2.ttf

### Font definition

```yaml
- file: ehmtx.ttf
    id: default_font
    size: 16
    glyphs:  |
      !?"%()+*=,-_.:°0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnÖÄÜöäüopqrstuvwxyz€@<>/
```

### Configuration

```yaml
ehmtx:
  show_dow: true
  default_font_yoffset: 6
  default_font_id: default_font
  ....
```

[ttf-file](https://github.com/lubeda/EspHoMaTriXv2/blob/main/copy2esphome/EHMTXv2.ttf)
![sample](https://github.com/lubeda/EspHoMaTriXv2/blob/main/wiki/ehmtxfont.png?raw=true)

## mateine.ttf

These fonts are eight pixels high, so you have to disable the day of week in clock display

### Font definition

```yaml
font:
  - file: mateine.ttf
    size: 16
    id: default_font
    glyphs:  |
      !?"%()+*=,-_.:°0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnÖÄÜöäüopqrstuvwxyz@<>ß§€/
```

[8 pixel font]https://github.com/lubeda/EspHoMaTriXv2/blob/main/copy2esphome/mateine.ttf
![sample](https://github.com/lubeda/EspHoMaTriXv2/blob/main/wiki/mateinefont.png?raw=true)

```yaml
font:
  - file: mateineThin.ttf
    size: 16
    id: default_font
    glyphs:  |
      !?"%()+*=,-_.:°0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnÖÄÜöäüopqrstuvwxyz@<>ß§€/
```

[8 pixel font thin]https://github.com/lubeda/EspHoMaTriXv2/blob/main/copy2esphome/mateineThin.ttf
![sample](https://github.com/lubeda/EspHoMaTriXv2/blob/main/wiki/mateinethinfont.png?raw=true)

### Configuration

```yaml
ehmtx:
  show_dow: true
  default_font_yoffset: 6
  default_font_id: default_font
  ....
```


## Trip5s fonts

Details for this fonts are well documented in his repo

[REPO](https://github.com/trip5/MatrixClockFonts)
