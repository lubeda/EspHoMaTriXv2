# "night mode"

**Skill-level:** low

**minimum version:** 2023.8.0

```yaml
globals:
  - id: night_mode
    type: "bool"
    restore_value: true
    initial_value: "false"

time:
    .....
    on_time:
      # Every morning
      - seconds: 0
        minutes: 30
        hours: 7
        then:
          - lambda: |
              id(night_mode) = false;
      # Every morning
      - seconds: 0
        minutes: 30
        hours: 21
        then:
          - lambda: |-
            id(night_mode) = true;



ehmtx:
  on_empty_queue:
    then:
      lambda: |-
        if (id(night_mode)){
            id(rgb8x32)->clock_screen(10,10,true,140,120,120);
            id(rgb8x32)->date_screen(10,5,true,120,200,150);
        } else {
          id(rgb8x32)->blank_screen(1,15);
        }
```

