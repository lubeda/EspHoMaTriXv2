# Introduction

**Skill-level:** medium

If you have got this error `Guru Meditation Error: Core 1 panic'ed (StoreProhibited). Exception was unhandled.`

Try to reorder your components in the yaml file, this order works!

```yaml
esphome:
....
light:
...
display:
...
ehmtxv2:
...
```