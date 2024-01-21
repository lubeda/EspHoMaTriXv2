#include "esphome.h"
#include <sstream>
#include <vector>
#include <algorithm>
#include <string>
#include <regex>

// #define USE_ESP32
// #define EHMTXv2_ADV_BOOT
// #define EHMTXv2_ADV_BOOT_MODE_0

#ifdef USE_ESP32
  #ifdef EHMTXv2_ADV_BOOT
    #if defined EHMTXv2_ADV_BOOT_MODE_6 || defined EHMTXv2_ADV_BOOT_MODE_7
      #define CHECK_BIT(var, pos) (var & (1 << pos))
    #endif
  #endif
#endif

namespace esphome
{
  EHMTX::EHMTX() : PollingComponent(POLLINGINTERVAL)
  {
    ESP_LOGD(TAG, "Constructor start");
    this->show_display = true;
    this->display_gauge = false;
    this->display_rindicator = 0;
    this->display_lindicator = 0;
    this->display_icon_indicator = 0;
    this->icon_indicator_y_pos = 7;
    this->icon_indicator_height = 1;
    this->icon_to_9 = 0;
    this->display_alarm = 0;
    this->clock_time = 10;
    this->icon_count = 0;
    this->hue_ = 180;
    this->rainbow_color = Color(CA_RED, CA_GREEN, CA_BLUE);
    this->info_lcolor = Color(CG_GREY, CG_GREY, CG_GREY);
    this->info_rcolor = Color(CG_GREY * 2, CG_GREY * 2, CG_GREY * 2);
    this->solid_color = Color(C_RED, C_GREEN, C_BLUE);
    this->calendar_color = Color(C_RED, C_BLACK, C_BLACK);
    this->next_action_time = 0.0;
    this->last_scroll_time = 0;
    this->screen_pointer = MAXQUEUE;
    this->is_running = false;
    this->set_today_color();
    this->set_weekday_color();
    this->night_mode = false;
    this->weekday_accent = false;

#ifdef EHMTXv2_USE_VERTICAL_SCROLL
    this->vertical_scroll = false;
#endif

#ifdef USE_ESP32
  #ifdef EHMTXv2_ADV_BOOT
    this->boot_logo = nullptr;
  #endif
#endif

    for (uint8_t i = 0; i < MAXQUEUE; i++)
    {
      this->queue[i] = new EHMTX_queue(this);
    }

#ifdef EHMTXv2_ADV_CLOCK
    this->info_clock_lcolor = Color(CG_GREY, CG_GREY, CG_GREY);
    this->info_clock_rcolor = Color(CG_GREY * 2, CG_GREY * 2, CG_GREY * 2);

    this->set_clock_color();
    this->set_adv_clock_color();
#endif

    ESP_LOGD(TAG, "Constructor finish");
  }

  void EHMTX::show_rindicator(int r, int g, int b, int size)
  {
    if (size > 0)
    {
      this->rindicator_color = Color((uint8_t)r, (uint8_t)g, (uint8_t)b);
      this->display_rindicator = size & 3;
      ESP_LOGD(TAG, "show rindicator size: %d r: %d g: %d b: %d", size, r, g, b);
    }
    else
    {
      this->hide_rindicator();
    }
  }

  void EHMTX::show_lindicator(int r, int g, int b, int size)
  {
    if (size > 0)
    {
      this->lindicator_color = Color((uint8_t)r, (uint8_t)g, (uint8_t)b);
      this->display_lindicator = size & 3;
      ESP_LOGD(TAG, "show lindicator size: %d r: %d g: %d b: %d", size, r, g, b);
    }
    else
    {
      this->hide_lindicator();
    }
  }

  void EHMTX::show_icon_indicator(int r, int g, int b, int size, int pos, int height)
  {
    if (size > 0)
    {
      this->icon_indicator_color = Color((uint8_t)r, (uint8_t)g, (uint8_t)b);
      this->display_icon_indicator = size;
      this->icon_indicator_height = height;
      this->icon_indicator_y_pos = pos;
      ESP_LOGD(TAG, "show icon_indicator size: %d height: %d r: %d g: %d b: %d pos: %d", size, height, r, g, b, pos);
    }
    else
    {
      this->hide_icon_indicator();
    }
  }

  void EHMTX::hide_rindicator()
  {
    this->display_rindicator = 0;
    ESP_LOGD(TAG, "hide rindicator");
  }

  void EHMTX::hide_lindicator()
  {
    this->display_lindicator = 0;
    ESP_LOGD(TAG, "hide lindicator");
  }

  void EHMTX::hide_icon_indicator()
  {
    this->display_icon_indicator = 0;
    ESP_LOGD(TAG, "hide icon indicator");
  }

  void EHMTX::set_display_off()
  {
    this->show_display = false;
    ESP_LOGD(TAG, "display off");
    display->clear();

    for (auto *t : on_show_display_triggers_)
    {
      t->process(this->show_display);
    }
  }

  void EHMTX::set_display_on()
  {
    this->show_display = true;
    ESP_LOGD(TAG, "display on");

    for (auto *t : on_show_display_triggers_)
    {
      t->process(this->show_display);
    }
  }

  void EHMTX::set_night_mode_off()
  {
    this->night_mode = false;
    ESP_LOGD(TAG, "night mode off");

    for (auto *t : on_night_mode_triggers_)
    {
      t->process(this->night_mode);
    }
  }

  void EHMTX::set_night_mode_on()
  {
    this->night_mode = true;
    ESP_LOGD(TAG, "night mode on");

    for (auto *t : on_night_mode_triggers_)
    {
      t->process(this->night_mode);
    }
  }

  void EHMTX::set_weekday_accent_off()
  {
    this->weekday_accent = false;
    ESP_LOGD(TAG, "weekday accent off");
  }

  void EHMTX::set_weekday_accent_on()
  {
    this->weekday_accent = true;
    ESP_LOGD(TAG, "weekday accent on");
  }

  void EHMTX::set_today_color(int r, int g, int b)
  {
    this->today_color = Color((uint8_t)r, (uint8_t)g, (uint8_t)b);
    ESP_LOGD(TAG, "default today color r: %d g: %d b: %d", r, g, b);
  }

  void EHMTX::set_weekday_color(int r, int g, int b)
  {
    this->weekday_color = Color((uint8_t)r, (uint8_t)g, (uint8_t)b);
    ESP_LOGD(TAG, "default weekday color: %d g: %d b: %d", r, g, b);
  }

  void EHMTX::expand_icon_to_9(int mode)
  {
    this->icon_to_9 = mode;
    ESP_LOGD(TAG, "icon expanded to 9 mode: %d", mode);
  }

  bool EHMTX::string_has_ending(std::string const &fullString, std::string const &ending)
  {
    if (fullString.length() >= ending.length())
    {
      return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
    }
    else
    {
      return false;
    }
  }

  std::string get_icon_name(std::string iconname, char delim = '|')
  {
    std::stringstream stream(iconname);
    std::string icon;
    std::vector<std::string> tokens;

    while (std::getline(stream, icon, delim))
    {
      if (!icon.empty())
      {
        tokens.push_back(icon);
      }
    }

    return (tokens.size() > 0) ? tokens[0] : "";
  }

  std::string get_screen_id(std::string iconname, char delim = '|')
  {
    std::stringstream stream(iconname);
    std::string screen_id;
    std::vector<std::string> tokens;

    while (std::getline(stream, screen_id, delim))
    {
      if (!screen_id.empty())
      {
        tokens.push_back(screen_id);
      }
    }

    return (tokens.size() > 1) ? tokens[1] : (tokens.size() > 0) ? (iconname.find("*") != std::string::npos) ? get_icon_name(tokens[0], '_') : tokens[0]
                                                                 : "";
  }

#ifdef USE_ESP32
  #ifdef EHMTXv2_ADV_BOOT
  void EHMTX::set_boot_logo(std::string logo)
  {
    if (logo == "")
    {
      delete[] this->boot_logo;
      this->boot_logo = nullptr;
      return;
    }

    if (this->boot_logo == NULL)
    {
    #if defined EHMTXv2_ADV_BOOT_MODE_0 || defined EHMTXv2_ADV_BOOT_MODE_1
      this->boot_logo = new Color[256];
    #else
      this->boot_logo = new uint8_t[256];
    #endif
    }

    const size_t CAPACITY = JSON_ARRAY_SIZE(256);
    StaticJsonDocument<CAPACITY> doc;
    deserializeJson(doc, logo);
    JsonArray array = doc.as<JsonArray>();
    // extract the values
    uint16_t i = 0;
    for (JsonVariant v : array)
    {
      uint16_t buf = v.as<int>();

    #if defined EHMTXv2_ADV_BOOT_MODE_0 || defined EHMTXv2_ADV_BOOT_MODE_1
      unsigned char b = (((buf) & 0x001F) << 3);
      unsigned char g = (((buf) & 0x07E0) >> 3); // Fixed: shift >> 5 and << 2
      unsigned char r = (((buf) & 0xF800) >> 8); // shift >> 11 and << 3
      this->boot_logo[i++] = Color(r, g, b);
    #elif defined EHMTXv2_ADV_BOOT_MODE_6 || defined EHMTXv2_ADV_BOOT_MODE_7
      this->boot_logo[i++] = (buf > 255) ? 255 : buf;
    #else
      this->boot_logo[i++] = (buf == 0) ? 0 : 1;
    #endif
    }
  }
  #endif
#endif

#ifndef USE_ESP8266
  void EHMTX::bitmap_screen(std::string text, int lifetime, int screen_time)
  {
    std::string ic = get_icon_name(text);
    std::string id = "";

    if (text.find("|") != std::string::npos)
    {
      id = get_screen_id(text);
    }

#ifdef EHMTXv2_ADV_BITMAP
    EHMTX_queue *screen = this->find_mode_icon_queue_element(MODE_BITMAP_SCREEN, id);
#else
    EHMTX_queue *screen = this->find_mode_queue_element(MODE_BITMAP_SCREEN);
#endif

#ifdef EHMTXv2_ADV_BITMAP
    if (screen->bitmap == NULL)
    {
      screen->bitmap = new Color[256];
    }
#endif

    const size_t CAPACITY = JSON_ARRAY_SIZE(256);
    StaticJsonDocument<CAPACITY> doc;
    deserializeJson(doc, ic);
    JsonArray array = doc.as<JsonArray>();
    // extract the values
    uint16_t i = 0;
    for (JsonVariant v : array)
    {
      uint16_t buf = v.as<int>();

      unsigned char b = (((buf) & 0x001F) << 3);
      unsigned char g = (((buf) & 0x07E0) >> 3); // Fixed: shift >> 5 and << 2
      unsigned char r = (((buf) & 0xF800) >> 8); // shift >> 11 and << 3
      Color c = Color(r, g, b);

#ifdef EHMTXv2_ADV_BITMAP
      screen->bitmap[i++] = c;
#else
      this->bitmap[i++] = c;
#endif
    }

    screen->icon_name = id;
    screen->text = "";
    screen->mode = MODE_BITMAP_SCREEN;
    screen->screen_time_ = screen_time * 1000.0;
    screen->endtime = this->get_tick() + (lifetime > 0 ? lifetime * 60000.0 : screen->screen_time_);
#ifdef EHMTXv2_USE_VERTICAL_SCROLL
    screen->pixels_ = 0;
    screen->scroll_reset = 32;
#endif
    if (id == "")
    {
      for (auto *t : on_add_screen_triggers_)
      {
        t->process("bitmap", (uint8_t)screen->mode);
      }
    }
    else
    {
      for (auto *t : on_add_screen_triggers_)
      {
        t->process("bitmap: " + id, (uint8_t)screen->mode);
      }
    }
    ESP_LOGD(TAG, "bitmap screen: lifetime: %d screen_time: %d", lifetime, screen_time);
    screen->status();
  }

  void EHMTX::bitmap_small(std::string icon, std::string text, int lifetime, int screen_time, bool default_font, int r, int g, int b)
  {
    std::string ic = get_icon_name(icon);
    std::string id = "";

    if (icon.find("|") != std::string::npos)
    {
      id = get_screen_id(icon);
    }

    EHMTX_queue *screen = this->find_mode_icon_queue_element(MODE_BITMAP_SMALL, id);

    screen->text = text;
    screen->icon_name = id;
    screen->text_color = Color(r, g, b);
    screen->mode = MODE_BITMAP_SMALL;
    screen->default_font = default_font;
    screen->calc_scroll_time(text, screen_time);
    screen->endtime = this->get_tick() + (lifetime > 0 ? lifetime * 60000.0 : screen->screen_time_);

    if (screen->sbitmap == NULL)
    {
      screen->sbitmap = new Color[64];
    }

    const size_t CAPACITY = JSON_ARRAY_SIZE(64);
    StaticJsonDocument<CAPACITY> doc;
    deserializeJson(doc, ic);
    JsonArray array = doc.as<JsonArray>();
    // extract the values
    uint16_t i = 0;
    for (JsonVariant v : array)
    {
      uint16_t buf = v.as<int>();

      unsigned char b = (((buf) & 0x001F) << 3);
      unsigned char g = (((buf) & 0x07E0) >> 3); // Fixed: shift >> 5 and << 2
      unsigned char r = (((buf) & 0xF800) >> 8); // shift >> 11 and << 3
      Color c = Color(r, g, b);

      screen->sbitmap[i++] = c;
    }

    if (id == "")
    {
      for (auto *t : on_add_screen_triggers_)
      {
        t->process("bitmap small", (uint8_t)screen->mode);
      }
      ESP_LOGD(TAG, "small bitmap screen: text: %s lifetime: %d screen_time: %d", text.c_str(), lifetime, screen_time);
    }
    else
    {
      for (auto *t : on_add_screen_triggers_)
      {
        t->process("bitmap small: " + id, (uint8_t)screen->mode);
      }
      ESP_LOGD(TAG, "small bitmap screen: id: %s text: %s lifetime: %d screen_time: %d", id.c_str(), text.c_str(), lifetime, screen_time);
    }
    screen->status();
  }

  void EHMTX::rainbow_bitmap_small(std::string icon, std::string text, int lifetime, int screen_time, bool default_font)
  {
    std::string ic = get_icon_name(icon);
    std::string id = "";

    if (icon.find("|") != std::string::npos)
    {
      id = get_screen_id(icon);
    }

    EHMTX_queue *screen = this->find_mode_icon_queue_element(MODE_RAINBOW_BITMAP_SMALL, id);

    screen->text = text;
    screen->icon_name = id;
    screen->mode = MODE_RAINBOW_BITMAP_SMALL;
    screen->default_font = default_font;
    screen->calc_scroll_time(text, screen_time);
    screen->endtime = this->get_tick() + (lifetime > 0 ? lifetime * 60000.0 : screen->screen_time_);

    if (screen->sbitmap == NULL)
    {
      screen->sbitmap = new Color[64];
    }

    const size_t CAPACITY = JSON_ARRAY_SIZE(64);
    StaticJsonDocument<CAPACITY> doc;
    deserializeJson(doc, ic);
    JsonArray array = doc.as<JsonArray>();
    // extract the values
    uint16_t i = 0;
    for (JsonVariant v : array)
    {
      uint16_t buf = v.as<int>();

      unsigned char b = (((buf) & 0x001F) << 3);
      unsigned char g = (((buf) & 0x07E0) >> 3); // Fixed: shift >> 5 and << 2
      unsigned char r = (((buf) & 0xF800) >> 8); // shift >> 11 and << 3
      Color c = Color(r, g, b);

      screen->sbitmap[i++] = c;
    }

    if (id == "")
    {
      for (auto *t : on_add_screen_triggers_)
      {
        t->process("bitmap small", (uint8_t)screen->mode);
      }
      ESP_LOGD(TAG, "small bitmap rainbow screen: text: %s lifetime: %d screen_time: %d", text.c_str(), lifetime, screen_time);
    }
    else
    {
      for (auto *t : on_add_screen_triggers_)
      {
        t->process("bitmap small: " + id, (uint8_t)screen->mode);
      }
      ESP_LOGD(TAG, "small bitmap rainbow screen: id: %s text: %s lifetime: %d screen_time: %d", id.c_str(), text.c_str(), lifetime, screen_time);
    }
    screen->status();
  }

  std::string trim(std::string const &s)
  {
    auto const first{s.find_first_not_of(' ')};
    if (first == std::string::npos)
      return {};
    auto const last{s.find_last_not_of(' ')};
    return s.substr(first, (last - first + 1));
  }

  void EHMTX::bitmap_stack(std::string icons, int lifetime, int screen_time)
  {
    icons.erase(remove(icons.begin(), icons.end(), ' '), icons.end());

    std::string ic = get_icon_name(icons);
    std::string id = "";

    if (icons.find("|") != std::string::npos)
    {
      id = get_screen_id(icons);
    }

    std::stringstream stream(ic);
    std::string icon;
    std::vector<std::string> tokens;
    uint8_t count = 0;

    while (std::getline(stream, icon, ','))
    {
      icon = trim(icon);
      if (!icon.empty())
      {
        tokens.push_back(icon);
        count++;
      }
      if (count >= 64)
      {
        break;
      }
    }
    if (count == 0)
    {
      ESP_LOGW(TAG, "bitmap stack: icons list [%s] empty", ic.c_str());
      return;
    }

    EHMTX_queue *screen = this->find_mode_queue_element(MODE_BITMAP_STACK_SCREEN);
    if (screen->sbitmap == NULL)
    {
      screen->sbitmap = new Color[64];
    }

    uint8_t real_count = 0;
    for (uint8_t i = 0; i < count; i++)
    {
      uint8_t icon = this->find_icon(tokens[i].c_str());

      if (icon == MAXICONS)
      {
        ESP_LOGW(TAG, "icon %d/%s not found => skip", icon, tokens[i].c_str());
        for (auto *t : on_icon_error_triggers_)
        {
          t->process(tokens[i]);
        }
      }
      else
      {
        screen->sbitmap[real_count] = Color(127, 255, icon, 5); // int16_t 32767 = uint8_t(127,255)
        real_count++;
      }
    }
    if (real_count == 0)
    {
      delete[] screen->sbitmap;
      screen->sbitmap = nullptr;

      ESP_LOGW(TAG, "bitmap stack: icons list [%s] does not contain any known icons.", ic.c_str());
      return;
    }

    screen->icon = real_count;
    screen->mode = MODE_BITMAP_STACK_SCREEN;
    screen->icon_name = id;
    screen->text = ic;
    screen->progress = (id == "two") ? 1 : 0; // 0 - one side scroll (right to left), 1 - two side (outside to center) if supported
    screen->default_font = false;
    screen->calc_scroll_time(screen->icon, screen_time);
    screen->endtime = this->get_tick() + (lifetime > 0 ? lifetime * 60000.0 : screen->screen_time_);
    for (auto *t : on_add_screen_triggers_)
    {
      t->process(screen->text, (uint8_t)screen->mode);
    }
    ESP_LOGD(TAG, "bitmap stack: has %d icons from: [%s] screen_time: %d", screen->icon, icons.c_str(), screen_time);
    screen->status();
  }
#endif

#ifdef USE_ESP8266
  void EHMTX::bitmap_screen(std::string text, int lifetime, int screen_time)
  {
    ESP_LOGW(TAG, "bitmap_screen is not available on ESP8266");
  }
  void EHMTX::bitmap_small(std::string i, std::string t, int l, int s, bool f, int r, int g, int b)
  {
    ESP_LOGW(TAG, "bitmap_small is not available on ESP8266");
  }
  void EHMTX::rainbow_bitmap_small(std::string i, std::string t, int l, int s, bool f)
  {
    ESP_LOGW(TAG, "bitmap_small_rainbow is not available on ESP8266");
  }
  void EHMTX::bitmap_stack(std::string i, int l, int s)
  {
    ESP_LOGW(TAG, "bitmap_stack is not available on ESP8266");
  }
#endif

  uint8_t EHMTX::find_icon(std::string name)
  {
    if (name == "blank")
    {
      return BLANKICON;
    }

    if (name == "solid")
    {
      return SOLIDICON;
    }

    if (name == "calendar")
    {
      return CALENDARICON;
    }

    for (uint8_t i = 0; i < this->icon_count; i++)
    {
      if (strcmp(this->icons[i]->name.c_str(), name.c_str()) == 0)
      {
        ESP_LOGD(TAG, "icon: %s found id: %d", name.c_str(), i);
        return i;
      }
    }
    ESP_LOGW(TAG, "icon: %s not found", name.c_str());

    return MAXICONS;
  }

  uint8_t EHMTX::find_icon_in_queue(std::string name)
  {
    for (uint8_t i = 0; i < MAXQUEUE; i++)
    {
      if (strcmp(this->queue[i]->icon_name.c_str(), name.c_str()) == 0)
      {
        ESP_LOGD(TAG, "find icon in queue: icon: %s at position %d", name.c_str(), i);
        return i;
      }
    }
    ESP_LOGW(TAG, "find icon in queue: icon: %s not found", name.c_str());
    return MAXICONS;
  }

  void EHMTX::hide_gauge()
  {
    this->display_gauge = false;
    ESP_LOGD(TAG, "hide gauge");
  }

#ifndef USE_ESP8266
  void EHMTX::color_gauge(std::string text)
  {
    ESP_LOGD(TAG, "color_gauge: %s", text.c_str());
    const size_t CAPACITY = JSON_ARRAY_SIZE(8);
    StaticJsonDocument<CAPACITY> doc;
    deserializeJson(doc, text);
    JsonArray array = doc.as<JsonArray>();
    uint8_t i = 0;
    for (JsonVariant v : array)
    {
      uint16_t buf = v.as<int>();

      unsigned char b = (((buf) & 0x001F) << 3);
      unsigned char g = (((buf) & 0x07E0) >> 3); // Fixed: shift >> 5 and << 2
      unsigned char r = (((buf) & 0xF800) >> 8); // shift >> 11 and << 3
      Color c = Color(r, g, b);
      this->cgauge[i++] = c;
      this->display_gauge = true;
    }
  }

  void EHMTX::show_gauge(int percent, int r, int g, int b, int bg_r, int bg_g, int bg_b)
  {
    if (percent <= 100)
    {
      Color c = Color(r, g, b);
      Color bgc = Color(bg_r, bg_g, bg_b);
      for (uint8_t i = 0; i < 8; i++)
      {
        if (percent > i * 12.5)
        {
          this->cgauge[7 - i] = c;
        }
        else
        {
          this->cgauge[7 - i] = bgc;
        }
      }
      this->display_gauge = true;
      ESP_LOGD(TAG, "show_gauge 2 color %d", round(percent));
    }
  }
#else
  void EHMTX::show_gauge(int percent, int r, int g, int b, int bg_r, int bg_g, int bg_b)
  {
    this->display_gauge = false;
    if (percent <= 100)
    {
      this->gauge_color = Color(r, g, b);
      this->gauge_bgcolor = Color(bg_r, bg_g, bg_b);
      this->display_gauge = true;
      this->gauge_value = (uint8_t)(100 - percent) * 7 / 100;
    }
    ESP_LOGD(TAG, "show_gauge 2 color %d", round(percent));
  }
#endif

#ifdef USE_ESP8266
  void EHMTX::draw_gauge()
  {
    if (this->display_gauge)
    {
      this->display->line(0, 7, 0, 0, this->gauge_bgcolor);
      this->display->line(1, 7, 1, 0, esphome::display::COLOR_OFF);
      this->display->line(0, 7, 0, this->gauge_value, this->gauge_color);
    }
  }
#else
  void EHMTX::draw_gauge()
  {
    if (this->display_gauge)
    {
      for (uint8_t y = 0; y < 8; y++)
      {
        this->display->draw_pixel_at(0, y, this->cgauge[y]);
      }
      this->display->line(1, 7, 1, 0, esphome::display::COLOR_OFF);
    }
  }
#endif

  void EHMTX::setup()
  {
    ESP_LOGD(TAG, "Setting up services");
    register_service(&EHMTX::get_status, "get_status");
    register_service(&EHMTX::set_display_on, "display_on");
    register_service(&EHMTX::set_display_off, "display_off");
    register_service(&EHMTX::hold_screen, "hold_screen", {"time"});
    register_service(&EHMTX::hide_rindicator, "hide_rindicator");
    register_service(&EHMTX::hide_lindicator, "hide_lindicator");
    register_service(&EHMTX::hide_gauge, "hide_gauge");
    register_service(&EHMTX::hide_alarm, "hide_alarm");
    register_service(&EHMTX::show_gauge, "show_gauge", {"percent", "r", "g", "b", "bg_r", "bg_g", "bg_b"});
    register_service(&EHMTX::show_alarm, "show_alarm", {"r", "g", "b", "size"});
    register_service(&EHMTX::show_rindicator, "show_rindicator", {"r", "g", "b", "size"});
    register_service(&EHMTX::show_lindicator, "show_lindicator", {"r", "g", "b", "size"});

    register_service(&EHMTX::set_today_color, "set_today_color", {"r", "g", "b"});
    register_service(&EHMTX::set_weekday_color, "set_weekday_color", {"r", "g", "b"});
    register_service(&EHMTX::set_clock_color, "set_clock_color", {"r", "g", "b"});
    register_service(&EHMTX::set_text_color, "set_text_color", {"r", "g", "b"});
    register_service(&EHMTX::set_infotext_color, "set_infotext_color", {"left_r", "left_g", "left_b", "right_r", "right_g", "right_b", "default_font", "y_offset"});
    register_service(&EHMTX::expand_icon_to_9, "expand_icon_to_9", {"mode"});

    register_service(&EHMTX::set_solid_color, "set_solid_color", {"r", "g", "b"});
    register_service(&EHMTX::set_calendar_color, "set_calendar_color", {"r", "g", "b"});

    register_service(&EHMTX::set_night_mode_on, "night_mode_on");
    register_service(&EHMTX::set_night_mode_off, "night_mode_off");

    register_service(&EHMTX::set_weekday_accent_on, "weekday_accent_on");
    register_service(&EHMTX::set_weekday_accent_off, "weekday_accent_off");

    register_service(&EHMTX::del_screen, "del_screen", {"icon_name", "mode"});
    register_service(&EHMTX::force_screen, "force_screen", {"icon_name", "mode"});

    register_service(&EHMTX::full_screen, "full_screen", {"icon_name", "lifetime", "screen_time"});
    register_service(&EHMTX::icon_screen, "icon_screen", {"icon_name", "text", "lifetime", "screen_time", "default_font", "r", "g", "b"});
    register_service(&EHMTX::alert_screen, "alert_screen", {"icon_name", "text", "screen_time", "default_font", "r", "g", "b"});
    register_service(&EHMTX::rainbow_alert_screen, "rainbow_alert_screen", {"icon_name", "text", "screen_time", "default_font"});
    register_service(&EHMTX::icon_clock, "icon_clock", {"icon_name", "lifetime", "screen_time", "default_font", "r", "g", "b"});
    register_service(&EHMTX::icon_date, "icon_date", {"icon_name", "lifetime", "screen_time", "default_font", "r", "g", "b"});
#ifdef USE_GRAPH
    register_service(&EHMTX::graph_screen, "graph_screen", {"lifetime", "screen_time"});
    register_service(&EHMTX::icon_graph_screen, "icon_graph_screen", {"icon_name", "lifetime", "screen_time"});
#endif
    register_service(&EHMTX::rainbow_icon_screen, "rainbow_icon_screen", {"icon_name", "text", "lifetime", "screen_time", "default_font"});

    register_service(&EHMTX::icon_text_screen, "icon_text_screen", {"icon_name", "text", "lifetime", "screen_time", "default_font", "r", "g", "b"});
    register_service(&EHMTX::rainbow_icon_text_screen, "rainbow_icon_text_screen", {"icon_name", "text", "lifetime", "screen_time", "default_font"});

    register_service(&EHMTX::text_screen, "text_screen", {"text", "lifetime", "screen_time", "default_font", "r", "g", "b"});
    register_service(&EHMTX::rainbow_text_screen, "rainbow_text_screen", {"text", "lifetime", "screen_time", "default_font"});

    register_service(&EHMTX::clock_screen, "clock_screen", {"lifetime", "screen_time", "default_font", "r", "g", "b"});

    register_service(&EHMTX::rainbow_clock_screen, "rainbow_clock_screen", {"lifetime", "screen_time", "default_font"});

    register_service(&EHMTX::date_screen, "date_screen", {"lifetime", "screen_time", "default_font", "r", "g", "b"});
    register_service(&EHMTX::rainbow_date_screen, "rainbow_date_screen", {"lifetime", "screen_time", "default_font"});

    register_service(&EHMTX::blank_screen, "blank_screen", {"lifetime", "screen_time"});
    register_service(&EHMTX::color_screen, "color_screen", {"lifetime", "screen_time", "r", "g", "b"});

    register_service(&EHMTX::set_brightness, "brightness", {"value"});
#ifndef USE_ESP8266
    register_service(&EHMTX::color_gauge, "color_gauge", {"colors"});
    register_service(&EHMTX::bitmap_screen, "bitmap_screen", {"icon", "lifetime", "screen_time"});
    register_service(&EHMTX::bitmap_small, "bitmap_small", {"icon", "text", "lifetime", "screen_time", "default_font", "r", "g", "b"});
    register_service(&EHMTX::rainbow_bitmap_small, "rainbow_bitmap_small", {"icon", "text", "lifetime", "screen_time", "default_font"});
    register_service(&EHMTX::bitmap_stack, "bitmap_stack", {"icons", "lifetime", "screen_time"});
#endif

    register_service(&EHMTX::show_icon_indicator, "show_icon_indicator", {"r", "g", "b", "size", "pos", "height"});
    register_service(&EHMTX::hide_icon_indicator, "hide_icon_indicator");

#ifdef USE_Fireplugin
    register_service(&EHMTX::fire_screen, "fire_screen", {"lifetime", "screen_time"});
#endif

#ifdef EHMTXv2_ADV_CLOCK
    register_service(&EHMTX::set_clock_infotext_color, "set_clock_infotext_color", {"left_r", "left_g", "left_b", "right_r", "right_g", "right_b", "default_font", "y_offset"});
    register_service(&EHMTX::set_adv_clock_color, "set_adv_clock_color", {"hr", "hg", "hb", "mr", "mg", "mb", "sr", "sg", "sb"});
#endif

    register_service(&EHMTX::text_screen_progress, "text_screen_progress", {"text", "value", "progress", "lifetime", "screen_time", "default_font", "value_color_as_progress", "r", "g", "b"});
    register_service(&EHMTX::icon_screen_progress, "icon_screen_progress", {"icon_name", "text", "progress", "lifetime", "screen_time", "default_font", "r", "g", "b"});
    register_service(&EHMTX::set_progressbar_color, "set_progressbar_color", {"icon_name", "mode", "r", "g", "b", "bg_r", "bg_g", "bg_b"});

    register_service(&EHMTX::icon_prognosis_screen, "icon_prognosis_screen", {"icon_name", "text", "prognosis", "lifetime", "screen_time", "default_font"});
    register_service(&EHMTX::icon_prognosis_screen_rgb, "icon_prognosis_screen_rgb", {"icon_name", "text", "prognosis", "lifetime", "screen_time", "default_font", "r", "g", "b"});

    ESP_LOGD(TAG, "Setup and running!");
  }

  void EHMTX::show_alarm(int r, int g, int b, int size)
  {
    if (size > 0)
    {
      this->alarm_color = Color((uint8_t)r, (uint8_t)g, (uint8_t)b);
      this->display_alarm = size & 3;
      ESP_LOGD(TAG, "show alarm size: %d color r: %d g: %d b: %d", size, r, g, b);
    }
    else
    {
      this->hide_alarm();
    }
  }

  void EHMTX::hide_alarm()
  {
    this->display_alarm = 0;
    ESP_LOGD(TAG, "hide alarm");
  }

  void EHMTX::set_clock_color(int r, int g, int b)
  {
    this->clock_color = Color((uint8_t)r, (uint8_t)g, (uint8_t)b);

    for (size_t i = 0; i < MAXQUEUE; i++)
    {
      if (this->queue[i]->mode == MODE_CLOCK ||
          this->queue[i]->mode == MODE_DATE ||
          this->queue[i]->mode == MODE_ICON_CLOCK ||
          this->queue[i]->mode == MODE_ICON_DATE)
      {
        this->queue[i]->text_color = this->clock_color;
      }
    }
    ESP_LOGD(TAG, "default clock color r: %d g: %d b: %d", r, g, b);
  }

  void EHMTX::set_text_color(int r, int g, int b)
  {
    this->text_color = Color((uint8_t)r, (uint8_t)g, (uint8_t)b);
    ESP_LOGD(TAG, "default text color r: %d g: %d b: %d", r, g, b);
  }

  void EHMTX::set_infotext_color(int lr, int lg, int lb, int rr, int rg, int rb, bool df, int y_offset)
  {
    this->info_lcolor = Color((uint8_t)lr, (uint8_t)lg, (uint8_t)lb);
    this->info_rcolor = Color((uint8_t)rr, (uint8_t)rg, (uint8_t)rb);
    this->info_font = df;
    this->info_y_offset = y_offset;
    ESP_LOGD(TAG, "info text color left: r: %d g: %d b: %d right: r: %d g: %d b: %d y_offset %d", lr, lg, lb, rr, rg, rb, y_offset);
  }

  void EHMTX::set_solid_color(int r, int g, int b)
  {
    this->solid_color = Color((uint8_t)r, (uint8_t)g, (uint8_t)b);
    ESP_LOGD(TAG, "solid icon color r: %d g: %d b: %d", r, g, b);
  }

  void EHMTX::set_calendar_color(int r, int g, int b)
  {
    this->calendar_color = Color((uint8_t)r, (uint8_t)g, (uint8_t)b);
    ESP_LOGD(TAG, "calendar icon color r: %d g: %d b: %d", r, g, b);
  }

  void EHMTX::update() // called from polling component
  {
    if (!this->is_running)
    {
      if (this->clock->now().is_valid())
      {
        ESP_LOGD(TAG, "time sync => start running");

        this->is_running = true;

        for (auto *t : on_start_running_triggers_)
        {
          ESP_LOGD(TAG, "on_start_running_triggers");
          t->process();
        }
      }
    }
    else
    {
    }
  }

  void EHMTX::force_screen(std::string icon_name, int mode)
  {
    for (uint8_t i = 0; i < MAXQUEUE; i++)
    {
      if (this->queue[i]->mode == mode)
      {
        bool force = true;
        if ((mode == MODE_ICON_SCREEN) ||
            (mode == MODE_ICON_CLOCK) ||
            (mode == MODE_ICON_DATE) ||
            (mode == MODE_FULL_SCREEN) ||
            (mode == MODE_RAINBOW_ICON) ||
            (mode == MODE_ICON_PROGRESS) ||
            (mode == MODE_ICON_TEXT_SCREEN) ||
            (mode == MODE_RAINBOW_ICON_TEXT_SCREEN) ||
            (mode == MODE_TEXT_PROGRESS) ||
#ifdef EHMTXv2_ADV_BITMAP
            (mode == MODE_BITMAP_SCREEN) ||
#endif
            (mode == MODE_PROGNOSIS_SCREEN))
        {
          if (strcmp(this->queue[i]->icon_name.c_str(), icon_name.c_str()) != 0)
          {
            force = false;
          }
        }
        // Will allow force any screen [andrewjswan].
        if (force)
        {
          ESP_LOGD(TAG, "force_screen: found position: %d", i);
          this->next_action_time = this->get_tick();
          this->queue[i]->last_time = 0.0;
          if (this->queue[i]->endtime < this->next_action_time + this->queue[i]->screen_time_)
          {
            this->queue[i]->endtime = this->next_action_time + this->queue[i]->screen_time_;
          }
          ESP_LOGW(TAG, "force_screen: icon %s in mode %d", icon_name.c_str(), mode);
        }
      }
    }
  }

  uint8_t EHMTX::find_oldest_queue_element()
  {
    uint8_t hit = MAXQUEUE;
    float last_time = this->get_tick();
    for (size_t i = 0; i < MAXQUEUE; i++)
    {
      if (this->night_mode)
      {
        bool skip = true;
        for (auto id : EHMTXv2_CONF_NIGHT_MODE_SCREENS)
        {
          if (this->queue[i]->mode == id)
          {
            skip = false;
          }
        }
        if (skip)
        {
          continue;
        }
      }

      if ((this->queue[i]->endtime > this->get_tick()) && (this->queue[i]->last_time < last_time))
      {
        hit = i;
        last_time = this->queue[i]->last_time;
      }
    }

    uint8_t queue_count = this->queue_count();
    if (hit != MAXQUEUE)
    {
      ESP_LOGD(TAG, "oldest queue element is: %d/%d", hit, queue_count);
    }
    else if (queue_count > 0)
    {
      hit = 0;
      ESP_LOGD(TAG, "oldest queue element is first: %d/%d", hit, queue_count);
    }
    else
    {
      ESP_LOGD(TAG, "oldest queue element not found");
    }
    this->queue[hit]->status();

    return hit;
  }

  uint8_t EHMTX::find_last_clock()
  {
    uint8_t hit = MAXQUEUE;
    if (EHMTXv2_CLOCK_INTERVALL > 0)
    {
      float ts = this->get_tick();
      for (size_t i = 0; i < MAXQUEUE; i++)
      {
        if (this->night_mode)
        {
          bool skip = true;
          for (auto id : EHMTXv2_CONF_NIGHT_MODE_SCREENS)
          {
            if (this->queue[i]->mode == id)
            {
              skip = false;
            }
          }
          if (skip)
          {
            continue;
          }
        }

        if ((this->queue[i]->mode == MODE_CLOCK) || (this->queue[i]->mode == MODE_RAINBOW_CLOCK) || (this->queue[i]->mode == MODE_ICON_CLOCK))
        {
          if (ts > (this->queue[i]->last_time + EHMTXv2_CLOCK_INTERVALL * 1000.0))
          {
            hit = i;
          }
          break;
        }
      }
      if (hit != MAXQUEUE)
      {
        ESP_LOGD(TAG, "forced clock_interval");
      }
    }
    return hit;
  }

  void EHMTX::remove_expired_queue_element()
  {
    if (this->clock->now().is_valid())
    {
      std::string infotext;
      float ts = this->get_tick() + static_cast<float>(EHMTXv2_SCROLL_INTERVALL); // Force remove expired queue element

      for (size_t i = 0; i < MAXQUEUE; i++)
      {
        if ((this->queue[i]->endtime > 0.0) && (this->queue[i]->endtime < ts))
        {
          this->queue[i]->endtime = 0.0;
          if (this->queue[i]->mode != MODE_EMPTY)
          {
            ESP_LOGD(TAG, "remove expired queue element: slot %d: mode: %d icon_name: %s text: %s", i, this->queue[i]->mode, this->queue[i]->icon_name.c_str(), this->queue[i]->text.c_str());
            for (auto *t : on_expired_screen_triggers_)
            {
              infotext = "";
              switch (this->queue[i]->mode)
              {
              case MODE_EMPTY:
                break;
              case MODE_BLANK:
                infotext = "blank";
                break;
              case MODE_COLOR:
                infotext = "color";
                break;
              case MODE_CLOCK:
              case MODE_RAINBOW_CLOCK:
                infotext = "clock";
                break;
              case MODE_DATE:
              case MODE_RAINBOW_DATE:
                infotext = "date";
                break;
              case MODE_FULL_SCREEN:
                infotext = "full screen: " + this->queue[i]->icon_name;
                break;
              case MODE_ICON_SCREEN:
              case MODE_RAINBOW_ICON:
              case MODE_ICON_CLOCK:
              case MODE_ICON_DATE:
              case MODE_ICON_PROGRESS:
              case MODE_ICON_TEXT_SCREEN:
              case MODE_RAINBOW_ICON_TEXT_SCREEN:
              case MODE_TEXT_PROGRESS:
              case MODE_PROGNOSIS_SCREEN:
                infotext = this->queue[i]->icon_name.c_str();
                break;
              case MODE_ALERT_SCREEN:
              case MODE_RAINBOW_ALERT_SCREEN:
                infotext = ("alert: " + this->queue[i]->icon_name).c_str();
                break;
              case MODE_GRAPH_SCREEN:
                infotext = ("graph: " + this->queue[i]->icon_name).c_str();
                break;
              case MODE_RAINBOW_TEXT:
              case MODE_TEXT_SCREEN:
                infotext = "text";
                break;
              case MODE_BITMAP_SMALL:
              case MODE_RAINBOW_BITMAP_SMALL:
                infotext = ("bitmap small: " + this->queue[i]->icon_name).c_str();
                break;
              case MODE_BITMAP_SCREEN:
                infotext = ("bitmap: " + this->queue[i]->icon_name).c_str();
                break;
              case MODE_BITMAP_STACK_SCREEN:
                infotext = ("bitmap stack: " + this->queue[i]->text).c_str();
                break;
              case MODE_FIRE:
                infotext = "fire";
                break;
              default:
                break;
              }
              t->process(this->queue[i]->icon_name, infotext);
            }
          }
          this->queue[i]->mode = MODE_EMPTY;
          if (this->queue[i]->sbitmap != NULL)
          {
            delete[] this->queue[i]->sbitmap;
            this->queue[i]->sbitmap = nullptr;
          }
        }
      }
    }
  }

  uint8_t EHMTX::queue_count()
  {
    float ts = this->get_tick();
    uint8_t c = 0;
    for (size_t i = 0; i < MAXQUEUE; i++)
    {
      if (this->queue[i]->endtime > ts)
      {
        c++;
      }
    }

    return c;
  }

  // tick in milliseconds
  float EHMTX::get_tick()
  {
#ifdef USE_ESP32
    struct timespec spec;
    clock_gettime(CLOCK_MONOTONIC, &spec);
    // tv_sec  - seconds
    // tv_nsec - nanoseconds
    return static_cast<float>(spec.tv_sec * 1000 + spec.tv_nsec / 1000000);
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    // tv_sec  - seconds
    // tv_usec - microseconds
    return static_cast<float>(tv.tv_sec * 1000 + tv.tv_usec / 1000);
#endif
  }

  void EHMTX::tick()
  {
    this->hue_++;
    if (this->hue_ == 360)
    {
      this->hue_ = 0;
    }
    float red, green, blue;
    esphome::hsv_to_rgb(this->hue_, 0.8, 0.8, red, green, blue);
    this->rainbow_color = Color(uint8_t(255 * red), uint8_t(255 * green), uint8_t(255 * blue));

    if (this->is_running && this->clock->now().is_valid())
    {
      float ts = this->get_tick();

      if (millis() - this->last_scroll_time >= EHMTXv2_SCROLL_INTERVALL)
      {
        this->scroll_step++;
        this->last_scroll_time = millis();
        if (this->scroll_step > this->queue[this->screen_pointer]->scroll_reset)
        {
          this->scroll_step = 0;
        }
      }

      if (ts > this->next_action_time)
      {
        this->remove_expired_queue_element();
        this->screen_pointer = this->find_last_clock();
        this->scroll_step = 0;
        this->ticks_ = 0;
#ifdef EHMTXv2_USE_VERTICAL_SCROLL
        this->vertical_scroll = false;
#endif

        if (this->screen_pointer == MAXQUEUE)
        {
          this->screen_pointer = find_oldest_queue_element();
        }

        if (this->screen_pointer != MAXQUEUE)
        {
          this->queue[this->screen_pointer]->last_time = ts;
          // todo nur bei animationen
          if (this->queue[this->screen_pointer]->mode == MODE_BITMAP_STACK_SCREEN && this->queue[this->screen_pointer]->sbitmap != NULL)
          {
            for (uint8_t i = 0; i < this->queue[this->screen_pointer]->icon; i++)
            {
              this->icons[this->queue[this->screen_pointer]->sbitmap[i].b]->set_frame(0);
              this->queue[this->screen_pointer]->sbitmap[i] = Color(127, 255, this->queue[this->screen_pointer]->sbitmap[i].b, 5);
              this->queue[this->screen_pointer]->default_font = false;
            }
          }
          else if (this->queue[this->screen_pointer]->icon < this->icon_count)
          {
            this->icons[this->queue[this->screen_pointer]->icon]->set_frame(0);
          }
          this->next_action_time = this->queue[this->screen_pointer]->last_time + this->queue[this->screen_pointer]->screen_time_;
          // Todo switch for Triggers
          if (this->queue[this->screen_pointer]->mode == MODE_CLOCK)
          {
            for (auto *t : on_next_clock_triggers_)
            {
              t->process();
            }
          }
          else
          {
            for (auto *t : on_next_screen_triggers_)
            {
              ESP_LOGD(TAG, "on_next_screen trigger");
              std::string infotext = "";
              switch (this->queue[this->screen_pointer]->mode)
              {
              case MODE_EMPTY:
                infotext = "empty";
                break;
              case MODE_BLANK:
                infotext = "blank";
                break;
              case MODE_COLOR:
                infotext = "color";
                break;
              case MODE_CLOCK:
              case MODE_RAINBOW_CLOCK:
                infotext = "clock";
                break;
              case MODE_DATE:
              case MODE_RAINBOW_DATE:
                infotext = "date";
                break;
              case MODE_FULL_SCREEN:
                infotext = "full screen " + this->queue[this->screen_pointer]->icon_name;
                break;
              case MODE_ICON_SCREEN:
              case MODE_RAINBOW_ICON:
              case MODE_ICON_CLOCK:
              case MODE_ICON_DATE:
              case MODE_ICON_PROGRESS:
              case MODE_ICON_TEXT_SCREEN:
              case MODE_RAINBOW_ICON_TEXT_SCREEN:
              case MODE_TEXT_PROGRESS:
              case MODE_PROGNOSIS_SCREEN:
                infotext = this->queue[this->screen_pointer]->icon_name.c_str();
                break;
              case MODE_ALERT_SCREEN:
              case MODE_RAINBOW_ALERT_SCREEN:
                infotext = ("alert: " + this->queue[this->screen_pointer]->icon_name).c_str();
                break;
              case MODE_GRAPH_SCREEN:
                infotext = ("graph: " + this->queue[this->screen_pointer]->icon_name).c_str();
                break;
              case MODE_RAINBOW_TEXT:
              case MODE_TEXT_SCREEN:
                infotext = "text";
                break;
              case MODE_BITMAP_SMALL:
              case MODE_RAINBOW_BITMAP_SMALL:
                infotext = ("bitmap small: " + this->queue[this->screen_pointer]->icon_name).c_str();
                break;
              case MODE_BITMAP_SCREEN:
                infotext = ("bitmap: " + this->queue[this->screen_pointer]->icon_name).c_str();
                break;
              case MODE_BITMAP_STACK_SCREEN:
                infotext = ("bitmap stack: " + this->queue[this->screen_pointer]->text).c_str();
                break;
              case MODE_FIRE:
                infotext = "fire";
                break;
              default:
                break;
              }
              t->process(infotext, this->queue[this->screen_pointer]->text);
            }
          }
        }
        else
        {
          this->next_action_time = ts;
          for (auto *t : on_empty_queue_triggers_)
          {
            ESP_LOGD(TAG, "on_empty_queue trigger");
            t->process();
          }
        }
      }

      // blend handling
#ifdef EHMTXv2_BLEND_STEPS
      if ((this->ticks_ <= EHMTXv2_BLEND_STEPS) && (this->brightness_ >= 50) && (this->queue_count() > 1))
      {
        uint8_t b = this->brightness_;
        uint8_t current_step = ((b - 50) / 205) * (EHMTXv2_BLEND_STEPS - EHMTXv2_BLEND_STEPS / 2) + EHMTXv2_BLEND_STEPS / 2;
        if (this->ticks_ <= current_step)
        {
          float br = lerp((float)this->ticks_ / current_step, 0, (float)b / 255);
          this->display->get_light()->set_correction(br, br, br);
        }
      }
      else
#endif
      {
        if (this->brightness_ != this->target_brightness_)
        {
          this->brightness_ = this->brightness_ + (this->target_brightness_ < this->brightness_ ? -1 : 1);
          float br = (float)this->brightness_ / (float)255;
          this->display->get_light()->set_correction(br, br, br);
        }
      }
      this->ticks_++;
    }
    else
    {
#ifdef USE_ESP32
  #ifdef EHMTXv2_ADV_BOOT
      if (this->boot_logo != NULL)
      {
    #if defined EHMTXv2_ADV_BOOT_MODE_0 || defined EHMTXv2_ADV_BOOT_MODE_2 || defined EHMTXv2_ADV_BOOT_MODE_4 || defined EHMTXv2_ADV_BOOT_MODE_6 || defined EHMTXv2_ADV_BOOT_MODE_7
      #if defined EHMTXv2_ADV_BOOT_MODE_6 || defined EHMTXv2_ADV_BOOT_MODE_7
        if (this->boot_anim % 8 == 0 )
      #endif
        for (uint8_t x = 0; x < 32; x++)
        {
          for (uint8_t y = 0; y < 8; y++)
          {
      #ifdef EHMTXv2_ADV_BOOT_MODE_0
            this->display->draw_pixel_at(x, y, this->boot_logo[x + y * 32]);
      #else
        #if defined EHMTXv2_ADV_BOOT_MODE_2 || defined EHMTXv2_ADV_BOOT_MODE_4
            if (this->boot_logo[x + y * 32] == 1)
            {
          #ifdef EHMTXv2_ADV_BOOT_MODE_2
              this->display->draw_pixel_at(x, y, Color(C_RED, C_GREEN, C_BLUE));
          #else
              this->display->draw_pixel_at(x, y, this->rainbow_color);
          #endif
            }
        #endif
        #if defined EHMTXv2_ADV_BOOT_MODE_6 || defined EHMTXv2_ADV_BOOT_MODE_7
            if (CHECK_BIT(this->boot_logo[x + y * 32], ((this->boot_anim % 64) / 8)))
            {
          #ifdef EHMTXv2_ADV_BOOT_MODE_6
              this->display->draw_pixel_at(x, y, Color(C_RED, C_GREEN, C_BLUE));
          #else
              this->display->draw_pixel_at(x, y, this->rainbow_color);
          #endif
            }
        #endif
      #endif
          }
        }
    #endif
    #if defined EHMTXv2_ADV_BOOT_MODE_1 || defined EHMTXv2_ADV_BOOT_MODE_3 || defined EHMTXv2_ADV_BOOT_MODE_5
        if (this->boot_anim % 8 == 0 )
        {
          uint8_t w = (2 + (uint8_t)(32 / 16) * (this->boot_anim * 2 / 16)) % 32;
          uint8_t l = 16 - w / 2;
          uint8_t r = 15 + w / 2;
          for (uint8_t y = 0; y < 8; y++)
          {
      #if defined EHMTXv2_ADV_BOOT_MODE_3 || defined EHMTXv2_ADV_BOOT_MODE_5
            if (this->boot_logo[l + y * 32] == 1)
            {
        #ifdef EHMTXv2_ADV_BOOT_MODE_3
              this->display->draw_pixel_at(l, y, Color(C_RED, C_GREEN, C_BLUE));
        #else
              this->display->draw_pixel_at(l, y, this->rainbow_color);
        #endif
            }
            if (this->boot_logo[r + y * 32] == 1)
            {
        #ifdef EHMTXv2_ADV_BOOT_MODE_3
              this->display->draw_pixel_at(r, y, Color(C_RED, C_GREEN, C_BLUE));
        #else
              this->display->draw_pixel_at(r, y, this->rainbow_color);
        #endif
            }
      #endif
      #ifdef EHMTXv2_ADV_BOOT_MODE_1
            this->display->draw_pixel_at(l, y, this->boot_logo[l + y * 32]);
            this->display->draw_pixel_at(r, y, this->boot_logo[r + y * 32]);
      #endif
          }
        }
    #endif
      }
  #endif
#endif

#ifndef EHMTXv2_ADV_BOOT
      {
        uint8_t w = 2 + ((uint8_t)(32 / 16) * (this->boot_anim / 16)) % 32;
        uint8_t l = 32 / 2 - w / 2;
        this->display->rectangle(l, 2, w, 4, this->rainbow_color);
      }
#endif
      this->boot_anim++;
    }
  }

  void EHMTX::skip_screen()
  {
    this->next_action_time = this->get_tick() - 1000.0;
  }

  void EHMTX::hold_screen(int time)
  {
    this->next_action_time = this->get_tick() + time * 1000.0;
  }

  void EHMTX::get_status()
  {
    time_t ts = this->clock->now().timestamp;
    ESP_LOGI(TAG, "status time: %d.%d.%d %02d:%02d", this->clock->now().day_of_month,
             this->clock->now().month, this->clock->now().year,
             this->clock->now().hour, this->clock->now().minute);
    ESP_LOGI(TAG, "status brightness: %d (0..255)", this->brightness_);
    ESP_LOGI(TAG, "status date format: %s", EHMTXv2_DATE_FORMAT);
    ESP_LOGI(TAG, "screen_pointer: %d", this->screen_pointer);
    if (this->screen_pointer != MAXQUEUE)
    {
      ESP_LOGI(TAG, "current screen mode: %d", this->queue[this->screen_pointer]->mode);
    }
    else
    {
      ESP_LOGI(TAG, "screenpointer at max");
    }
    ESP_LOGI(TAG, "status time format: %s", EHMTXv2_TIME_FORMAT);
    ESP_LOGI(TAG, "status date format: %s", EHMTXv2_DATE_FORMAT);
    ESP_LOGI(TAG, "status display %s", this->show_display ? F("on") : F("off"));
    ESP_LOGI(TAG, "status night mode %s", this->night_mode ? F("on") : F("off"));
    ESP_LOGI(TAG, "status weekday accent %s", this->weekday_accent ? F("on") : F("off"));
    ESP_LOGI(TAG, "status replace time and date %s", this->replace_time_date_active ? F("on") : F("off"));

    this->queue_status();
  }

  void EHMTX::queue_status()
  {
    uint8_t empty = 0;
    for (uint8_t i = 0; i < MAXQUEUE; i++)
    {
      if (this->queue[i]->mode != MODE_EMPTY)
        this->queue[i]->status();
      else
        empty++;
    }
    if (empty > 0)
      ESP_LOGI(TAG, "queue: %d empty slots", empty);
  }

  void EHMTX::set_default_font(display::BaseFont *font)
  {
    this->default_font = font;
  }

  void EHMTX::set_special_font(display::BaseFont *font)
  {
    this->special_font = font;
  }

  void EHMTX::set_replace_time_date_active(bool b)
  {
    this->replace_time_date_active = b;
    ESP_LOGI(TAG, "replace_time_date %s", b ? F("on") : F("off"));
  }

  std::string EHMTX::replace_time_date(std::string time_date) // Replace Time Date Strings / Trip5
  {
    std::string replace_from_string = EHMTXv2_REPLACE_TIME_DATE_FROM;
    std::string replace_to_string = EHMTXv2_REPLACE_TIME_DATE_TO;
    std::string replace_from_arr[50]; // AM + PM + 7 Days + 12 Months = 21 but 50 to be super-safe
    std::string replace_to_arr[50];
    std::string replace_from;
    std::string replace_to;
    uint16_t replace_arr_n;
    if (replace_from_string != "" && replace_to_string != "")
    {
      std::istringstream iss_from(replace_from_string);
      std::vector<std::string> replace_from_arr;
      for (std::string s_from; iss_from >> s_from;)
        replace_from_arr.push_back(s_from);
      replace_arr_n = replace_from_arr.size();
      std::istringstream iss_to(replace_to_string);
      std::vector<std::string> replace_to_arr;
      for (std::string s_to; iss_to >> s_to;)
        replace_to_arr.push_back(s_to);
      if (replace_to_arr.size() < replace_arr_n)
      {
        replace_arr_n = replace_to_arr.size();
      }
      uint16_t k = 0;
      do
      {
        std::vector<std::uint8_t> data(time_date.begin(), time_date.end());
        std::vector<std::uint8_t> pattern(replace_from_arr[k].begin(), replace_from_arr[k].end());
        std::vector<std::uint8_t> replaceData(replace_to_arr[k].begin(), replace_to_arr[k].end());
        std::vector<std::uint8_t>::iterator itr;
        while ((itr = std::search(data.begin(), data.end(), pattern.begin(), pattern.end())) != data.end())
        {
          data.erase(itr, itr + pattern.size());
          data.insert(itr, replaceData.begin(), replaceData.end());
        }
        time_date = std::string(data.begin(), data.end());
        k++;
      } while (k < replace_arr_n);
    }
    return time_date;
  }

  void EHMTX::del_screen(std::string icon_name, int mode)
  {
    for (uint8_t i = 0; i < MAXQUEUE; i++)
    {
      if (this->queue[i]->mode == mode)
      {
        bool force = true;
        ESP_LOGD(TAG, "del_screen: icon %s in position: %s mode %d", icon_name.c_str(), this->queue[i]->icon_name.c_str(), mode);
        if ((mode == MODE_ICON_SCREEN) ||
            (mode == MODE_ICON_CLOCK) ||
            (mode == MODE_ICON_DATE) ||
            (mode == MODE_FULL_SCREEN) ||
            (mode == MODE_RAINBOW_ICON) ||
            (mode == MODE_ICON_PROGRESS) ||
            (mode == MODE_ICON_TEXT_SCREEN) ||
            (mode == MODE_RAINBOW_ICON_TEXT_SCREEN) ||
            (mode == MODE_TEXT_PROGRESS) ||
#ifdef EHMTXv2_ADV_BITMAP
            (mode == MODE_BITMAP_SCREEN) ||
#endif
            (mode == MODE_PROGNOSIS_SCREEN))
        {
          if (this->string_has_ending(icon_name, "*"))
          {
            std::string comparename = icon_name.substr(0, icon_name.length() - 1);

            if (this->queue[i]->icon_name.rfind(comparename, 0) != 0)
            {
              force = false;
            }
          }
          else if (strcmp(this->queue[i]->icon_name.c_str(), icon_name.c_str()) != 0)
          {
            force = false;
          }
        }
        if (force)
        {
          ESP_LOGW(TAG, "del_screen: slot %d deleted", i);
          this->queue[i]->mode = MODE_EMPTY;
          this->queue[i]->endtime = 0.0;
          this->queue[i]->last_time = 0.0;
          if (this->queue[i]->sbitmap != NULL)
          {
            delete[] this->queue[i]->sbitmap;
            this->queue[i]->sbitmap = nullptr;
          }
#ifdef EHMTXv2_ADV_BITMAP
          if (this->queue[i]->bitmap != NULL)
          {
            delete[] this->queue[i]->bitmap;
            this->queue[i]->bitmap = nullptr;
          }
#endif
          if (i == this->screen_pointer)
          {
            this->next_action_time = this->get_tick();
          }
        }
      }
    }
  }

  void EHMTX::alert_screen(std::string iconname, std::string text, int screen_time, bool default_font, int r, int g, int b)
  {
    uint8_t icon = this->find_icon(iconname.c_str());

    if (icon == MAXICONS)
    {
      ESP_LOGW(TAG, "icon %d/%s not found => default: 0", icon, iconname.c_str());
      icon = 0;
      for (auto *t : on_icon_error_triggers_)
      {
        t->process(iconname);
      }
    }
    EHMTX_queue *screen = this->find_mode_queue_element(MODE_ALERT_SCREEN);

    screen->text = text;

    screen->text_color = Color(r, g, b);
    screen->default_font = default_font;
    screen->mode = MODE_ALERT_SCREEN;
    screen->icon_name = iconname;
    screen->icon = icon;
    screen->calc_scroll_time(text, screen_time);
    // time needed for scrolling
    screen->endtime = this->get_tick() + screen->screen_time_;
    for (auto *t : on_add_screen_triggers_)
    {
      t->process(screen->icon_name, (uint8_t)screen->mode);
    }
    ESP_LOGD(TAG, "alert screen icon: %d iconname: %s text: %s screen_time: %d", icon, iconname.c_str(), text.c_str(), screen_time);
    screen->status();

    force_screen(iconname, MODE_ALERT_SCREEN);
  }

  void EHMTX::rainbow_alert_screen(std::string iconname, std::string text, int screen_time, bool default_font)
  {
    uint8_t icon = this->find_icon(iconname.c_str());

    if (icon == MAXICONS)
    {
      ESP_LOGW(TAG, "icon %d/%s not found => default: 0", icon, iconname.c_str());
      icon = 0;
      for (auto *t : on_icon_error_triggers_)
      {
        t->process(iconname);
      }
    }
    EHMTX_queue *screen = this->find_mode_queue_element(MODE_RAINBOW_ALERT_SCREEN);

    screen->text = text;

    screen->default_font = default_font;
    screen->mode = MODE_RAINBOW_ALERT_SCREEN;
    screen->icon_name = iconname;
    screen->icon = icon;
    screen->calc_scroll_time(text, screen_time);
    // time needed for scrolling
    screen->endtime = this->get_tick() + screen->screen_time_;
    for (auto *t : on_add_screen_triggers_)
    {
      t->process(screen->icon_name, (uint8_t)screen->mode);
    }
    ESP_LOGD(TAG, "alert screen rainbow icon: %d iconname: %s text: %s screen_time: %d", icon, iconname.c_str(), text.c_str(), screen_time);
    screen->status();

    force_screen(iconname, MODE_RAINBOW_ALERT_SCREEN);
  }

  void EHMTX::icon_screen(std::string iconname, std::string text, int lifetime, int screen_time, bool default_font, int r, int g, int b)
  {
    std::string ic = get_icon_name(iconname);
    std::string id = get_screen_id(iconname);

    uint8_t icon = this->find_icon(ic.c_str());

    if (icon == MAXICONS)
    {
      ESP_LOGW(TAG, "icon %d/%s not found => default: 0", icon, ic.c_str());
      icon = 0;
      for (auto *t : on_icon_error_triggers_)
      {
        t->process(ic);
      }
    }
    EHMTX_queue *screen = this->find_mode_icon_queue_element(MODE_ICON_SCREEN, id);

    screen->text = text;
    screen->text_color = Color(r, g, b);
    screen->default_font = default_font;
    screen->mode = MODE_ICON_SCREEN;
    screen->icon_name = id;
    screen->icon = icon;
    screen->calc_scroll_time(text, screen_time);
    screen->endtime = this->get_tick() + (lifetime > 0 ? lifetime * 60000.0 : screen->screen_time_);
    for (auto *t : on_add_screen_triggers_)
    {
      t->process(screen->icon_name, (uint8_t)screen->mode);
    }
    ESP_LOGD(TAG, "icon screen icon: %d iconname: %s text: %s lifetime: %d screen_time: %d", icon, iconname.c_str(), text.c_str(), lifetime, screen_time);
    screen->status();
  }

  void EHMTX::text_screen_progress(std::string text, std::string value, int progress, int lifetime, int screen_time, bool default_font, bool value_color_as_progress, int r, int g, int b)
  {
    EHMTX_queue *screen = this->find_mode_icon_queue_element(MODE_TEXT_PROGRESS, text);

    screen->icon_name = text;
    screen->text = value;
    screen->text_color = Color(r, g, b);
    screen->default_font = default_font;
    screen->mode = MODE_TEXT_PROGRESS;
    screen->icon = value_color_as_progress;
    screen->progress = (progress > 100) ? 100 : (progress < -100) ? -100
                                                                  : progress;
    screen->screen_time_ = screen_time * 1000.0;
    screen->endtime = this->get_tick() + (lifetime > 0 ? lifetime * 60000.0 : screen->screen_time_);
#ifdef EHMTXv2_USE_VERTICAL_SCROLL
    screen->pixels_ = 0;
    screen->scroll_reset = 32;
#endif
    for (auto *t : on_add_screen_triggers_)
    {
      t->process(screen->icon_name, (uint8_t)screen->mode);
    }
    ESP_LOGD(TAG, "text progress screen text: %s value: %s progress %d lifetime: %d screen_time: %d", text.c_str(), value.c_str(), progress, lifetime, screen_time);
    screen->status();
  }

  void EHMTX::icon_screen_progress(std::string iconname, std::string text, int progress, int lifetime, int screen_time, bool default_font, int r, int g, int b)
  {
    std::string ic = get_icon_name(iconname);
    std::string id = get_screen_id(iconname);

    uint8_t icon = this->find_icon(ic.c_str());

    if (icon == MAXICONS)
    {
      ESP_LOGW(TAG, "icon %d/%s not found => default: 0", icon, ic.c_str());
      icon = 0;
      for (auto *t : on_icon_error_triggers_)
      {
        t->process(ic);
      }
    }
    EHMTX_queue *screen = this->find_mode_icon_queue_element(MODE_ICON_PROGRESS, id);

    screen->text = text;
    screen->text_color = Color(r, g, b);
    screen->default_font = default_font;
    screen->mode = MODE_ICON_PROGRESS;
    screen->icon_name = id;
    screen->icon = icon;
    screen->progress = (progress > 100) ? 100 : (progress < -100) ? -100
                                                                  : progress;
    screen->calc_scroll_time(text, screen_time);
    screen->endtime = this->get_tick() + (lifetime > 0 ? lifetime * 60000.0 : screen->screen_time_);
    for (auto *t : on_add_screen_triggers_)
    {
      t->process(screen->icon_name, (uint8_t)screen->mode);
    }
    ESP_LOGD(TAG, "icon progress screen icon: %d iconname: %s text: %s progress %d lifetime: %d screen_time: %d", icon, iconname.c_str(), text.c_str(), progress, lifetime, screen_time);
    screen->status();
  }

  void EHMTX::set_progressbar_color(std::string iconname, int mode, int r, int g, int b, int bg_r, int bg_g, int bg_b)
  {
    EHMTX_queue *screen = this->find_mode_icon_queue_element(mode, get_screen_id(iconname));

    screen->progressbar_color = (r + g + b == C_BLACK) ? esphome::display::COLOR_OFF : Color((uint8_t)r, (uint8_t)g, (uint8_t)b);
    screen->progressbar_back_color = (bg_r + bg_g + bg_b == C_BLACK) ? esphome::display::COLOR_OFF : Color((uint8_t)bg_r, (uint8_t)bg_g, (uint8_t)bg_b);

    ESP_LOGD(TAG, "progress screen mode: %d iconname: %s color progressbar: r: %d g: %d b: %d background: r: %d g: %d b: %d", mode, iconname.c_str(), r, g, b, bg_r, bg_g, bg_b);
    screen->status();
  }

  void EHMTX::icon_clock(std::string iconname, int lifetime, int screen_time, bool default_font, int r, int g, int b)
  {
    std::string ic = get_icon_name(iconname);
    std::string id = get_screen_id(iconname);

    uint8_t icon = this->find_icon(ic.c_str());

    if (icon == MAXICONS)
    {
      ESP_LOGW(TAG, "icon %d/%s not found => default: 0", icon, ic.c_str());
      icon = 0;
      for (auto *t : on_icon_error_triggers_)
      {
        t->process(ic);
      }
    }
    EHMTX_queue *screen = this->find_mode_icon_queue_element(MODE_ICON_CLOCK, id);

    screen->text_color = Color(r, g, b);
    screen->default_font = default_font;
    screen->mode = MODE_ICON_CLOCK;
    screen->icon_name = id;
    screen->icon = icon;
    screen->screen_time_ = screen_time * 1000.0;
    screen->endtime = this->get_tick() + (lifetime > 0 ? lifetime * 60000.0 : screen->screen_time_);
#ifdef EHMTXv2_USE_VERTICAL_SCROLL
    screen->pixels_ = 0;
    screen->scroll_reset = 32;
#endif
    for (auto *t : on_add_screen_triggers_)
    {
      t->process(screen->icon_name, (uint8_t)screen->mode);
    }
    ESP_LOGD(TAG, "icon clock icon: %d iconname: %s lifetime: %d screen_time: %d", icon, iconname.c_str(), lifetime, screen_time);
    screen->status();
  }

  void EHMTX::icon_date(std::string iconname, int lifetime, int screen_time, bool default_font, int r, int g, int b)
  {
    std::string ic = get_icon_name(iconname);
    std::string id = get_screen_id(iconname);

    uint8_t icon = this->find_icon(ic.c_str());

    if (icon == MAXICONS)
    {
      ESP_LOGW(TAG, "icon %d/%s not found => default: 0", icon, ic.c_str());
      icon = 0;
      for (auto *t : on_icon_error_triggers_)
      {
        t->process(ic);
      }
    }
    EHMTX_queue *screen = this->find_mode_icon_queue_element(MODE_ICON_DATE, id);

    screen->text_color = Color(r, g, b);
    screen->default_font = default_font;
    screen->mode = MODE_ICON_DATE;
    screen->icon_name = id;
    screen->icon = icon;
    screen->screen_time_ = screen_time * 1000.0;
    screen->endtime = this->get_tick() + (lifetime > 0 ? lifetime * 60000.0 : screen->screen_time_);
#ifdef EHMTXv2_USE_VERTICAL_SCROLL
    screen->pixels_ = 0;
    screen->scroll_reset = 32;
#endif
    for (auto *t : on_add_screen_triggers_)
    {
      t->process(screen->icon_name, (uint8_t)screen->mode);
    }
    ESP_LOGD(TAG, "icon date icon: %d iconname: %s lifetime: %d screen_time: %d", icon, iconname.c_str(), lifetime, screen_time);
    screen->status();
  }

  void EHMTX::rainbow_icon_screen(std::string iconname, std::string text, int lifetime, int screen_time, bool default_font)
  {
    std::string ic = get_icon_name(iconname);
    std::string id = get_screen_id(iconname);

    uint8_t icon = this->find_icon(ic.c_str());

    if (icon == MAXICONS)
    {
      ESP_LOGW(TAG, "icon %d/%s not found => default: 0", icon, ic.c_str());
      icon = 0;
      for (auto *t : on_icon_error_triggers_)
      {
        t->process(ic);
      }
    }
    EHMTX_queue *screen = this->find_mode_icon_queue_element(MODE_RAINBOW_ICON, id);

    screen->text = text;

    screen->default_font = default_font;
    screen->mode = MODE_RAINBOW_ICON;
    screen->icon_name = id;
    screen->icon = icon;
    screen->calc_scroll_time(text, screen_time);
    screen->endtime = this->get_tick() + (lifetime > 0 ? lifetime * 60000.0 : screen->screen_time_);
    for (auto *t : on_add_screen_triggers_)
    {
      t->process(screen->icon_name, (uint8_t)screen->mode);
    }
    ESP_LOGD(TAG, "rainbow icon screen icon: %d iconname: %s text: %s lifetime: %d screen_time: %d", icon, iconname.c_str(), text.c_str(), lifetime, screen_time);
    screen->status();
  }

  void EHMTX::rainbow_clock_screen(int lifetime, int screen_time, bool default_font)
  {
    EHMTX_queue *screen = this->find_mode_queue_element(MODE_RAINBOW_CLOCK);

    ESP_LOGD(TAG, "rainbow_clock_screen lifetime: %d screen_time: %d", lifetime, screen_time);
    screen->mode = MODE_RAINBOW_CLOCK;
    screen->default_font = default_font;
    if (EHMTXv2_CLOCK_INTERVALL == 0 || (EHMTXv2_CLOCK_INTERVALL * 1000.0 > screen_time * 1000.0))
    {
      screen->screen_time_ = screen_time * 1000.0;
    }
    else
    {
      screen->screen_time_ = EHMTXv2_CLOCK_INTERVALL * 1000.0 - 2000.0;
    }
#ifdef EHMTXv2_USE_VERTICAL_SCROLL
    screen->pixels_ = 0;
    screen->scroll_reset = 32;
#endif
    screen->endtime = this->get_tick() + (lifetime > 0 ? lifetime * 60000.0 : screen->screen_time_);
    screen->status();
  }

  void EHMTX::rainbow_date_screen(int lifetime, int screen_time, bool default_font)
  {
    ESP_LOGD(TAG, "rainbow_date_screen lifetime: %d screen_time: %d", lifetime, screen_time);
    EHMTX_queue *screen = this->find_mode_queue_element(MODE_RAINBOW_DATE);

    screen->mode = MODE_RAINBOW_DATE;
    screen->default_font = default_font;
    screen->screen_time_ = screen_time * 1000.0;
    screen->endtime = this->get_tick() + (lifetime > 0 ? lifetime * 60000.0 : screen->screen_time_);
#ifdef EHMTXv2_USE_VERTICAL_SCROLL
    screen->pixels_ = 0;
    screen->scroll_reset = 32;
#endif
    screen->status();
  }

  void EHMTX::blank_screen(int lifetime, int showtime)
  {
    EHMTX_queue *screen = this->find_free_queue_element();
    screen->mode = MODE_BLANK;
    screen->screen_time_ = showtime * 1000.0;
    screen->endtime = this->get_tick() + (lifetime > 0 ? lifetime * 60000.0 : screen->screen_time_);
    for (auto *t : on_add_screen_triggers_)
    {
      t->process("blank", (uint8_t)screen->mode);
    }
    screen->status();
  }

  void EHMTX::color_screen(int lifetime, int showtime, int r, int g, int b)
  {
    EHMTX_queue *screen = this->find_free_queue_element();
    screen->mode = MODE_COLOR;
    screen->screen_time_ = showtime * 1000.0;
    screen->text_color = Color(r, g, b);
    screen->endtime = this->get_tick() + (lifetime > 0 ? lifetime * 60000.0 : screen->screen_time_);
    for (auto *t : on_add_screen_triggers_)
    {
      t->process("color", (uint8_t)screen->mode);
    }
    screen->status();
  }

  void EHMTX::text_screen(std::string text, int lifetime, int screen_time, bool default_font, int r, int g, int b)
  {
    EHMTX_queue *screen = this->find_free_queue_element();

    screen->text = text;
    screen->default_font = default_font;
    screen->text_color = Color(r, g, b);
    screen->mode = MODE_TEXT_SCREEN;
    screen->calc_scroll_time(text, screen_time);
    screen->endtime = this->get_tick() + (lifetime > 0 ? lifetime * 60000.0 : screen->screen_time_);
    screen->status();
  }

  void EHMTX::rainbow_text_screen(std::string text, int lifetime, int screen_time, bool default_font)
  {
    EHMTX_queue *screen = this->find_free_queue_element();
    screen->text = text;
    screen->default_font = default_font;
    screen->mode = MODE_RAINBOW_TEXT;
    screen->calc_scroll_time(text, screen_time);
    screen->endtime = this->get_tick() + (lifetime > 0 ? lifetime * 60000.0 : screen->screen_time_);
    screen->status();
  }

  void EHMTX::fire_screen(int lifetime, int screen_time)
  {
    EHMTX_queue *screen = this->find_mode_queue_element(MODE_FIRE);
    screen->mode = MODE_FIRE;
    screen->icon = 0;
    screen->screen_time_ = screen_time * 1000.0;
    screen->endtime = this->get_tick() + (lifetime > 0 ? lifetime * 60000.0 : screen->screen_time_);
#ifdef EHMTXv2_USE_VERTICAL_SCROLL
    screen->pixels_ = 0;
    screen->scroll_reset = 32;
#endif
    for (auto *t : on_add_screen_triggers_)
    {
      t->process("Fire", (uint8_t)screen->mode);
    }
    ESP_LOGD(TAG, "fire screen: lifetime: %d screen_time:%d ", lifetime, screen_time);
    screen->status();
  }

  void EHMTX::full_screen(std::string iconname, int lifetime, int screen_time)
  {
    uint8_t icon = this->find_icon(iconname.c_str());

    if (icon == MAXICONS)
    {
      ESP_LOGW(TAG, "full screen: icon %d not found => default: 0", icon);
      for (auto *t : on_icon_error_triggers_)
      {
        t->process(iconname);
      }
      icon = 0;
    }
    EHMTX_queue *screen = this->find_icon_queue_element(icon);

    screen->mode = MODE_FULL_SCREEN;
    screen->icon = icon;
    screen->icon_name = iconname;
    screen->screen_time_ = screen_time * 1000.0;
    screen->endtime = this->get_tick() + (lifetime > 0 ? lifetime * 60000.0 : screen->screen_time_);
#ifdef EHMTXv2_USE_VERTICAL_SCROLL
    screen->pixels_ = 0;
    screen->scroll_reset = 32;
#endif
    for (auto *t : on_add_screen_triggers_)
    {
      t->process(screen->icon_name, (uint8_t)screen->mode);
    }
    ESP_LOGD(TAG, "full screen: icon: %d iconname: %s lifetime: %d screen_time:%d ", icon, iconname.c_str(), lifetime, screen_time);
    screen->status();
  }

  void EHMTX::clock_screen(int lifetime, int screen_time, bool default_font, int r, int g, int b)
  {
    EHMTX_queue *screen = this->find_mode_queue_element(MODE_CLOCK);
    screen->text_color = Color(r, g, b);
    ESP_LOGD(TAG, "clock_screen_color lifetime: %d screen_time: %d red: %d green: %d blue: %d", lifetime, screen_time, r, g, b);
    screen->mode = MODE_CLOCK;
    screen->default_font = default_font;
    screen->screen_time_ = screen_time * 1000.0;
    screen->endtime = this->get_tick() + (lifetime > 0 ? lifetime * 60000.0 : screen->screen_time_);
#ifdef EHMTXv2_USE_VERTICAL_SCROLL
    screen->pixels_ = 0;
    screen->scroll_reset = 32;
#endif
    screen->status();
  }

  void EHMTX::date_screen(int lifetime, int screen_time, bool default_font, int r, int g, int b)
  {
    ESP_LOGD(TAG, "date_screen lifetime: %d screen_time: %d red: %d green: %d blue: %d", lifetime, screen_time, r, g, b);
    EHMTX_queue *screen = this->find_free_queue_element();

    screen->text_color = Color(r, g, b);

    screen->mode = MODE_DATE;
    screen->default_font = default_font;
    screen->screen_time_ = screen_time * 1000.0;
    screen->endtime = this->get_tick() + (lifetime > 0 ? lifetime * 60000.0 : screen->screen_time_);
#ifdef EHMTXv2_USE_VERTICAL_SCROLL
    screen->pixels_ = 0;
    screen->scroll_reset = 32;
#endif
    screen->status();
  }

  void EHMTX::icon_text_screen(std::string iconname, std::string text, int lifetime, int screen_time, bool default_font, int r, int g, int b)
  {
    std::string ic = get_icon_name(iconname);
    std::string id = get_screen_id(iconname);

    uint8_t icon = this->find_icon(ic.c_str());

    if (icon == MAXICONS)
    {
      ESP_LOGW(TAG, "icon %d/%s not found => default: 0", icon, ic.c_str());
      icon = 0;
      for (auto *t : on_icon_error_triggers_)
      {
        t->process(ic);
      }
    }
    EHMTX_queue *screen = this->find_mode_icon_queue_element(MODE_ICON_TEXT_SCREEN, id);

    screen->text = text;
    screen->text_color = Color(r, g, b);
    screen->default_font = default_font;
    screen->mode = MODE_ICON_TEXT_SCREEN;
    screen->icon_name = id;
    screen->icon = icon;
    screen->calc_scroll_time(text, screen_time);
    screen->endtime = this->get_tick() + (lifetime > 0 ? lifetime * 60000.0 : screen->screen_time_);
    for (auto *t : on_add_screen_triggers_)
    {
      t->process(screen->icon_name, (uint8_t)screen->mode);
    }
    ESP_LOGD(TAG, "icon text screen icon: %d iconname: %s text: %s lifetime: %d screen_time: %d", icon, iconname.c_str(), text.c_str(), lifetime, screen_time);
    screen->status();
  }

  void EHMTX::rainbow_icon_text_screen(std::string iconname, std::string text, int lifetime, int screen_time, bool default_font)
  {
    std::string ic = get_icon_name(iconname);
    std::string id = get_screen_id(iconname);

    uint8_t icon = this->find_icon(ic.c_str());

    if (icon == MAXICONS)
    {
      ESP_LOGW(TAG, "icon %d/%s not found => default: 0", icon, ic.c_str());
      icon = 0;
      for (auto *t : on_icon_error_triggers_)
      {
        t->process(ic);
      }
    }
    EHMTX_queue *screen = this->find_mode_icon_queue_element(MODE_RAINBOW_ICON_TEXT_SCREEN, id);

    screen->text = text;
    screen->default_font = default_font;
    screen->mode = MODE_RAINBOW_ICON_TEXT_SCREEN;
    screen->icon_name = id;
    screen->icon = icon;
    screen->calc_scroll_time(text, screen_time);
    screen->endtime = this->get_tick() + (lifetime > 0 ? lifetime * 60000.0 : screen->screen_time_);
    for (auto *t : on_add_screen_triggers_)
    {
      t->process(screen->icon_name, (uint8_t)screen->mode);
    }
    ESP_LOGD(TAG, "rainbow icon text screen icon: %d iconname: %s text: %s lifetime: %d screen_time: %d", icon, iconname.c_str(), text.c_str(), lifetime, screen_time);
    screen->status();
  }

  void EHMTX::icon_prognosis_screen(std::string iconname, std::string text, std::string prognosis, int lifetime, int screen_time, bool default_font)
  {
    this->icon_prognosis_screen_rgb(iconname, text, prognosis, lifetime, screen_time, default_font, C_BLACK, C_BLACK, C_BLACK);
  }

  void EHMTX::icon_prognosis_screen_rgb(std::string iconname, std::string text, std::string prognosis, int lifetime, int screen_time, bool default_font, int r, int g, int b)
  {
    std::string ic = get_icon_name(iconname);
    std::string id = get_screen_id(iconname);

    uint8_t icon = this->find_icon(ic.c_str());

    if (icon == MAXICONS)
    {
      ESP_LOGW(TAG, "icon %d/%s not found => default: 0", icon, ic.c_str());
      icon = 0;
      for (auto *t : on_icon_error_triggers_)
      {
        t->process(ic);
      }
    }

    EHMTX_queue *screen = this->find_mode_icon_queue_element(MODE_PROGNOSIS_SCREEN, id);

    screen->text = text;
    screen->text_color = Color(r, g, b);
    screen->default_font = default_font;
    screen->mode = MODE_PROGNOSIS_SCREEN;
    screen->icon_name = id;
    screen->icon = icon;
    screen->calc_scroll_time(text, screen_time);
    screen->endtime = this->get_tick() + (lifetime > 0 ? lifetime * 60000.0 : screen->screen_time_);

    if (screen->sbitmap == NULL)
    {
      screen->sbitmap = new Color[64];
    }

    const size_t CAPACITY = JSON_ARRAY_SIZE(72);
    StaticJsonDocument<CAPACITY> doc;
    deserializeJson(doc, prognosis);
    JsonArray array = doc.as<JsonArray>();
    // extract the 24 color values
    uint8_t red = 0;
    uint8_t gre = 0;
    uint8_t blu = 0;

    uint16_t i = 0;
    for (JsonVariant v : array)
    {
      if (i % 3 == 0)
      {
        red = v.as<int>();
      }
      if (i % 3 == 1)
      {
        gre = v.as<int>();
      }
      if (i % 3 == 2)
      {
        blu = v.as<int>();

        screen->sbitmap[static_cast<uint8_t>(i / 3)] = Color(red, gre, blu);
      }

      i++;
    }

    if (r + g + b == C_BLACK)
    {
      screen->text_color = screen->sbitmap[0];
    }

    for (auto *t : on_add_screen_triggers_)
    {
      t->process(screen->icon_name, (uint8_t)screen->mode);
    }
    ESP_LOGD(TAG, "icon prognosis screen prognosis: %s", prognosis.c_str());
    ESP_LOGD(TAG, "icon prognosis screen icon: %d iconname: %s text: %s prognosis: %d lifetime: %d screen_time: %d", icon, iconname.c_str(), text.c_str(), static_cast<uint8_t>(i / 3), lifetime, screen_time);
    screen->status();
  }

  EHMTX_queue *EHMTX::find_icon_queue_element(uint8_t icon)
  {
    for (size_t i = 0; i < MAXQUEUE; i++)
    {
      if (((this->queue[i]->mode == MODE_ICON_SCREEN) ||
           (this->queue[i]->mode == MODE_RAINBOW_ICON) ||
           (this->queue[i]->mode == MODE_ICON_PROGRESS)) &&
          (this->queue[i]->icon == icon))
      {
        ESP_LOGD(TAG, "icon_screen: found by icon");
        return this->queue[i];
      }
    }
    return this->find_free_queue_element();
  }

  EHMTX_queue *EHMTX::find_free_queue_element()
  {
    float ts = this->get_tick();
    for (size_t i = 0; i < MAXQUEUE; i++)
    {
      if (this->queue[i]->endtime < ts)
      {
        ESP_LOGD(TAG, "free_screen: found by endtime %d", i);
        return this->queue[i];
      }
    }
    return this->queue[0];
  }

  EHMTX_queue *EHMTX::find_mode_queue_element(uint8_t mode)
  {
    for (size_t i = 0; i < MAXQUEUE; i++)
    {
      if (this->queue[i]->mode == mode)
      {
        ESP_LOGD(TAG, "find screen: found by mode %d", i);
        return this->queue[i];
      }
    }
    return this->find_free_queue_element();
  }

  EHMTX_queue *EHMTX::find_mode_icon_queue_element(uint8_t mode, std::string name)
  {
    for (size_t i = 0; i < MAXQUEUE; i++)
    {
      if (this->queue[i]->mode == mode && strcmp(this->queue[i]->icon_name.c_str(), name.c_str()) == 0)
      {
        ESP_LOGD(TAG, "find screen: found by mode %d icon %s", i, name);
        return this->queue[i];
      }
    }
    return this->find_free_queue_element();
  }

  void EHMTX::set_show_seconds(bool b)
  {
    this->show_seconds = b;
    ESP_LOGI(TAG, "%sshow seconds", b ? F("") : F("don't "));
  }

  void EHMTX::set_show_day_of_week(bool b)
  {
    this->show_day_of_week = b;
    ESP_LOGI(TAG, "%sshow day of week", b ? F("") : F("don't "));
  }

  void EHMTX::set_brightness(int value)
  {
    if (value < 256)
    {
      this->target_brightness_ = value;
      float br = (float)value / (float)255;
      ESP_LOGI(TAG, "set_brightness %d => %.2f %%", value, 100 * br);
    }
  }

  uint8_t EHMTX::get_brightness()
  {
    return this->brightness_;
  }

  void EHMTX::set_clock_time(uint16_t t)
  {
    this->clock_time = t;
  }

  void EHMTX::set_display(addressable_light::AddressableLightDisplay *disp)
  {
    this->display = disp;
    this->show_display = true;
    this->night_mode = false;
    this->weekday_accent = false;
    ESP_LOGD(TAG, "set_display");
  }

#ifdef USE_GRAPH
  void EHMTX::set_graph(graph::Graph *graph)
  {
    this->graph = graph;
    ESP_LOGD(TAG, "set_graph");
  }

  void EHMTX::graph_screen(int lifetime, int screen_time)
  {
    ESP_LOGD(TAG, "graph screen: lifetime: %d screen_time: %d", lifetime, screen_time);

    EHMTX_queue *screen = this->find_mode_queue_element(MODE_GRAPH_SCREEN);

    screen->mode = MODE_GRAPH_SCREEN;
    screen->icon = MAXICONS;
    screen->screen_time_ = screen_time * 1000.0;
    screen->endtime = this->get_tick() + (lifetime > 0 ? lifetime * 60000.0 : screen->screen_time_);
#ifdef EHMTXv2_USE_VERTICAL_SCROLL
    screen->pixels_ = 0;
    screen->scroll_reset = 32;
#endif

    this->graph->set_height(8);
    this->graph->set_width(32);

    for (auto *t : on_add_screen_triggers_)
    {
      t->process("graph", (uint8_t)screen->mode);
    }
    screen->status();
  }

  void EHMTX::icon_graph_screen(std::string iconname, int lifetime, int screen_time)
  {
    uint8_t icon = this->find_icon(iconname.c_str());

    if (icon == MAXICONS)
    {
      ESP_LOGW(TAG, "graph screen with icon: icon %d not found => default: 0", icon);
      for (auto *t : on_icon_error_triggers_)
      {
        t->process(iconname);
      }
      graph_screen(lifetime, screen_time);
      return;
    }

    EHMTX_queue *screen = this->find_mode_queue_element(MODE_GRAPH_SCREEN);

    screen->mode = MODE_GRAPH_SCREEN;
    screen->icon = icon;
    screen->icon_name = iconname;
    screen->screen_time_ = screen_time * 1000.0;
    screen->endtime = this->get_tick() + (lifetime > 0 ? lifetime * 60000.0 : screen->screen_time_);
#ifdef EHMTXv2_USE_VERTICAL_SCROLL
    screen->pixels_ = 0;
    screen->scroll_reset = 32;
#endif

    this->graph->set_height(8);
    this->graph->set_width(24);

    for (auto *t : on_add_screen_triggers_)
    {
      t->process("graph", (uint8_t)screen->mode);
    }
    ESP_LOGD(TAG, "graph screen with icon: icon: %d iconname: %s lifetime: %d screen_time:%d ", icon, iconname.c_str(), lifetime, screen_time);
    screen->status();
  }

#endif

  void EHMTX::set_clock(time::RealTimeClock *clock)
  {
    this->clock = clock;
    ESP_LOGD(TAG, "set_clock");
  }

#define max3(x, y, z) ((x) > (y) ? ((x) > (z) ? (x) : (z)) : ((y) > (z) ? (y) : (z)))

  void EHMTX::draw_day_of_week(int ypos, bool small)
  {
    if (this->show_day_of_week)
    {
      auto dow = this->clock->now().day_of_week - 1; // SUN = 0

      Color accent_color = esphome::display::COLOR_OFF;
      if (this->weekday_accent && this->brightness_ < 50)
      {
        uint8_t max_diff = max3(this->today_color.r, this->today_color.g, this->today_color.b) - max3(this->weekday_color.r, this->weekday_color.g, this->weekday_color.b);
        accent_color = this->weekday_color + max_diff;
      }

      if (!small)
      {
        for (uint8_t i = 0; i <= 6; i++)
        {
          if (((!EHMTXv2_WEEK_START) && (dow == i)) ||
              ((EHMTXv2_WEEK_START) && ((dow == (i + 1)) || ((dow == 0 && i == 6)))))
          {
            this->display->line(2 + i * 4, ypos + 7, i * 4 + 4, ypos + 7, this->today_color);
          }
          else
          {
            this->display->line(2 + i * 4, ypos + 7, i * 4 + 4, ypos + 7, this->weekday_color);
            if (accent_color != esphome::display::COLOR_OFF)
            {
              this->display->line(i * 4 + 3, ypos + 7, i * 4 + 3, ypos + 7, accent_color);
            }
          }
        }
      }
      else
      {
        uint8_t off_l = (this->icon_to_9 == 3) ? 11 : 10;
        uint8_t off_r = (this->icon_to_9 == 3) ? 12 : 11;
        for (uint8_t i = 0; i <= 6; i++)
        {
          if (((!EHMTXv2_WEEK_START) && (dow == i)) ||
              ((EHMTXv2_WEEK_START) && ((dow == (i + 1)) || ((dow == 0 && i == 6)))))
          {
            this->display->line(off_l + i * 3, ypos + 7, off_r + i * 3, ypos + 7, this->today_color);
          }
          else
          {
            this->display->line(off_l + i * 3, ypos + 7, off_r + i * 3, ypos + 7, this->weekday_color);
            if (accent_color != esphome::display::COLOR_OFF)
            {
              this->display->line((i < dow ? off_r : off_l) + i * 3, ypos + 7, (i < dow ? off_r : off_l) + i * 3, ypos + 7, accent_color);
            }
          }
        }
      }
    }
  }

#ifdef EHMTXv2_ADV_CLOCK
  void EHMTX::set_clock_infotext_color(int lr, int lg, int lb, int rr, int rg, int rb, bool df, int y_offset)
  {
    this->info_clock_lcolor = Color((uint8_t)lr, (uint8_t)lg, (uint8_t)lb);
    this->info_clock_rcolor = Color((uint8_t)rr, (uint8_t)rg, (uint8_t)rb);
    this->info_clock_font = df;
    this->info_clock_y_offset = y_offset;
    ESP_LOGD(TAG, "info clock text color left: r: %d g: %d b: %d right: r: %d g: %d b: %d y_offset %d", lr, lg, lb, rr, rg, rb, y_offset);
  }

  void EHMTX::set_adv_clock_color(int hr, int hg, int hb, int mr, int mg, int mb, int sr, int sg, int sb)
  {
    this->hour_color = Color((uint8_t)hr, (uint8_t)hg, (uint8_t)hb);
    this->minutes_color = Color((uint8_t)mr, (uint8_t)mg, (uint8_t)mb);
    this->spacer_color = Color((uint8_t)sr, (uint8_t)sg, (uint8_t)sb);
    ESP_LOGD(TAG, "advanced clock color hour: r: %d g: %d b: %d minutes: r: %d g: %d b: %d spacer: r: %d g: %d b: %d", hr, hg, hb, mr, mg, mb, sr, sg, sb);
  }

  bool EHMTX::draw_clock(std::string format, esphome::display::BaseFont *font, Color color, int xpos, int ypos)
  {
    std::regex rgx{" "};
    std::smatch match;
    if (!std::regex_search(format, match, rgx))
      return false;

    std::vector<std::string> parts;
    std::vector<uint8_t> len;
    std::string sep = "";

    uint8_t full_length = 0;

    for (int i = 1; i < match.length(); i++)
    {
      std::string output = match[i].str();

      if (output.length() > 0)
      {
        if (output.find("%") != std::string::npos)
        {
          if (output == "%p" && this->replace_time_date_active) // check for replace active
          {
            output = this->clock->now().strftime(output);
            output = this->replace_time_date(output);
          }
          else
          {
            output = this->clock->now().strftime(output);
          }
        }
        else if (sep == "")
        {
          sep = output;
        }

        parts.push_back(output);
        len.push_back(output.length() > 0 ? this->GetTextWidth(font, "%s", output.c_str()) : 0);
        full_length += len.back();
      }
    }

    uint8_t x = xpos - full_length / 2;
    for (int i = 0; i < parts.size(); i++)
    {
      if (parts.at(i).length() > 0)
      {
        if (!(this->show_seconds && parts.at(i) == sep && (this->clock->now().second % 2 == 1)))
        {
          Color c_ = i == 0 ? this->hour_color : i == 2           ? this->minutes_color
                                             : parts.at(i) == sep ? this->spacer_color
                                                                  : color;
          if (c_.r + c_.g + c_.b == C_BLACK)
          {
            c_ = color;
          }

          this->display->printf(x, ypos, font, c_, display::TextAlign::BASELINE_LEFT, "%s", parts.at(i).c_str());
        }
        x += len.at(i);
      }
    }

    return true;
  }
#endif

  void EHMTX::set_weekday_char_count(uint8_t i)
  {
    this->weekday_char_count = i;
  }

  std::string EHMTX::GetWeekdayChar(int position)
  {
    std::string weekday_char = "";
    int pos = 0;

    for (int i = 0; i < strlen(EHMTXv2_WEEKDAYTEXT);)
    {
      weekday_char = weekday_char + EHMTXv2_WEEKDAYTEXT[i];
      if (EHMTXv2_WEEKDAYTEXT[i] & 0x80)
      {
        weekday_char = weekday_char + EHMTXv2_WEEKDAYTEXT[i + 1];
        if (EHMTXv2_WEEKDAYTEXT[i] & 0x20)
        {
          weekday_char = weekday_char + EHMTXv2_WEEKDAYTEXT[i + 2];
          if (EHMTXv2_WEEKDAYTEXT[i] & 0x10)
          {
            weekday_char = weekday_char + EHMTXv2_WEEKDAYTEXT[i + 3];
            i += 4;
          }
          else
          {
            i += 3;
          }
        }
        else
        {
          i += 2;
        }
      }
      else
      {
        i += 1;
      }

      if (pos == position)
      {
        return weekday_char;
      }
      weekday_char = "";
      pos++;
    }

    return "";
  }

  int EHMTX::GetTextBounds(esphome::display::BaseFont *font, const char *buffer)
  {
    int x = 0;      // A pointer to store the returned x coordinate of the upper left corner in.
    int y = 0;      // A pointer to store the returned y coordinate of the upper left corner in.
    int width = 0;  // A pointer to store the returned text width in.
    int height = 0; // A pointer to store the returned text height in.
    this->display->get_text_bounds(0, 0, buffer, font, esphome::display::TextAlign::TOP_LEFT, &x, &y, &width, &height);
    return width;
  }

  int EHMTX::GetTextWidth(esphome::display::BaseFont *font, const char *formatting, const char raw_char)
  {
    char temp_buffer[80];
    sprintf(temp_buffer, formatting, raw_char);
    return GetTextBounds(font, temp_buffer);
  }

  int EHMTX::GetTextWidth(esphome::display::BaseFont *font, const char *formatting, const char *raw_text)
  {
    char temp_buffer[80];
    sprintf(temp_buffer, formatting, raw_text);
    return GetTextBounds(font, temp_buffer);
  }

  int EHMTX::GetTextWidth(esphome::display::BaseFont *font, const char *formatting, const int raw_int)
  {
    char temp_buffer[80];
    sprintf(temp_buffer, formatting, raw_int);
    return GetTextBounds(font, temp_buffer);
  }

  int EHMTX::GetTextWidth(esphome::display::BaseFont *font, const char *formatting, const float raw_float)
  {
    char temp_buffer[80];
    sprintf(temp_buffer, formatting, raw_float);
    return GetTextBounds(font, temp_buffer);
  }

  int EHMTX::GetTextWidth(esphome::display::BaseFont *font, const char *formatting, esphome::ESPTime time)
  {
    auto c_tm = time.to_c_tm();
    size_t buffer_length = 80;
    char temp_buffer[buffer_length];
    strftime(temp_buffer, buffer_length, formatting, &c_tm);
    return GetTextBounds(font, temp_buffer);
  }

  void EHMTX::dump_config()
  {
    ESP_LOGCONFIG(TAG, "EspHoMatriXv2 version: %s", EHMTX_VERSION);
    ESP_LOGCONFIG(TAG, "Icons: %d of %d", this->icon_count, MAXICONS);
    ESP_LOGCONFIG(TAG, "Clock interval: %d s", EHMTXv2_CLOCK_INTERVALL);
    ESP_LOGCONFIG(TAG, "Date format: %s", EHMTXv2_DATE_FORMAT);
    ESP_LOGCONFIG(TAG, "Time format: %s", EHMTXv2_TIME_FORMAT);
    ESP_LOGCONFIG(TAG, "Interval (ms) scroll: %d", EHMTXv2_SCROLL_INTERVALL);
    if (this->show_day_of_week)
    {
      ESP_LOGCONFIG(TAG, "Show day of week");
    }
#ifdef EHMTXv2_USE_RTL
    ESP_LOGCONFIG(TAG, "RTL activated");
#endif
#ifdef EHMTXv2_BLEND_STEPS
    ESP_LOGCONFIG(TAG, "Fade in activated: %d steps", EHMTXv2_BLEND_STEPS);
#endif
    ESP_LOGCONFIG(TAG, "Weekstart: %s", EHMTXv2_WEEK_START ? F("Monday") : F("Sunday"));
    ESP_LOGCONFIG(TAG, "Weekdays: %s Count: %d", EHMTXv2_WEEKDAYTEXT, this->weekday_char_count);
    ESP_LOGCONFIG(TAG, "Display: %s", this->show_display ? F("On") : F("Off"));
    ESP_LOGCONFIG(TAG, "Night mode: %s", this->night_mode ? F("On") : F("Off"));
    ESP_LOGCONFIG(TAG, "Weekday accent: %s", this->weekday_accent ? F("On") : F("Off"));
    ESP_LOGCONFIG(TAG, "Replace Time and Date: %s", this->replace_time_date_active ? F("On") : F("Off"));
    if (this->replace_time_date_active)
    {
      ESP_LOGCONFIG(TAG, "Replace from: %s", EHMTXv2_REPLACE_TIME_DATE_FROM);
      ESP_LOGCONFIG(TAG, "Replace to  : %s", EHMTXv2_REPLACE_TIME_DATE_TO);
    }
  }

  void EHMTX::add_icon(EHMTX_Icon *icon)
  {
    this->icons[this->icon_count] = icon;
    ESP_LOGD(TAG, "add_icon no.: %d name: %s frame_duration: %d ms", this->icon_count, icon->name.c_str(), icon->frame_duration);
    this->icon_count++;
  }

  void EHMTX::draw_alarm()
  {
    if (this->display_alarm > 2)
    {
      this->display->line(31, 2, 29, 0, this->alarm_color);
    }
    if (this->display_alarm > 1)
    {
      this->display->draw_pixel_at(30, 0, this->alarm_color);
      this->display->draw_pixel_at(31, 1, this->alarm_color);
    }
    if (this->display_alarm > 0)
    {
      this->display->draw_pixel_at(31, 0, this->alarm_color);
    }
  }

  void EHMTX::draw_rindicator()
  {
    if (this->display_rindicator > 2)
    {
      this->display->line(31, 5, 29, 7, this->rindicator_color);
    }

    if (this->display_rindicator > 1)
    {
      this->display->draw_pixel_at(30, 7, this->rindicator_color);
      this->display->draw_pixel_at(31, 6, this->rindicator_color);
    }

    if (this->display_rindicator > 0)
    {
      this->display->draw_pixel_at(31, 7, this->rindicator_color);
    }
  }

  void EHMTX::draw_lindicator()
  {
    if (this->display_lindicator > 2)
    {
      this->display->line(0, 5, 2, 7, this->lindicator_color);
    }

    if (this->display_lindicator > 1)
    {
      this->display->draw_pixel_at(1, 7, this->lindicator_color);
      this->display->draw_pixel_at(0, 6, this->lindicator_color);
    }

    if (this->display_lindicator > 0)
    {
      this->display->draw_pixel_at(0, 7, this->lindicator_color);
    }
  }

  void EHMTX::draw_icon_indicator()
  {
    if (this->display_icon_indicator > 0)
    {
      for (auto id : EHMTXv2_CONF_ICON_INDICATOR_SCREENS)
      {
        if (this->queue[this->screen_pointer]->mode == id)
        {
          if ((this->queue[this->screen_pointer]->mode == MODE_ICON_CLOCK && this->icon_to_9 == 1) ||
              (this->queue[this->screen_pointer]->mode == MODE_ICON_DATE && this->icon_to_9 == 2) ||
              (this->icon_to_9 == 3))
          {
            if (this->icon_indicator_height == 1)
            {
              this->display->line(4 - display_icon_indicator / 2, this->icon_indicator_y_pos,
                                  4 + display_icon_indicator / 2, this->icon_indicator_y_pos,
                                  this->icon_indicator_color);
            }
            else
            {
              this->display->filled_rectangle(4 - display_icon_indicator / 2, this->icon_indicator_y_pos,
                                              this->display_icon_indicator, this->icon_indicator_height,
                                              this->icon_indicator_color);
            }
          }
          else
          {
            if (this->icon_indicator_height == 1)
            {
              this->display->line(4 - display_icon_indicator / 2, this->icon_indicator_y_pos,
                                  3 + display_icon_indicator / 2, this->icon_indicator_y_pos,
                                  this->icon_indicator_color);
            }
            else
            {
              this->display->filled_rectangle(4 - display_icon_indicator / 2, this->icon_indicator_y_pos,
                                              this->display_icon_indicator, this->icon_indicator_height,
                                              this->icon_indicator_color);
            }
          }
          break;
        }
      }
    }
  }

  void HOT EHMTX::draw()
  {
    if ((this->is_running) && (this->show_display))
    {
      if (this->screen_pointer != MAXQUEUE)
      {
        this->queue[this->screen_pointer]->draw();
      }
      if (this->queue[this->screen_pointer]->mode != MODE_FULL_SCREEN &&
          this->queue[this->screen_pointer]->mode != MODE_BITMAP_SCREEN &&
          this->queue[this->screen_pointer]->mode != MODE_ICON_PROGRESS &&
          this->queue[this->screen_pointer]->mode != MODE_TEXT_PROGRESS)
      {
        this->draw_gauge();
      }
#ifndef EHMTXv2_ALWAYS_SHOW_RLINDICATORS
      if (this->queue[this->screen_pointer]->mode != MODE_CLOCK &&
          this->queue[this->screen_pointer]->mode != MODE_DATE &&
          this->queue[this->screen_pointer]->mode != MODE_FULL_SCREEN &&
          this->queue[this->screen_pointer]->mode != MODE_BITMAP_SCREEN)
      {
#endif
        this->draw_rindicator();

#ifndef EHMTXv2_ALWAYS_SHOW_RLINDICATORS
        if (this->queue[this->screen_pointer]->mode != MODE_ICON_SCREEN &&
            this->queue[this->screen_pointer]->mode != MODE_RAINBOW_ICON &&
            this->queue[this->screen_pointer]->mode != MODE_PROGNOSIS_SCREEN &&
            !this->display_gauge)
        {
#endif
          this->draw_lindicator();
#ifndef EHMTXv2_ALWAYS_SHOW_RLINDICATORS
        }
      }
#endif

      this->draw_icon_indicator();
      this->draw_alarm();
    }
  }

  void EHMTXStartRunningTrigger::process()
  {
    this->trigger();
  }

  void EHMTXEmptyQueueTrigger::process()
  {
    this->trigger();
  }

  void EHMTXNextScreenTrigger::process(std::string iconname, std::string text)
  {
    this->trigger(iconname, text);
  }

  void EHMTXAddScreenTrigger::process(std::string iconname, uint8_t mode)
  {
    this->trigger(iconname, mode);
  }

  void EHMTXIconErrorTrigger::process(std::string iconname)
  {
    this->trigger(iconname);
  }

  void EHMTXExpiredScreenTrigger::process(std::string iconname, std::string text)
  {
    this->trigger(iconname, text);
  }

  void EHMTXNextClockTrigger::process()
  {
    this->trigger();
  }

  void EHMTXShowDisplayTrigger::process(bool state)
  {
    this->trigger(state);
  }

  void EHMTXNightModeTrigger::process(bool state)
  {
    this->trigger(state);
  }
}
