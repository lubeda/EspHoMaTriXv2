# modify Ulanzi TC-001

**Skill-level:** high

[see also here](https://github.com/lubeda/EsphoMaTrix/issues/113)

How to remove the battery and activate "always on"

See here: [reddit](https://www.reddit.com/r/homeassistant/comments/131okf7/comment/jp0e1tw/?utm_source=share&utm_medium=web2x&context=3)

**How much power does the device draw when it is on?**

1,5 Watt when idle online with display off
3,2 watt with high brightness white clock
6,4 watt with "light" function (all leds white at middle brightness)

**Short how to:**

@popy2k14: After reading the IC marking numbers and a little bit of measuring and googling, i found the FET (switch) for the main supply.
With the wire (shown in the picture) you just shorting source & drain of the FET (-> bridge the switch).
This isn't harmfull for the device and can easily reverted by just removing the wire.

**Use this hint at your own risk**

Good luck!