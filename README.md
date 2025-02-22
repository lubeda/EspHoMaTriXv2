# EspHoMaTriX version 2 (EHMTXv2)

[donation-badge]:https://img.shields.io/badge/PayPal-00457C?style=for-the-badge&logo=paypal&logoColor=white
[donation-url]: https://www.paypal.com/donate/?hosted_button_id=FZDKSLQ46HJTU

![Home Assistant](https://img.shields.io/badge/home%20assistant-%2341BDF5.svg?style=for-the-badge&logo=home-assistant&logoColor=white)
![GitHub](https://img.shields.io/badge/github-%23121011.svg?style=for-the-badge&logo=github&logoColor=white)
[![Donate][donation-badge]][donation-url]
[![Publish wiki](https://github.com/lubeda/EspHoMaTriXv2/actions/workflows/wiki.yaml/badge.svg)](https://github.com/lubeda/EspHoMaTriXv2/actions/workflows/wiki.yaml)
[![Build](https://github.com/lubeda/EspHoMaTriXv2/actions/workflows/main.yml/badge.svg)](https://github.com/lubeda/EspHoMaTriXv2/actions/workflows/main.yml)

> [!TIP]
> If you like this project, please donate a star on GitHub and consider [sponsoring](https://www.paypal.com/donate/?hosted_button_id=FZDKSLQ46HJTU) me 🙏 !

# Don't use "latest"

**Always** use the latest version **according to your esphome version** of this tool and use it directly from the repo, like this:

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/lubeda/EspHoMaTriXv2
      ref: 2025.2.0
    refresh: 60s 
    components: [ ehmtxv2 ]   
```

**But expect breaking changes whn you change to a newer version!!!!!**