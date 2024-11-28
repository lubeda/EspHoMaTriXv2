# Introduction

**Skill-level:** intermediate

In the ehmtxv2: section of your yaml, you can change the [time date format](time-date-format).

Unfortunately, ESPHome does not seem capable of passing on localized time and date formats through compilation.

Using the replace function you can replace various elements of date and time formats completely which can help localize or customize your display.

## Date formats

Normal time and date function to get: `4:30PM` and `Oct 11`:

```yaml
  time_format: "%l:%M%p"
  date_format: "%b d%"
```

An example to get Korean date and time like `4:30오후` and `10월11일`:

```yaml
  time_format: "%p%l:%M"
  date_format: "%b%d일"
  replace_time_date_from: "AM PM Sun Mon Tue Wed Thu Fri Sat Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec"
  replace_time_date_to: "오전 오후 일 월 화 수 목 금 토 1월 2월 3월 4월 5월 6월 7월 8월 9월 10월 11월 12월"
```

Here is an example to get Ukrainian `Жовт 11`:

```yaml
  date_format: "%b %d"
  replace_time_date_from: "Sun Mon Tue Wed Thu Fri Sat Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec"
  replace_time_date_to: "ПН ВТ СР ЧТ ПТ СБ ВС Сiч Лют Бер Квiт Трав Черв Лип Серп Вер Жовт Лист Груд"
```

These are just a few examples.  You can replace almost any element you want, except spaces (this includes special Unicode spaces like thin spaces).
Yes, that means you can even replace numbers and the colon with other characters.  Use emojis, if you like!

Keep an eye on the ESPHome output when using this.  You should see a message if the replace is deactivated due to some error.
Note there is also a 50 pair limit.  Using too many pairs may cause slowdowns.  Although tested well with the Korean pair above, using more is considered experimental.

Please note also that input for `time_format` and `date_format` may be limited.  Experiment!

### Don't forget to include matching glyphs in your font section!
