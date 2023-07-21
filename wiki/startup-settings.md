# Introduction

**Skill-level:** medium

## Set parameters on device boot

Since 2023.7.0 you can use a trigger with lambdas

```yaml
ehmtxv2:
  ...
  on_start_running:
    then:
      lambda: |-
        ESP_LOGD(TAG, "remove default clock and set new color");
        id(rgb8x32)->del_screen("*", 2);
        id(rgb8x32)->clock_screen(10,10,true,200,10,100);
```