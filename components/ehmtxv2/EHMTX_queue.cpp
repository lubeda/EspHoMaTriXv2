#include "esphome.h"

namespace esphome
{

  EHMTX_queue::EHMTX_queue(EHMTX *config)
  {
    this->config_ = config;
    this->endtime = 0;
    this->last_time = 0;
    this->screen_time_ = 0;
    this->mode = MODE_EMPTY;
    this->icon_name = "";
    this->icon = 0;
    this->text = "";
    this->default_font = true;
  }

  void EHMTX_queue::status()
  {
    switch (this->mode)
    {
    case MODE_EMPTY:
      ESP_LOGD(TAG, ("empty slot"));
      break;
    case MODE_BLANK:
      ESP_LOGD(TAG, "queue: blank screen for %d sec", this->screen_time_);
      break;
    case MODE_CLOCK:
      ESP_LOGD(TAG, "queue: clock for: %d sec", this->screen_time_);
      break;
    case MODE_DATE:
      ESP_LOGD(TAG, "queue: date for: %d sec", this->screen_time_);
      break;
    case MODE_FULL_SCREEN:
      ESP_LOGD(TAG, "queue: full screen: \"%s\" for: %d sec", this->icon_name.c_str(), this->screen_time_);
      break;
    case MODE_ICON_SCREEN:
      ESP_LOGD(TAG, "queue: icon screen: \"%s\" text: %s for: %d sec", this->icon_name.c_str(), this->text.c_str(), this->screen_time_);
      break;
    case MODE_TEXT_SCREEN:
      ESP_LOGD(TAG, "queue: text text: \"%s\" for: %d sec", this->text.c_str(), this->screen_time_);
      break;
    case MODE_RAINBOW_ICON:
      ESP_LOGD(TAG, "queue: rainbow icon: \"%s\" text: %s for: %d sec", this->icon_name.c_str(), this->text.c_str(), this->screen_time_);
      break;
    case MODE_RAINBOW_TEXT:
      ESP_LOGD(TAG, "queue: rainbow text: \"%s\" for: %d sec", this->text.c_str(), this->screen_time_);
      break;
    case MODE_RAINBOW_CLOCK:
      ESP_LOGD(TAG, "queue: clock for: %d sec", this->screen_time_);
      break;
    case MODE_RAINBOW_DATE:
      ESP_LOGD(TAG, "queue: date for: %d sec", this->screen_time_);
      break;

#ifndef USE_ESP8266
    case MODE_BITMAP_SCREEN:
      ESP_LOGD(TAG, "queue: bitmap for: %d sec", this->screen_time_);
      break;
    case MODE_BITMAP_SMALL:
      ESP_LOGD(TAG, "queue: small bitmap for: %d sec", this->screen_time_);
      break;
#endif
    default:
      ESP_LOGD(TAG, "queue: UPPS");
      break;
    }
  }

  int EHMTX_queue::xpos()
  {
    uint8_t width = 32;
    uint8_t startx = 0;
    int result = 0;
    switch (this->mode)
    {
    case MODE_RAINBOW_ICON:
    case MODE_BITMAP_SMALL:
    case MODE_ICON_SCREEN:
      startx = 8;
      break;
    case MODE_TEXT_SCREEN:
    case MODE_RAINBOW_TEXT:
      // no correction
      break;
    default:
      break;
    }

    if (this->config_->display_gauge)
    {
      startx += 2;
    }
    width -= startx;

#ifdef EHMTXv2_USE_RTL
    if (this->pixels_ < width)
    {
      result = 32 - ceil((width - this->pixels_) / 2);
    }
    else
    {

      result = startx + this->config_->scroll_step;
    }
#else
#ifdef EHMTXv2_SCROLL_SMALL_TEXT
    result = startx - this->config_->scroll_step + width;
#else
    if (this->pixels_ < width)
    {
      result = startx + ceil((width - this->pixels_) / 2);
    }
    else
    {
      result = startx - this->config_->scroll_step + width;
    }
#endif
#endif
    return result;
  }

  void EHMTX_queue::update_screen()
  {
    if (millis() - this->config_->last_rainbow_time >= EHMTXv2_RAINBOW_INTERVALL)
    {
      this->config_->hue_++;
      if (this->config_->hue_ == 360)
      {
        this->config_->hue_ = 0;
      }
      float red, green, blue;
      esphome::hsv_to_rgb(this->config_->hue_, 0.8, 0.8, red, green, blue);
      this->config_->rainbow_color = Color(uint8_t(255 * red), uint8_t(255 * green), uint8_t(255 * blue));
      this->config_->last_rainbow_time = millis();
    }

    if (millis() - this->config_->last_anim_time >= this->config_->icons[this->icon]->frame_duration)
    {
      this->config_->icons[this->icon]->next_frame();
      this->config_->last_anim_time = millis();
    }
  }

  void EHMTX_queue::draw()
  {
    display::BaseFont *font = this->default_font ? this->config_->default_font : this->config_->special_font;
    int8_t yoffset = this->default_font ? EHMTXv2_DEFAULT_FONT_OFFSET_Y : EHMTXv2_SPECIAL_FONT_OFFSET_Y;
    int8_t xoffset = this->default_font ? EHMTXv2_DEFAULT_FONT_OFFSET_X : EHMTXv2_SPECIAL_FONT_OFFSET_X;

    Color color_;

    if (this->config_->is_running)
    {
      switch (this->mode)
      {
      case MODE_EMPTY:
        break;

      case MODE_BITMAP_SCREEN:
#ifndef USE_ESP8266
        for (uint8_t x = 0; x < 32; x++)
        {
          for (uint8_t y = 0; y < 8; y++)
          {
            this->config_->display->draw_pixel_at(x, y, this->config_->bitmap[x + y * 32]);
          }
        }
#endif
        break;

      case MODE_BITMAP_SMALL:
#ifndef USE_ESP8266
        color_ = this->text_color;
#ifdef EHMTXv2_USE_RTL
        this->config_->display->print(this->xpos() + xoffset, yoffset, font, color_, esphome::display::TextAlign::BASELINE_RIGHT,
                                      this->text.c_str());
#else
        this->config_->display->print(this->xpos() + xoffset, yoffset, font, color_, esphome::display::TextAlign::BASELINE_LEFT,
                                      this->text.c_str());
#endif
        if (this->config_->display_gauge)
        {
          this->config_->display->line(10, 0, 10, 7, esphome::display::COLOR_OFF);
          for (uint8_t x = 0; x < 8; x++)
          {
            for (uint8_t y = 0; y < 8; y++)
            {
              this->config_->display->draw_pixel_at(x + 2, y, this->config_->sbitmap[x + y * 8]);
            }
          }
        }
        else
        {
          this->config_->display->line(8, 0, 8, 7, esphome::display::COLOR_OFF);
          for (uint8_t x = 0; x < 8; x++)
          {
            for (uint8_t y = 0; y < 8; y++)
            {
              this->config_->display->draw_pixel_at(x, y, this->config_->sbitmap[x + y * 8]);
            }
          }
        }
#endif
        break;

      case MODE_RAINBOW_CLOCK:
      case MODE_CLOCK:
        if (this->config_->clock->now().is_valid()) // valid time
        {
          color_ = (this->mode == MODE_RAINBOW_CLOCK) ? this->config_->rainbow_color : this->text_color;
          time_t ts = this->config_->clock->now().timestamp;
          this->config_->display->strftime(xoffset + 15, yoffset, font, color_, display::TextAlign::BASELINE_CENTER, EHMTXv2_TIME_FORMAT,
                                           this->config_->clock->now());
          if ((this->config_->clock->now().second % 2 == 0) && this->config_->show_seconds)
          {
            this->config_->display->draw_pixel_at(0, 0, color_);
          }
          if (this->mode != MODE_RAINBOW_CLOCK)
          {
            this->config_->draw_day_of_week();
          }
        }
        else
        {
          this->config_->display->print(15 + xoffset, yoffset, font, this->config_->alarm_color, display::TextAlign::BASELINE_CENTER, "!t!");
        }
        break;
      case MODE_RAINBOW_DATE:
      case MODE_DATE:
        if (this->config_->clock->now().is_valid())
        {
          color_ = (this->mode == MODE_RAINBOW_DATE) ? this->config_->rainbow_color : this->text_color;
          time_t ts = this->config_->clock->now().timestamp;
          this->config_->display->strftime(xoffset + 15, yoffset, font, color_, display::TextAlign::BASELINE_CENTER, EHMTXv2_DATE_FORMAT,
                                           this->config_->clock->now());
          if ((this->config_->clock->now().second % 2 == 0) && this->config_->show_seconds)
          {
            this->config_->display->draw_pixel_at(0, 0, color_);
          }
          if (this->mode != MODE_RAINBOW_DATE)
          {
            this->config_->draw_day_of_week();
          }
        }
        else
        {
          this->config_->display->print(xoffset + 15, yoffset, font, this->config_->alarm_color, display::TextAlign::BASELINE_CENTER, "!d!");
        }
        break;
      case MODE_FULL_SCREEN:
        this->config_->display->image(0, 0, this->config_->icons[this->icon]);
        break;
      case MODE_ICON_SCREEN:
      case MODE_RAINBOW_ICON:
      {
        color_ = (this->mode == MODE_RAINBOW_ICON) ? this->config_->rainbow_color : this->text_color;
#ifdef EHMTXv2_USE_RTL
        this->config_->display->print(this->xpos() + xoffset, yoffset, font, color_, esphome::display::TextAlign::BASELINE_RIGHT,
                                      this->text.c_str());
#else
        this->config_->display->print(this->xpos() + xoffset, yoffset, font, color_, esphome::display::TextAlign::BASELINE_LEFT,
                                      this->text.c_str());
#endif
        if (this->config_->display_gauge)
        {
          this->config_->display->image(2, 0, this->config_->icons[this->icon]);
          this->config_->display->line(10, 0, 10, 7, esphome::display::COLOR_OFF);
        }
        else
        {
          this->config_->display->line(8, 0, 8, 7, esphome::display::COLOR_OFF);
          this->config_->display->image(0, 0, this->config_->icons[this->icon]);
        }
      }
      break;
      case MODE_TEXT_SCREEN:
      case MODE_RAINBOW_TEXT:
        color_ = (this->mode == MODE_RAINBOW_TEXT) ? this->config_->rainbow_color : this->text_color;
#ifdef EHMTXv2_USE_RTL
        this->config_->display->print(this->xpos() + xoffset, yoffset, font, color_, esphome::display::TextAlign::BASELINE_RIGHT,
                                      this->text.c_str());
#else
        this->config_->display->print(this->xpos() + xoffset, yoffset, font, color_, esphome::display::TextAlign::BASELINE_LEFT,
                                      this->text.c_str());
#endif
        break;
      case MODE_BLANK:
          this->config_->display->fill(Color());
        break;
      default:
        ESP_LOGD("draw", "Upps");
        break;
      }
      this->update_screen();
    }
  }

  void EHMTX_queue::hold_slot(uint8_t _sec)
  {
    this->endtime += _sec;
    ESP_LOGD(TAG, "hold for %d secs", _sec);
  }

  // TODO void EHMTX_queue::set_mode_icon()

  void EHMTX_queue::calc_scroll_time(std::string text, uint16_t screen_time)
  {
    int x, y, w, h;
    float display_duration;

    uint8_t width = 32;
    uint8_t startx = 0;
    uint16_t max_steps = 0;

    if (this->default_font)
    {
      this->config_->display->get_text_bounds(0, 0, text.c_str(), this->config_->default_font, display::TextAlign::LEFT, &x, &y, &w, &h);
    }
    else
    {
      this->config_->display->get_text_bounds(0, 0, text.c_str(), this->config_->special_font, display::TextAlign::LEFT, &x, &y, &w, &h);
    }

    this->pixels_ = w;

    switch (this->mode)
    {
    case MODE_RAINBOW_TEXT:
    case MODE_TEXT_SCREEN:
#ifdef EHMTXv2_SCROLL_SMALL_TEXT
      max_steps = (EHMTXv2_SCROLL_COUNT + 1) * (width - startx) + EHMTXv2_SCROLL_COUNT * this->pixels_;
      display_duration = ceil((max_steps * EHMTXv2_SCROLL_INTERVALL) / 1000);
      this->screen_time_ = (display_duration > screen_time) ? display_duration : screen_time;
#else
      if (this->pixels_ < 32)
      {
        this->screen_time_ = screen_time;
      }
      else
      {
        max_steps = (EHMTXv2_SCROLL_COUNT + 1) * (width - startx) + EHMTXv2_SCROLL_COUNT * this->pixels_;
        display_duration = ceil((max_steps * EHMTXv2_SCROLL_INTERVALL) / 1000);
        this->screen_time_ = (display_duration > screen_time) ? display_duration : screen_time;
      }
#endif
      break;
    case MODE_RAINBOW_ICON:
    case MODE_BITMAP_SMALL:
    case MODE_ICON_SCREEN:
      startx = 8;
      if (this->pixels_ < 23)
      {
        this->screen_time_ = screen_time;
      }
      else
      {
        max_steps = (EHMTXv2_SCROLL_COUNT + 1) * (width - startx) + EHMTXv2_SCROLL_COUNT * this->pixels_;
        display_duration = ceil((max_steps * EHMTXv2_SCROLL_INTERVALL) / 1000);
        this->screen_time_ = (display_duration > screen_time) ? display_duration : screen_time;
      }
      break;
    default:
      break;
    }

    this->scroll_reset = (width - startx) + this->pixels_;
    ;

    ESP_LOGD(TAG, "calc_scroll_time: mode: %d text: \"%s\" pixels %d calculated: %d defined: %d max_steps: %d", this->mode, text.c_str(), this->pixels_, this->screen_time_, screen_time, this->scroll_reset);
  }
}
