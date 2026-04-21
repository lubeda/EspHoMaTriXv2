7 and 9● Here are the optimizations found, grouped by impact:

  ---
  High Impact

  1. replace_time_date — O(n²) string replacement (EHMTX.cpp:1868–1908)

  For each replacement pair, it does std::search + erase + insert on the string body. Each   
  erase/insert is O(n), and this runs for every pair on every draw tick.
  Fix: Build the output in a single pass, or use std::regex_replace.

  2. Night mode filtering repeated 3× per tick (EHMTX.cpp:1288, 1338, 1476)

  find_oldest_queue_element, find_last_clock, and queue_count each loop through all 24 queue 
  entries and re-iterate EHMTXv2_CONF_NIGHT_MODE_SCREENS for every element on every call. All
   three are called from update() (250ms polling).
  Fix: Cache whether a given mode is night-mode-visible in a lookup array/bitfield at setup, 
  instead of re-scanning the vector each time.

  ---
  Medium Impact

  3. Strings passed by value instead of const std::string& (EHMTX.h)

  Several public methods take std::string by value, causing a copy on every service call:    
  - find_mode_icon_queue_element(uint8_t mode, std::string name) — line 121
  - draw_text(std::string text, ...) — line 435
  - replace_time_date(std::string time_date) — line 440

  Fix: Change to const std::string&.

  4. strcmp on .c_str() instead of == (EHMTX.cpp:926, 941)

  strcmp(this->icons[i]->name.c_str(), name.c_str())  // unnecessary
  this->icons[i]->name == name                         // correct

  5. Boot logo heap allocation without null-check (EHMTX.cpp:541–543)

  this->boot_logo_color = new Color[256];
  this->boot_logo_mask  = new uint8_t[256];
  Neither allocation is checked for nullptr before use. On low-RAM ESP8266, these can        
  silently fail and crash.

  6. Pixel-by-pixel image processing in __init__.py (lines 451–458)

  Nested putpixel() loops over 8×8 and 32×8 images. PIL's getdata()/putdata() or NumPy array 
  operations would be significantly faster at compile time (matters with large icon sets).   

  ---
  Low Impact / Code Quality

  7. split() called twice on same value (__init__.py:611, 615)

  len(config[CONF_REPLACE_TIME_DATE_FROM].split())  # line 611
  len(config[CONF_REPLACE_TIME_DATE_FROM].split())  # line 615 — identical
  Cache it once.

  8. Bare file I/O without context manager (__init__.py:418–420)

  f = open(path, "wb")
  f.write(r.content)
  f.close()           # not called if write() raises
  Fix: with open(path, "wb") as f: f.write(r.content)

  9. Bare except: clause (__init__.py:479)

  except:  # catches everything including KeyboardInterrupt
  Fix: except KeyError:

  10. HTML string building by concatenation (__init__.py:543–559)

  Repeated html_string += inside a loop. Accumulate into a list and "".join() at the end.    

  11. Global heap allocation never freed (EHMTX.cpp:13)

  uint8_t *m_heat = new (std::nothrow) uint8_t[m_heatSize];  // fire plugin, 256 bytes       
  Never deallocated. Fine in practice on embedded (no process exit), but should be static    
  uint8_t m_heat[m_heatSize] to avoid heap fragmentation.

  12. Magic constant 9 in xpos (EHMTX_queue.cpp:287)

  result = width - this->config_->scroll_step + item * 9;
  The 9 (icon width + 1px gap) is unexplained. Should be a named constant.