# Optional LVGL

LVGL is prepared as an optional feature flag:

```sh
pio run -e cardputer_adv_lvgl
```

Before using it, verify:

- dependency version
- RAM usage
- display flush callback
- keyboard input mapping
- tick and `lv_timer_handler()` scheduling
