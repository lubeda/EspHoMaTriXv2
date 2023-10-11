# Introduction

**Skill-level:** low

In the ehmtxv2: section of your yaml, you can change the format that the date and time are displayed.

There are some helpful hints on the [EspHome site](https://esphome.io/components/time/?highlight=strftime#strftime) but there are some undocumented ones, too. EspHome only uses a subset of [strftime](https://strftime.org/) but there are still some good tricks available.

## Date formats

Month.Day (02.28)

```yaml
  date_format: "%m.%d"
```

Day.Month (28.02)

```yaml
  date_format: "%d.%m"
```

Year.Month.Day (23.02.28)

```yaml
  date_format: "%y.%m.%d"
```

Abbreviated Month and Date (Feb 08)

```yaml
  date_format: "%b %d"
```

Abbreviated Day and Date (Tue 08)

```yaml
  date_format: "%a %d"
```

## Time formats

24-hour time (23:15)

```yaml
  time_format: "%H:%M"
```

12-hour time with padded hour (01:25PM)

```yaml
  time_format: "%I:%M%p"
```

12-hour time with unpadded hour (1:25PM)

```yaml
  time_format: "%l:%M%p"
```
## Localized Time and Date

Unfortunately, ESPHome does not seem capable of passing on localized time and date formats so that you can get time formats in your own language easily.

But you can still do it with a special function: [time-date-replace](time-date-replace).