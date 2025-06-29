# EspHoMaTriX version 2 (EHMTXv2)

[donation-badge]:https://img.shields.io/badge/PayPal-00457C?style=for-the-badge&logo=paypal&logoColor=white
[donation-url]: https://www.paypal.com/donate/?hosted_button_id=FZDKSLQ46HJTU

![Home Assistant](https://img.shields.io/badge/home%20assistant-%2341BDF5.svg?style=for-the-badge&logo=home-assistant&logoColor=white)
![GitHub](https://img.shields.io/badge/github-%23121011.svg?style=for-the-badge&logo=github&logoColor=white)
[![Donate][donation-badge]][donation-url]
[![Publish wiki](https://github.com/lubeda/EspHoMaTriXv2/actions/workflows/wiki.yaml/badge.svg)](https://github.com/lubeda/EspHoMaTriXv2/actions/workflows/wiki.yaml)
[![Build](https://github.com/lubeda/EspHoMaTriXv2/actions/workflows/build.yaml/badge.svg)](https://github.com/lubeda/EspHoMaTriXv2/actions/workflows/build.yaml)


# Important

Each esphome release is packed with new features and suprises (breaking changes), so it is hard to stay up to date for me. Each user can decide which version of esphome he uses. So there is not one combination of esphome, ehmtx and yaml that fits all.

## So ehmtx is prone to breaking on every esphome update.

**My suggestion is:**

**Keep esphome up to date with the latest official version!**

**Which are:**
- [Branch 2025.6.0](https://github.com/lubeda/EspHoMaTriXv2/tree/2025.6.0) for esphome 2025.6.0- (some fixes and PRs from andrewjswan)
- [Branch 2025.5.2](https://github.com/lubeda/EspHoMaTriXv2/tree/2025.5.2) for esphome 2025.4.0- (some fixes "empty queue")
- [Branch 2025.5.1](https://github.com/lubeda/EspHoMaTriXv2/tree/2025.5.1) for esphome 2025.4.0- 
- [Branch 2025.2.1](https://github.com/lubeda/EspHoMaTriXv2/tree/2025.2.1) for esphome 2025.2.0- (optimized handling of transparent icons)
- [Branch 2025.2.0](https://github.com/lubeda/EspHoMaTriXv2/tree/2025.2.0) for esphome 2025.2.0-
- [Branch 2024.12.1](https://github.com/lubeda/EspHoMaTriXv2/tree/2024.12.1) for esphome 2024.12.0-...

For ehmtx use the newest version tag by manually editing the ref tag in the external_componets section in your yaml.
Only change your selected ehmtx version if there are compile errors. But expect manual changes to your yaml or even your automations.

The details to each EspHoMaTriX version are on the branch pages

```yaml
external_components:
  - source:
      #type: local
      #path: EsphoMaTrix/components # e.g. /config/esphome/components
      type: git
      url: https://github.com/lubeda/EspHoMaTriXv2
      ref: 2025.5.2
    refresh: 60s 
    components: [ ehmtxv2 ]   
```

### Features

Based on a 8x32 RGB matrix, it displays a clock, the date and up to 24 other 'screens' provided by Home Assistant. Each screen (value/text) can be associated with a 8x8 bit RGB icon or GIF animation (see [installation](#installation-of-esphomatrixv2-custom-component)). The values/text can be updated or deleted from the display queue. Each screen has a lifetime, if not refreshed in its lifetime, it will disappear. Even 8x32 GIF animations are possible. You can control nearly everything of the component.

### Advice

It is highly recommended to use an **ESP32 device**. There are conditions where the RAM size is too limited in a **ESP8266 device** so some of the features had to be removed for these boards (Example: bitmap_screen).

## EspHoMaTriX in the media

See this German tutorial video with information on setting up your display [RGB-LED Status Display für Home Assistant mit ESPHome | ESPHoMaTrix](https://www.youtube.com/watch?v=DTd9vAhet9A).

Another german tutorial video focused on the Ulanzi [Smarte Pixel Clock über Home Assistant steuern - Entitäten / Icons und mehr in der Ulanzi](https://www.youtube.com/watch?v=LgaT0mNbl34)

See this [nice article](https://blakadder.com/esphome-pixel-clock/) about EsphoMaTrix on a Ulanzi TC001 from [blakadder](https://github.com/blakadder).

Short video on Instagram [@blak_adder](https://www.insbuiltagram.com/reel/CpYVByRIaSI)

See these english discussions:
[Share your projects](https://community.home-assistant.io/t/esphomatrix-a-simple-clock-status-display/425325)
[ESPHOME](https://community.home-assistant.io/t/a-simple-diy-status-display-with-an-8x32-rgb-led/379051)

It was also mentioned in the blog [Building the Open Home](https://building.open-home.io/local-control-is-the-only-way/) and in the home assistant [livestream](https://youtu.be/IGnCGDaXR0M?t=6267)

Or in German:
[Showroom](https://community.simon42.com/t/8x32-pixel-uhr-mit-homeassistant-anbindung/1076)

## Disclaimer

THE SOFTWARE IS PROVIDED “AS IS”, use at your own risk!

## Thanks

See all the **coding heroes** in the newest branch!

- **[blakadder](https://github.com/blakadder)** for his contribution (cleanup README.md, fixed sample)
- **[dbuezas](https://github.com/dbuezas)** for his fonts [infos](https://github.com/lubeda/EspHoMaTriXv2/issues/63)
- **[jfurtner](https://github.com/jfurtner)** for his the on_start_running trigger and a new parameter
- **[andrew-codechimp](https://github.com/andrew-codechimp)** for his contribution (display on/off & del_screen "*" & show_clock with 0) and improved blueprint slelection
- **[jd1](https://github.com/jd1)** for his contributions
- **[aptonline](https://github.com/aptonline)** for his work on the Ulanzi hardware
- **[wsbtak](https://github.com/wsbtak)** for the work on the Ulanzi hardware
- **[ofirsnb](https://github.com/ofirsnb)** for his contributions
- **[trip5](https://github.com/trip5)** for his work on optimized fonts and user support and work on the wiki
- **[pplucky](https://user-images.githubusercontent.com/16407309/224850723-634c9b2d-55d9-44f2-9f93-765c0485b090.GIF)** for his 8x32 GIF animation
- **[dennisse](https://github.com/dennisse)** Auto brightness for the Ulanzi
- **[hco](https://github.com/hco)** fixing documentation
- **[geekofweek](https://github.com/geekofweek)** fixed sample YAML, cleanup documentation
- **[joncar](https://github.com/joncar)** fixed scroll small text
- **Everybody** who found bugs/issues and reported them!

## Special thanks to all the sponsors
