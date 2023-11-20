# Own service calls

**Skill-level:** all

Up to esphome 2023.10.2  you colud do this:

The solution:

```yaml
api:
  services:
    - service: alarm
      variables:
        icon_name: string
        text: string
      then:
        lambda: |-
          id(rgb8x32)->icon_screen(icon_name,text,7,20,true);
          id(rgb8x32)->force_screen(icon_name);
```

But sinde esphome 2023.11.0 `text`is a reserved word you have to use this:

The solution:

```yaml
api:
  services:
    - service: alarm
      variables:
        icon_name: string
        message: string
      then:
        lambda: |-
          id(rgb8x32)->icon_screen(icon_name,message,7,20,true);
          id(rgb8x32)->force_screen(icon_name);
```

**Just dont use `text` as variable name!**
