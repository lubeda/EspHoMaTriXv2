# AGENTS.md - Guidelines for EspHoMaTriXv2 Development

## 1. Build, Lint, and Test Commands

### Building Firmware
The project uses ESPHome for building firmware. Test configurations are in the `tests/` directory.

#### Build a specific test
```bash
esphome build tests/<test-name>.yaml
```
Example:
```bash
esphome build tests/esp32-idf.yaml
```

#### Build and upload to device
```bash
esphome run tests/<test-name>.yaml
```

#### Clean build artifacts
```bash
esphome clean tests/<test-name>.yaml
```

### Linting
There are no automated linters configured in the repository. However, follow these guidelines:

#### YAML Files
- Use 2 spaces for indentation (no tabs)
- Keep line length under 120 characters
- Use quoted strings for special values (e.g., `"true"`, `"123"`)
- Validate YAML syntax with online tools or `yamllint` if available locally

#### C++ Files (components/)
- Follow the existing code style (see section 2 below)
- No automatic formatter configured; maintain consistency with surrounding code

### Running Tests
This repository does not contain unit tests. Validation is done by:
1. Building each test configuration in the CI (see `.github/workflows/main.yml`)
2. Manual verification on hardware

To verify a change builds successfully for all test configurations:
```bash
# From the repository root
for f in tests/*.yaml; do
  if [[ "$f" != *"secrets"* ]]; then
    echo "Building $f"
    esphome build "$f" || exit 1
  fi
done
```

## 2. Code Style Guidelines

### File Organization
- Components live in `components/ehmtxv2/`
- Header files: `.h`
- Implementation files: `.cpp`
- Python configuration: `__init__.py`

### C++ Style (components/ehmtxv2/)

#### Formatting
- Indentation: 2 spaces (no tabs)
- Line length: Aim for <= 120 characters
- Braces: 
  - Opening brace on same line for functions, classes, namespaces
  - Opening brace on new line for structs/enums (follow existing usage)
- Pointer/Reference: `Type*` and `Type&` (no spaces around `*`/`&`)
- Spaces: 
  - After commas in function calls and parameters
  - Around binary operators (`+`, `-`, `==`, etc.)
  - No spaces after `(` or before `)`

#### Imports/Includes
- Order: 
  1. Standard library (`<vector>`, `<string>`, etc.)
  2. ESPHome core (`esphome.h`)
  3. ESPHome components (`esphome/components/...`)
  4. Other ESPHome components (if any)
  5. Local component headers (`EHMTX.h`, `EHMTX_queue.h`, etc.)
- Each import on its own line
- Use `#ifndef` header guards with `_H` suffix

#### Types
- Use ESPHome types when available:
  - `uint8_t`, `uint16_t`, `uint32_t` for integers
  - `bool` for booleans
  - `std::string` for strings
  - `Color` for RGB colors (defined in esphome.h)
  - `esphome::display::BaseFont*` for fonts
- Avoid raw pointers when possible; use ESPHome's smart pointers for components
- For arrays: prefer `std::array` or `std::vector` over raw arrays when size is variable

#### Naming Conventions
- Classes: `PascalCase` (e.g., `EHMTX`, `EHMTX_queue`)
- Functions and methods: `snake_case` (e.g., `setup()`, `draw_text()`)
- Member variables: `snake_case` with trailing underscore (e.g., `brightness_`, `screen_pointer_`)
- Constants: `UPPER_SNAKE_CASE` (e.g., `MAXQUEUE`, `POLINGINTERVAL`)
- Enums: `PascalCase` for enum name, `UPPER_SNAKE_CASE` for values (e.g., `show_mode::MODE_CLOCK`)
- Namespaces: `lowercase` (e.g., `esphome`)

#### Error Handling
- Use ESPHome's logging macros:
  - `ESP_LOGD(tag, format, ...)` for debug
  - `ESP_LOGI(tag, format, ...)` for info
  - `ESP_LOGW(tag, format, ...)` for warnings
  - `ESP_LOGE(tag, format, ...)` for errors
  - `ESP_LOGXX_FORCE_TAG(tag, ...)` to force tag
- Define `static const char *const TAG = "EHMTXv2";` at top of files
- For recoverable errors, log and return safe defaults
- For unrecoverable errors in setup, consider `ESP_LOGE` and halt execution
- Validate inputs in public methods (e.g., check for null pointers, valid ranges)

### Python Style (__init__.py)

#### Formatting
- Indentation: 4 spaces (standard Python)
- Line length: Aim for <= 88 characters (Black style)
- Imports: 
  - Standard library first
  - Third-party (PIL, requests) second
  - ESPHome modules last
  - Each import grouped and sorted alphabetically within group
- Use `from esphome import ...` for ESPHome imports

#### Naming
- Variables and functions: `snake_case`
- Constants: `UPPER_SNAKE_CASE`
- Classes: `PascalCase`

#### Error Handling
- Use `try`/`except` for specific exceptions:
  ```python
  try:
      # operation
  except SpecificError as e:
      raise core.EsphomeError(f"Context: {e}")
  ```
- Avoid bare `except:` clauses
- Log warnings with `logging.warning()` for recoverable issues
- Use `logging.info()` for non-error status updates

#### ESPHome-Specific
- Use `cv.` for config validation (from `esphome.config_validation`)
- Use `cg.` for code generation (from `esphome.codegen`)
- Access config values via `config[CONF_KEY]`
- Use `CORE.relative_config_path(path)` for file paths
- Register triggers with `automation.build_automation()`

## 3. Repository-Specific Practices

### Configuration
- All user-facing options use `CONF_*` constants defined in `__init__.py`
- Add new options to both the schema and the `to_code` function
- Use `cv.Optional` with sensible defaults for new features
- Validate ranges with `cv.int_range(min=X, max=Y)`
- Use `cv.templatable` for options that should support lambdas

### Component Lifecycle
- `setup()`: Called once at startup; initialize hardware and state
- `dump_config()`: Optional; logs configuration to ESPHome logs
- Loop methods: 
  - `update()`: Called at `get_setup_priority()` interval (set via `PollingComponent`)
  - `tick()`: High-frequency update (if needed)
- Ensure all dynamically allocated memory is freed (though ESPHome usually handles this)

### Hardware Abstraction
- Use ESPHome's `display::` and `light::` APIs for hardware access
- Avoid direct register access; use ESPHome wrappers
- For timing, use `millis()` or `micros()` from Arduino core
- Use `yield()` in long-running operations to avoid watchdog triggers

### Adding Features
1. Add configuration option to `__init__.py` schema
2. Add member variables to `EHMTX` class in `EHMTX.h`
3. Initialize in constructor
4. Handle in `update()`/`tick()`/`draw()` methods as appropriate
5. Add public API method if needed for services/triggers
6. Update documentation in README.md if user-facing

### Versioning
- Version defined in `EHMTX.h`: `static const char *const EHMTX_VERSION = "YYYY.MM.D";`
- Update when making breaking changes or significant features
- Follow CalVer (Year.Month.Day) or similar incremental scheme
