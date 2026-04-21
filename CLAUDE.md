# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What This Project Is

EspHoMaTriXv2 is an ESPHome custom component for an 8×32 RGB LED matrix display (e.g., Ulanzi TC001 pixel clock). It integrates with Home Assistant via service calls and supports 27+ display modes (clock, date, icons, animations, progress bars, graphs, fire effects, etc.).

## Build and Test Commands

No unit tests exist. Validation is done by building ESPHome firmware from the test configurations in `tests/`.

```bash
# Build a specific test configuration
esphome build tests/esp32-idf.yaml

# Build and flash to device
esphome run tests/esp32-idf.yaml

# Build all test configs (mirrors CI)
for f in tests/*.yaml; do
  [[ "$f" == *"secrets"* ]] && continue
  esphome build "$f" || exit 1
done
```

Test YAML files: `esp32-arduino.yaml`, `esp32-idf.yaml`, `esp32-idf-features.yaml`, `esp32-idf-noapi.yaml`, `esp8266-arduino.yaml`.

## Architecture

All component code lives in `components/ehmtxv2/`:

- **`__init__.py`** — ESPHome Python config module: schema validation, code generation, icon downloading/processing at compile time. Defines all `CONF_*` constants and registers automation triggers.
- **`EHMTX.h`** — Main class header. Defines the `show_mode` enum (27 modes), constants (`MAXQUEUE=24`, `POLLINGINTERVAL=250ms`), all trigger classes, and the `EHMTX` class (extends `PollingComponent`, optionally `CustomAPIDevice` when `USE_API` is defined).
- **`EHMTX.cpp`** — Core implementation: display rendering, color effects, brightness, night mode, icon/text drawing, all public service methods.
- **`EHMTX_queue.cpp`** — Queue management: screen lifecycle, fire animation plugin (black body radiation spectrum), screen transitions, blend effects.
- **`EHMTX_icons.cpp`** — `EHMTX_Icon` class extending `esphome::Animation`; handles pingpong frame direction.

### Key Design Patterns

- `EHMTX` is a `PollingComponent`; `update()` fires every 250ms and drives the display tick.
- Screen queue holds up to 24 `EHMTX_queue` entries, each with a mode, icon, text, lifetime, and screen_time.
- Compile-time icon processing: `__init__.py` downloads URLs, resizes images to 8×8 or 8×32, and encodes them as RGB565 arrays baked into firmware.
- API availability is conditional: `#ifdef USE_API` wraps all Home Assistant service registrations.

### Adding a New Feature

1. Add `CONF_*` constant and schema entry in `__init__.py` (`cv.Optional` with default).
2. Add member variable to `EHMTX` class in `EHMTX.h`, initialize in constructor.
3. Implement logic in the appropriate `.cpp` file.
4. Add public service method if Home Assistant-callable.
5. Update `README.md` for any user-facing changes.

## Versioning

CalVer format: `YYYY.MM.D` (e.g., `2026.4.1`). Version string is defined in `EHMTX.h`:
```cpp
static const char *const EHMTX_VERSION = "2026.4.1";
```
Update this when making breaking changes or significant new features.

## Code Style

**C++ (2-space indent, 120-char line limit):**
- Classes: `PascalCase` — Methods/variables: `snake_case` — Member variables: trailing underscore (`brightness_`) — Constants/enums: `UPPER_SNAKE_CASE`
- ESPHome types: `uint8_t`/`uint16_t`, `Color`, `esphome::display::BaseFont*`
- Logging: `ESP_LOGD/I/W/E(TAG, ...)` with `static const char *const TAG = "EHMTXv2";`

**Python (`__init__.py`, 4-space indent, 88-char line limit):**
- `cv.` for config validation, `cg.` for code generation
- Config values via `config[CONF_KEY]`
- Register triggers with `automation.build_automation()`
