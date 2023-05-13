#include "esphome.h"

namespace esphome
{
  EHMTX::EHMTX() : PollingComponent(POLLINGINTERVAL)
  {
    this->show_display = true;
    this->display_gauge = false;
    this->display_indicator = 0;
    this->display_alarm = 0;
    this->clock_time = 10;
    this->hold_time = 10;
    this->icon_count = 0;
    this->hue_ = 0;
    this->text_color = Color(C_RED, C_GREEN, C_BLUE);
    this->today_color = Color(C_RED, C_GREEN, C_BLUE);
    this->weekday_color = Color(CD_RED, CD_GREEN, CD_BLUE);
    this->clock_color = Color(C_RED, C_GREEN, C_BLUE);
    this->rainbow_color = Color(CA_RED, CA_GREEN, CA_BLUE);
    this->alarm_color = Color(CA_RED, CA_GREEN, CA_BLUE);
    this->gauge_color = Color(CD_RED, CD_GREEN, CD_BLUE);
    this->gauge_value = 0;
    this->screen_pointer = 0;

    for (uint8_t i = 0; i < MAXQUEUE; i++)
    {
      this->queue[i] = new EHMTX_queue(this);
    }
  }

  void EHMTX::set_time_format(std::string s)
  {
    this->time_fmt = s;
  }

  void EHMTX::set_date_format(std::string s)
  {
    this->date_fmt = s;
  }

  void EHMTX::show_indicator(int r, int g, int b, int size)
  {
    if (size > 0 ) {
      this->indicator_color = Color((uint8_t)r & 248, (uint8_t)g & 252, (uint8_t)b & 248);
    this->display_indicator = size & 3;
    ESP_LOGD(TAG, "show indicator size: %d r: %d g: %d b: %d", size, r, g, b);
    } else {
      this->hide_indicator();
    }
    
  }

  void EHMTX::hide_indicator()
  {
    this->display_indicator = 0;
    ESP_LOGD(TAG, "hide indicator");
  }

  void EHMTX::set_display_off()
  {
    this->show_display = false;
    ESP_LOGD(TAG, "display off");
  }

  void EHMTX::set_display_on()
  {
    this->show_display = true;
    ESP_LOGD(TAG, "display on");
  }

  void EHMTX::set_today_color(int r, int g, int b)
  {
    this->today_color = Color((uint8_t)r & 248, (uint8_t)g & 252, (uint8_t)b & 248);
    ESP_LOGD(TAG, "default today color r: %d g: %d b: %d", r, g, b);
  }

  void EHMTX::set_weekday_color(int r, int g, int b)
  {
    this->weekday_color = Color((uint8_t)r & 248, (uint8_t)g & 252, (uint8_t)b & 248);
    ESP_LOGD(TAG, "default weekday color: %d g: %d b: %d", r, g, b);
  }

  void EHMTX::set_clock_color(int r, int g, int b)
  {
    this->clock_color = Color((uint8_t)r & 248, (uint8_t)g & 252, (uint8_t)b & 248);
    ESP_LOGD(TAG, "default clock color r: %d g: %d b: %d", r, g, b);
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

  uint8_t EHMTX::find_icon(std::string name)
  {
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

  void EHMTX::show_gauge(int percent, int r, int g, int b)
  {
    this->display_gauge = false;
    if (percent <= 100)
    {
      this->gauge_color = Color(r, g, b);
      this->display_gauge = true;
      this->gauge_value = (uint8_t)(100 - percent) * 7 / 100;
    }
  }

  void EHMTX::draw_gauge()
  {
    if (this->display_gauge)
    {
      this->display->line(0, 7, 0, 0, esphome::display::COLOR_OFF);
      this->display->line(1, 7, 1, 0, esphome::display::COLOR_OFF);
      this->display->line(0, 7, 0, this->gauge_value, this->gauge_color);
    }
  }

  void EHMTX::setup()
  {
    ESP_LOGD(TAG, "Setting up services");
    register_service(&EHMTX::get_status, "get_status");
    register_service(&EHMTX::set_display_on, "display_on");
    register_service(&EHMTX::set_display_off, "display_off");
    register_service(&EHMTX::hold_screen, "hold_screen", {"time"});
    register_service(&EHMTX::hide_indicator, "hide_indicator");
    register_service(&EHMTX::hide_gauge, "hide_gauge");
    register_service(&EHMTX::hide_alarm, "hide_alarm");
    register_service(&EHMTX::show_gauge, "show_gauge", {"percent", "r", "g", "b"});
    register_service(&EHMTX::show_alarm, "show_alarm", {"r", "g", "b", "size"});
    register_service(&EHMTX::show_indicator, "show_indicator", {"r", "g", "b", "size"});

    register_service(&EHMTX::set_clock_color, "set_clock_color", {"r", "g", "b"});
    register_service(&EHMTX::set_today_color, "set_today_color", {"r", "g", "b"});
    register_service(&EHMTX::set_weekday_color, "set_weekday_color", {"r", "g", "b"});

    register_service(&EHMTX::del_screen, "del_screen", {"icon_name", "mode"});
    register_service(&EHMTX::force_screen, "force_screen", {"icon_name", "mode"});

    register_service(&EHMTX::full_screen, "full_screen", {"icon_name", "lifetime", "screen_time"});
    register_service(&EHMTX::icon_screen, "icon_screen", {"icon_name", "text", "lifetime", "screen_time", "default_font", "r", "g", "b"});
    register_service(&EHMTX::rainbow_icon_screen, "rainbow_icon_screen", {"icon_name", "text", "lifetime", "screen_time", "default_font"});

    register_service(&EHMTX::text_screen, "text_screen", {"text", "lifetime", "screen_time", "default_font", "r", "g", "b"});
    register_service(&EHMTX::rainbow_text_screen, "rainbow_text_screen", {"text", "lifetime", "screen_time", "default_font"});

    register_service(&EHMTX::clock_screen, "clock_screen", {"lifetime", "screen_time", "default_font", "r", "g", "b"});
    register_service(&EHMTX::rainbow_clock_screen, "rainbow_clock_screen", {"lifetime", "screen_time", "default_font"});
    
    register_service(&EHMTX::date_screen, "date_screen", {"lifetime", "screen_time", "default_font", "r", "g", "b"});
    register_service(&EHMTX::rainbow_date_screen, "rainbow_date_screen", {"lifetime", "screen_time", "default_font"});

    register_service(&EHMTX::blank_screen, "blank_screen", {"lifetime", "screen_time"});


    register_service(&EHMTX::set_brightness, "brightness", {"value"});
    ESP_LOGD(TAG, "Setup and running!");
  }

  void EHMTX::show_alarm(int r, int g, int b, int size)
  {
    if (size > 0){
      this->alarm_color = Color((uint8_t)r & 248, (uint8_t)g & 252, (uint8_t)b & 248);
    this->display_alarm = size & 3;
    ESP_LOGD(TAG, "show alarm size: %d color r: %d g: %d b: %d", size, r, g, b);
    } else {
      this->hide_alarm();
    }
  }

  void EHMTX::hide_alarm()
  {
    this->display_alarm = 0;
    ESP_LOGD(TAG, "hide alarm");
  }

  void EHMTX::blank_screen(int lifetime, int showtime)
  {
    auto scr = this->find_free_queue_element();
    scr->screen_time = showtime;
    scr->mode = MODE_BLANK;
    scr->endtime = this->clock->now().timestamp + lifetime * 60;
  }

  void EHMTX::update() // called from polling component
  {
    if (!this->is_running){
      if (this->clock->now().timestamp > 15) {
        ESP_LOGD(TAG, "time sync => starting");
        this->is_running = true;
      }
    }
  }

  void EHMTX::force_screen(std::string icon_name, int mode)
  {
    // if (this->string_has_ending(icon_name, "*"))
    // {
    //   // remove the *
    //   icon_name = icon_name.substr(0, icon_name.length() - 1);
    // }

    for (uint8_t i = 0; i < MAXQUEUE; i++)
    {
      if (this->queue[i]->mode == mode)
      {
        bool force = true;
        if ((mode == MODE_ICON_SCREEN) || (mode == MODE_FULL_SCREEN) || (mode == MODE_RAINBOW_ICON))
        {
          if (strcmp(this->queue[i]->icon_name.c_str(), icon_name.c_str()) != 0)
          {
            force = false;
          }
          if (force)
          {
            ESP_LOGD(TAG, "force_screen: found position: %d", i);
            this->queue[i]->last_time = 0;
            this->queue[i]->endtime += this->queue[i]->screen_time;
            this->next_action_time = this->clock->now().timestamp;
            ESP_LOGW(TAG, "force_screen: icon %s in mode %d", icon_name.c_str(), mode);
          }
        }
      }
    }
  }
  uint8_t EHMTX::find_oldest_queue_element()
  {
    uint8_t hit = MAXQUEUE;
    time_t last_time = this->clock->now().timestamp;
    for (size_t i = 0; i < MAXQUEUE; i++)
    {
      if ((this->queue[i]->endtime > 0) && (this->queue[i]->last_time < last_time))
      {
        hit = i;
        last_time = this->queue[i]->last_time;
      }
    }
    if (hit != MAXQUEUE)
    {
      this->queue[hit]->status();
    }
    return hit;
  }

  void EHMTX::remove_expired_queue_element()
  {
    time_t ts = this->clock->now().timestamp;
    std::string infotext;
    for (size_t i = 0; i < MAXQUEUE; i++)
    {
      if ((this->queue[i]->endtime > 0) && (this->queue[i]->endtime < ts))
      {
        this->queue[i]->endtime = 0;
        if (this->queue[i]->mode != MODE_EMPTY)
        {
          ESP_LOGD(TAG, "remove expired queue element: removed slot %d: icon_name: %s text: %s", i, this->queue[i]->icon_name.c_str(), this->queue[i]->text.c_str());
          for (auto *t : on_expired_screen_triggers_)
          {
            infotext = "";
            switch (this->queue[i]->mode)
            {
            case MODE_EMPTY:
              break;
            case MODE_BLANK:
              break;
            case MODE_CLOCK:
              infotext = "clock";
              break;
            case MODE_DATE:
              infotext = "clock";
              break;
            case MODE_FULL_SCREEN:
              infotext = "full screen " + this->queue[i]->icon_name;
              break;
            case MODE_ICON_SCREEN:
            case MODE_RAINBOW_ICON:
              infotext = this->queue[i]->icon_name.c_str();
              break;
            case MODE_RAINBOW_TEXT:
            case MODE_TEXT_SCREEN:
              infotext = "TEXT";
              break;
            default:
              break;
            }
            t->process(this->queue[i]->icon_name, infotext);
          }
        }
        this->queue[i]->mode = MODE_EMPTY;
      }
    }
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
    time_t ts = this->clock->now().timestamp;

    if (this->is_running)
    { 
      if (ts > this->next_action_time)
      {
        this->remove_expired_queue_element();
        this->screen_pointer = find_oldest_queue_element();
        if (this->screen_pointer != MAXQUEUE)
        {
          this->queue[this->screen_pointer]->shiftx_ = 0;
          this->queue[this->screen_pointer]->last_time = ts + this->queue[this->screen_pointer]->screen_time;
          if (this->queue[this->screen_pointer]->icon < this->icon_count)
          {
            this->icons[this->queue[this->screen_pointer]->icon]->set_frame(0);
          }
          this->next_action_time = this->queue[this->screen_pointer]->last_time;
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
              t->process(this->queue[this->screen_pointer]->icon_name, this->queue[this->screen_pointer]->text);
            }
          }
        }
        else
        {
          ESP_LOGW(TAG, "tick: nothing to do. Restarting clock display!");
          this->clock_screen(24 * 60, this->clock_time, false, C_RED, C_GREEN, C_BLUE);
          this->date_screen(24 * 60, (int)this->clock_time / 2, false, C_RED, C_GREEN, C_BLUE);
          this->next_action_time = ts + this->clock_time;
        }
      }
    }
    else
    {
      uint8_t w = (1+(uint8_t)(32 / 16) * (this->boot_anim / 16)) % 32;
      this->display->rectangle(0, 2, w, 4, this->rainbow_color); // Color(120, 190, 40));
      this->boot_anim++;
    }
  }

  void EHMTX::skip_screen()
  {
    this->next_action_time = this->clock->now().timestamp - 1;
  }

  void EHMTX::hold_screen(int time)
  {
    this->next_action_time += this->hold_time;
    this->hold_time = time;
  }

  void EHMTX::get_status()
  {
    time_t ts = this->clock->now().timestamp;
    ESP_LOGI(TAG, "status time: %d.%d.%d %02d:%02d", this->clock->now().day_of_month,
             this->clock->now().month, this->clock->now().year,
             this->clock->now().hour, this->clock->now().minute);
    ESP_LOGI(TAG, "status brightness: %d (0..255)", this->brightness_);
    ESP_LOGI(TAG, "status date format: %s", this->date_fmt.c_str());
    ESP_LOGI(TAG, "status time format: %s", this->time_fmt.c_str());
    ESP_LOGI(TAG, "status alarm_color: RGB(%d,%d,%d)", this->alarm_color.r, this->alarm_color.g, this->alarm_color.b);
    if (this->show_display)
    {
      ESP_LOGI(TAG, "status display on");
    }
    else
    {
      ESP_LOGI(TAG, "status display off");
    }

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

  void EHMTX::set_default_font(display::Font *font)
  {
    this->default_font = font;
  }

  void EHMTX::set_special_font(display::Font *font)
  {
    this->special_font = font;
  }

  void EHMTX::set_frame_interval(uint16_t fi)
  {
    this->frame_interval = fi;
  }

  void EHMTX::set_scroll_interval(uint16_t si)
  {
    this->scroll_interval = si;
  }

  void EHMTX::set_rainbow_interval(uint16_t si)
  {
    this->rainbow_interval = si;
  }

  void EHMTX::del_screen(std::string icon_name, int mode)
  {
    for (uint8_t i = 0; i < MAXQUEUE; i++)
    {
      if (this->queue[i]->mode == mode)
      {
        bool force = true;
        ESP_LOGW(TAG, "del_screen: icon %s in position: %d mode %d", icon_name.c_str(), i, mode);
        if ((mode == MODE_ICON_SCREEN) || (mode == MODE_FULL_SCREEN) || (mode == MODE_RAINBOW_ICON))
        {
          if (strcmp(this->queue[i]->icon_name.c_str(), icon_name.c_str()) != 0)
          {
            force = false;
          }
        }
        if (force)
        {
          ESP_LOGW(TAG, "del_screen: force");
          this->queue[i]->mode = MODE_EMPTY;
          this->queue[i]->endtime = 0;
          if (i == this->screen_pointer)
          {
            this->next_action_time = this->clock->now().timestamp;
          }
        }
      }
    }
  }

  void EHMTX::icon_screen(std::string iconname, std::string text, int lifetime, int screen_time, bool default_font, int r, int g, int b)
  {
    uint8_t icon = this->find_icon(iconname.c_str());

    if (icon >= this->icon_count)
    {
      ESP_LOGW(TAG, "icon %d not found => default: 0", icon);
      icon = 0;
      for (auto *t : on_icon_error_triggers_)
      {
        t->process(iconname);
      }
    }
    EHMTX_queue *screen = this->find_icon_queue_element(icon);

    int x, y, w, h;
    if (default_font)
    {
      this->display->get_text_bounds(0, 0, text.c_str(), this->default_font, display::TextAlign::LEFT, &x, &y, &w, &h);
    }
    else
    {
      this->display->get_text_bounds(0, 0, text.c_str(), this->special_font, display::TextAlign::LEFT, &x, &y, &w, &h);
    }
    screen->set_text(text, icon, w, lifetime, screen_time);
    screen->text_color = Color(r, g, b);
    screen->default_font = default_font;
    screen->mode = MODE_ICON_SCREEN;
    screen->icon_name = iconname;
    for (auto *t : on_add_screen_triggers_)
    {
       t->process(screen->icon_name,(uint8_t)screen->mode);
    }
    ESP_LOGD(TAG, "icon screen icon: %d iconname: %s text: %s lifetime: %d screen_time: %d", icon, iconname.c_str(), text.c_str(), lifetime, screen_time);
    screen->status();
  }

  void EHMTX::rainbow_clock_screen(int lifetime, int screen_time, bool default_font)
  {
    EHMTX_queue *screen = this->find_free_queue_element();

    ESP_LOGD(TAG, "rainbow_clock_screen lifetime: %d screen_time: %d", lifetime, screen_time);
    screen->mode = MODE_RAINBOW_CLOCK;
    screen->default_font = default_font;
    screen->screen_time = screen_time;
    screen->endtime = this->clock->now().timestamp + lifetime * 60;
    screen->status();
  }

  void EHMTX::rainbow_date_screen(int lifetime, int screen_time, bool default_font)
  {
    EHMTX_queue *screen = this->find_free_queue_element();

    ESP_LOGD(TAG, "rainbow_date_screen lifetime: %d screen_time: %d", lifetime, screen_time);
    screen->mode = MODE_RAINBOW_DATE;
    screen->default_font = default_font;
    screen->screen_time = screen_time;
    screen->endtime = this->clock->now().timestamp + lifetime * 60;
    screen->status();
  }



  void EHMTX::rainbow_icon_screen(std::string iconname, std::string text, int lifetime, int screen_time, bool default_font)
  {
    uint8_t icon = this->find_icon(iconname.c_str());

    if (icon >= this->icon_count)
    {
      ESP_LOGW(TAG, "icon %d not found => default: 0", icon);
      icon = 0;
      for (auto *t : on_icon_error_triggers_)
      {
        t->process(iconname);
      }
    }
    EHMTX_queue *screen = this->find_icon_queue_element(icon);
    screen->icon_name = iconname;
    screen->icon = icon;
    screen->text = text;
    screen->endtime = this->clock->now().timestamp + lifetime * 60;
    screen->screen_time = screen_time;
    screen->default_font = default_font;  
    screen->mode = MODE_RAINBOW_ICON;
    screen->calc_scroll_time();
    for (auto *t : on_add_screen_triggers_)
    {
       t->process(screen->icon_name,(uint8_t)screen->mode);
    }
    ESP_LOGD(TAG, "rainbow_icon_screen icon: %d iconname: %s text: %s lifetime: %d screen_time: %d", icon, iconname.c_str(), text.c_str(), lifetime, screen_time);
    screen->status();
  }

  void EHMTX::text_screen(std::string text, int lifetime, int screen_time, bool default_font, int r, int g, int b)
  {
    EHMTX_queue *screen = this->find_free_queue_element();

    screen->text = text;
    screen->endtime = this->clock->now().timestamp + lifetime * 60;
    screen->screen_time = screen_time;
    screen->default_font = default_font;
    screen->text_color = Color(r, g, b);
    screen->mode = MODE_TEXT_SCREEN;
    screen->calc_scroll_time();
    screen->status();
  }

  void EHMTX::rainbow_text_screen(std::string text, int lifetime, int screen_time, bool default_font)
  {
    EHMTX_queue *screen = this->find_free_queue_element();

    screen->text = text;
    screen->endtime = this->clock->now().timestamp + lifetime * 60;
    screen->screen_time = screen_time;    
    screen->default_font = default_font;
    screen->mode = MODE_RAINBOW_TEXT;
    screen->calc_scroll_time();
    screen->status();
  }

  void EHMTX::full_screen(std::string iconname, int lifetime, int screen_time)
  {
    uint8_t icon = this->find_icon(iconname.c_str());

    if (icon >= this->icon_count)
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
    screen->screen_time = screen_time;
    screen->endtime = this->clock->now().timestamp + lifetime * 60;
    for (auto *t : on_add_screen_triggers_)
    {
       t->process(screen->icon_name,(uint8_t)screen->mode);
    }
    ESP_LOGD(TAG, "full screen: icon: %d iconname: %s lifetime: %d screen_time:%d ", icon, iconname.c_str(), lifetime, screen_time);
    screen->status();
  }

  void EHMTX::clock_screen(int lifetime, int screen_time, bool default_font, int r, int g, int b)
  {
    EHMTX_queue *screen = this->find_free_queue_element();

    screen->text_color = Color(r, g, b);
    ESP_LOGD(TAG, "clock_screen_color lifetime: %d screen_time: %d red: %d green: %d blue: %d", lifetime, screen_time, r, g, b);
    screen->mode = MODE_CLOCK;
    screen->default_font = default_font;
    screen->screen_time = screen_time;
    screen->endtime = this->clock->now().timestamp + lifetime * 60;
    screen->status();
  }

  void EHMTX::date_screen(int lifetime, int screen_time, bool default_font, int r, int g, int b)
  {
    EHMTX_queue *screen = this->find_free_queue_element();

    screen->text_color = Color(r, g, b);
    ESP_LOGD(TAG, "date_screen lifetime: %d screen_time: %d red: %d green: %d blue: %d", lifetime, screen_time, r, g, b);
    screen->mode = MODE_DATE;
    screen->screen_time = screen_time;
    screen->default_font = default_font;
    screen->endtime = this->clock->now().timestamp + lifetime * 60;
    screen->status();
  }

  EHMTX_queue *EHMTX::find_icon_queue_element(uint8_t icon)
  {
    for (size_t i = 0; i < MAXQUEUE; i++)
    {
      if ((this->queue[i]->mode == MODE_ICON_SCREEN) && (this->queue[i]->icon == icon))
      {
        ESP_LOGD(TAG, "free_screen: found by icon");
        return this->queue[i];
      }
    }
    return this->find_free_queue_element();
  }

  EHMTX_queue *EHMTX::find_free_queue_element()
  {
    time_t ts = this->clock->now().timestamp;
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

  void EHMTX::set_show_date(bool b)
  {
    this->show_date = b;
    if (b)
    {
      ESP_LOGI(TAG, "show date");
    }
    else
    {
      ESP_LOGI(TAG, "don't show date");
    }
  }

  void EHMTX::set_show_seconds(bool b)
  {
    this->show_seconds = b;
    if (b)
    {
      ESP_LOGI(TAG, "show seconds");
    }
    else
    {
      ESP_LOGI(TAG, "don't show seconds");
    }
  }

  void EHMTX::set_show_day_of_week(bool b)
  {
    this->show_day_of_week = b;
    if (b)
    {
      ESP_LOGI(TAG, "show day of week");
    }
    else
    {
      ESP_LOGI(TAG, "don't show day of week");
    }
  }

  void EHMTX::set_week_start(bool b)
  {
    this->week_starts_monday = b;
    if (b)
    {
      ESP_LOGI(TAG, "weekstart: monday");
    }
    else
    {
      ESP_LOGI(TAG, "weekstart: sunday");
    }
  }

  void EHMTX::set_brightness(int value)
  {
    if (value < 256)
    {
      this->brightness_ = value;
      float br = (float)value / (float)255;
      ESP_LOGI(TAG, "set_brightness %d => %.2f %%", value, 100 * br);
      this->display->get_light()->set_correction(br, br, br);
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

  void EHMTX::set_scroll_count(uint8_t c)
  {
    this->scroll_count = c;
  }

  void EHMTX::set_display(addressable_light::AddressableLightDisplay *disp)
  {
    this->display = disp;
    this->show_display = true;
  }

  void EHMTX::set_clock(time::RealTimeClock *clock)
  {
    this->clock = clock;
  }

  void EHMTX::draw_day_of_week()
  {
    if (this->show_day_of_week)
    {
      auto dow = this->clock->now().day_of_week - 1; // SUN = 0
      for (uint8_t i = 0; i <= 6; i++)
      {
        if (((!this->week_starts_monday) && (dow == i)) ||
            ((this->week_starts_monday) && ((dow == (i + 1)) || ((dow == 0 && i == 6)))))
        {
          this->display->line(2 + i * 4, 7, i * 4 + 4, 7, this->today_color);
        }
        else
        {
          this->display->line(2 + i * 4, 7, i * 4 + 4, 7, this->weekday_color);
        }
      }
    }
  };

  void EHMTX::set_default_font_offset(int8_t y, int8_t x)
  {
    this->default_xoffset = x;
    this->default_yoffset = y;
    ESP_LOGD(TAG, "set_default_font_offset x: %d y: %d", x, y);
  }

  void EHMTX::set_special_font_offset(int8_t y, int8_t x)
  {
    this->special_xoffset = x;
    this->special_yoffset = y;
    ESP_LOGD(TAG, "set_special_font_offset x: %d y: %d", x, y);
  }

  void EHMTX::dump_config()
  {
    ESP_LOGCONFIG(TAG, "EspHoMatriXv2 %s", EHMTX_VERSION);
    ESP_LOGCONFIG(TAG, "Boot anim: %d", this->boot_anim);
    ESP_LOGCONFIG(TAG, "Icons: %d of %d", this->icon_count, MAXICONS);
    ESP_LOGCONFIG(TAG, "Max screens: %d", MAXQUEUE);
    ESP_LOGCONFIG(TAG, "Date format: %s", this->date_fmt.c_str());
    ESP_LOGCONFIG(TAG, "Time format: %s", this->time_fmt.c_str());
    ESP_LOGCONFIG(TAG, "Interval (ms) scroll: %d frame: %d", this->scroll_interval, this->frame_interval);
    ESP_LOGCONFIG(TAG, "Displaytime (s) clock: %d", this->clock_time);
    if (this->show_day_of_week)
    {
      ESP_LOGCONFIG(TAG, "show day of week");
    }
    if (this->show_date)
    {
      ESP_LOGCONFIG(TAG, "show date");
    }
    if (this->week_starts_monday)
    {
      ESP_LOGCONFIG(TAG, "weekstart: monday");
    }
    else
    {
      ESP_LOGCONFIG(TAG, "weekstart: sunday");
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

  void EHMTX::draw_indicator()
  {
    if (this->display_indicator > 2)
    {
      this->display->line(31, 5, 29, 7, this->indicator_color);
    }

    if (this->display_indicator > 1)
    {
      this->display->draw_pixel_at(30, 7, this->indicator_color);
      this->display->draw_pixel_at(31, 6, this->indicator_color);
    }

    if (this->display_indicator > 0)
    {
      this->display->draw_pixel_at(31, 7, this->indicator_color);
    }
  }

  void EHMTX::draw()
  {
    if ((this->is_running) && (this->show_display) && (this->screen_pointer != MAXQUEUE))
    {
      this->queue[this->screen_pointer]->draw();
      if (this->queue[this->screen_pointer]->mode != MODE_FULL_SCREEN)
      {
        this->draw_gauge();
      }
      if (this->queue[this->screen_pointer]->mode != MODE_CLOCK && this->queue[this->screen_pointer]->mode != MODE_DATE && this->queue[this->screen_pointer]->mode != MODE_FULL_SCREEN)
      {
        this->draw_indicator();
      }
      this->draw_alarm();
    }
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
}
