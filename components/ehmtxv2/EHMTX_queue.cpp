#include "esphome.h"

namespace esphome
{

#ifdef USE_Fireplugin
  const size_t m_heatSize = 8 * 32; /**< Number of heat temperatures */
  uint8_t *m_heat = new (std::nothrow) uint8_t[m_heatSize];
  /**
   * Cooling: How much does the air cool as it rises?
   * Less cooling => taller flames.
   * More cooling => shorter flames.
   */
  static const uint8_t COOLING = 60U;

  /**
   * Sparking: What chance (out of 255) is there that a new spark will be lit?
   * Higher chance = more roaring fire.  Lower chance = more flickery fire.
   */
  static const uint8_t SPARKING = 120U;

  /**
   * Approximates a 'black body radiation' spectrum for a given 'heat' level.
   * This is useful for animations of 'fire'.
   * Heat is specified as an arbitrary scale from 0 (cool) to 255 (hot).
   * This is NOT a chromatically correct 'black body radiation'
   * spectrum, but it's surprisingly close, and it's fast and small.
   */
  Color heatColor(uint8_t temperature);
#endif
#ifdef USE_Fireplugin

  Color EHMTX_queue::heatColor(uint8_t temperature)
  {
    Color heatColor;

    /* Scale 'heat' down from 0-255 to 0-191, which can then be easily divided
     * into three equal 'thirds' of 64 units each.
     */
    uint8_t t192 = static_cast<uint32_t>(temperature) * 191U / 255U;

    /* Calculate a value that ramps up from zero to 255 in each 'third' of the scale. */
    uint8_t heatRamp = t192 & 0x3fU; /* 0..63 */

    /* Scale up to 0..252 */
    heatRamp <<= 2;

    /* Now figure out which third of the spectrum we're in. */
    if (t192 & 0x80U)
    {
      /* We're in the hottest third */
      heatColor = Color(255U, 255U, heatRamp); /* Ramp up blue */
    }
    else if (t192 & 0x40U)
    {
      /* We're in the middle third */
      heatColor = Color(255U, heatRamp, 0U); /* No blue */
    }
    else
    {
      /* We're in the coolest third */
      heatColor = Color(heatRamp, 0, 0);
    }

    return heatColor;
  }

#endif

  EHMTX_queue::EHMTX_queue(EHMTX *config)
  {
    this->config_ = config;
    this->endtime = 0.0;
    this->last_time = 0.0;
    this->screen_time_ = 0.0;
    this->mode = MODE_EMPTY;
    this->icon_name = "";
    this->icon = 0;
    this->text = "";
    this->default_font = true;
    this->progress = 0;
    this->sbitmap = nullptr;
    #ifdef EHMTXv2_ADV_BITMAP
    this->bitmap = nullptr;
    #endif
    this->progressbar_color = esphome::display::COLOR_OFF;
    this->progressbar_back_color = esphome::display::COLOR_OFF;
  }

  void EHMTX_queue::status()
  {
    switch (this->mode)
    {
    case MODE_EMPTY:
      ESP_LOGD(TAG, "empty slot");
      break;
    case MODE_BLANK:
      ESP_LOGD(TAG, "queue: blank screen for %.1f sec", this->screen_time_ / 1000.0);
      break;
    case MODE_COLOR:
      ESP_LOGD(TAG, "queue: color screen for %.1f sec", this->screen_time_ / 1000.0);
      break;
    case MODE_CLOCK:
      ESP_LOGD(TAG, "queue: clock for: %.1f sec", this->screen_time_ / 1000.0);
      break;
    case MODE_DATE:
      ESP_LOGD(TAG, "queue: date for: %.1f sec", this->screen_time_ / 1000.0);
      break;
    case MODE_GRAPH_SCREEN:
      ESP_LOGD(TAG, "queue: graph for: %.1f sec", this->screen_time_ / 1000.0);
      break;
    case MODE_FULL_SCREEN:
      ESP_LOGD(TAG, "queue: full screen: \"%s\" for: %.1f sec", this->icon_name.c_str(), this->screen_time_ / 1000.0);
      break;
    case MODE_ICON_SCREEN:
      ESP_LOGD(TAG, "queue: icon screen: \"%s\" text: %s for: %.1f sec", this->icon_name.c_str(), this->text.c_str(), this->screen_time_ / 1000.0);
      break;
    case MODE_ICON_PROGRESS:
      ESP_LOGD(TAG, "queue: icon progress: \"%s\" text: %s for: %.1f sec", this->icon_name.c_str(), this->text.c_str(), this->screen_time_ / 1000.0);
      break;
    case MODE_PROGNOSIS_SCREEN:
      ESP_LOGD(TAG, "queue: icon prognosis screen: \"%s\" text: %s for: %.1f sec", this->icon_name.c_str(), this->text.c_str(), this->screen_time_ / 1000.0);
      break;
    case MODE_TEXT_PROGRESS:
      ESP_LOGD(TAG, "queue: text progress: \"%s\" text: %s for: %.1f sec", this->icon_name.c_str(), this->text.c_str(), this->screen_time_ / 1000.0);
      break;
    case MODE_ICON_CLOCK:
      ESP_LOGD(TAG, "queue: icon clock: \"%s\" for: %.1f sec", this->icon_name.c_str(), this->screen_time_ / 1000.0);
      break;
    case MODE_ICON_DATE:
      ESP_LOGD(TAG, "queue: icon date: \"%s\" for: %.1f sec", this->icon_name.c_str(), this->screen_time_ / 1000.0);
      break;
    case MODE_ALERT_SCREEN:
      ESP_LOGD(TAG, "queue: alert icon: \"%s\" for: %.1f sec", this->icon_name.c_str(), this->screen_time_ / 1000.0);
      break;
    case MODE_RAINBOW_ALERT_SCREEN:
      ESP_LOGD(TAG, "queue: rainbow alert icon: \"%s\" for: %.1f sec", this->icon_name.c_str(), this->screen_time_ / 1000.0);
      break;
    case MODE_TEXT_SCREEN:
      ESP_LOGD(TAG, "queue: text text: \"%s\" for: %.1f sec", this->text.c_str(), this->screen_time_ / 1000.0);
      break;
    case MODE_RAINBOW_ICON:
      ESP_LOGD(TAG, "queue: rainbow icon: \"%s\" text: %s for: %.1f sec", this->icon_name.c_str(), this->text.c_str(), this->screen_time_ / 1000.0);
      break;
    case MODE_RAINBOW_TEXT:
      ESP_LOGD(TAG, "queue: rainbow text: \"%s\" for: %.1f sec", this->text.c_str(), this->screen_time_ / 1000.0);
      break;
    case MODE_RAINBOW_CLOCK:
      ESP_LOGD(TAG, "queue: rainbow clock for: %.1f sec", this->screen_time_ / 1000.0);
      break;
    case MODE_RAINBOW_DATE:
      ESP_LOGD(TAG, "queue: rainbow date for: %.1f sec", this->screen_time_ / 1000.0);
      break;
    case MODE_ICON_TEXT_SCREEN:
      ESP_LOGD(TAG, "queue: icon text screen: \"%s\" text: %s for: %.1f sec", this->icon_name.c_str(), this->text.c_str(), this->screen_time_ / 1000.0);
      break;
    case MODE_RAINBOW_ICON_TEXT_SCREEN:
      ESP_LOGD(TAG, "queue: rainbow icon text screen: \"%s\" text: %s for: %.1f sec", this->icon_name.c_str(), this->text.c_str(), this->screen_time_ / 1000.0);
      break;
    case MODE_FIRE:
      ESP_LOGD(TAG, "queue: fire for: %.1f sec", this->screen_time_ / 1000.0);
      break;

#ifndef USE_ESP8266
    case MODE_BITMAP_SCREEN:
      ESP_LOGD(TAG, "queue: bitmap for: %.1f sec", this->screen_time_ / 1000.0);
      break;
    case MODE_BITMAP_SMALL:
      ESP_LOGD(TAG, "queue: small bitmap for: %.1f sec", this->screen_time_ / 1000.0);
      break;
    case MODE_RAINBOW_BITMAP_SMALL:
      ESP_LOGD(TAG, "queue: rainbow small bitmap for: %.1f sec", this->screen_time_ / 1000.0);
      break;
    case MODE_BITMAP_STACK_SCREEN :
      ESP_LOGD(TAG, "queue: bitmap stack for: %.1f sec", this->screen_time_ / 1000.0);
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
    case MODE_RAINBOW_BITMAP_SMALL:
    case MODE_ICON_SCREEN:
    case MODE_ICON_CLOCK:
    case MODE_ICON_DATE:
    case MODE_ALERT_SCREEN:
    case MODE_RAINBOW_ALERT_SCREEN:
    case MODE_ICON_PROGRESS:
    case MODE_PROGNOSIS_SCREEN:
      startx = 8;
      break;
    case MODE_TEXT_SCREEN:
    case MODE_RAINBOW_TEXT:
      // no correction
      break;
    case MODE_ICON_TEXT_SCREEN:
    case MODE_RAINBOW_ICON_TEXT_SCREEN:
      if (this->pixels_ < 32)
      {
        startx = 8;
      }
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

  void c16to8(int16_t t, uint8_t& r, uint8_t& g)
  {
    r = static_cast<uint8_t>((t & 0xFF00) >> 8);
    g = static_cast<uint8_t>(t & 0x00FF);
  }

  int16_t c8to16(uint8_t r, uint8_t g)
  {
    return (static_cast<uint16_t>(r) << 8) | g;
  }

  uint8_t is_tick(int step, uint8_t& state)
  {
    if (step % 2 == state)
    {
      return 0;
    }
    state = step % 2;
    return 1;
  }

  int EHMTX_queue::xpos(uint8_t item)
  {
    uint8_t width = 32;
    int result = width - this->config_->scroll_step + item * 9;
    
    if (this->icon < 5)
    {
      int16_t item_pos = c8to16(this->sbitmap[item].r, this->sbitmap[item].g);
      
      uint8_t target = round(((static_cast<float>(width) - 8 * static_cast<float>(this->icon)) / static_cast<float>(this->icon + 1)) * (item + 1) + 8 * item);
      if ((this->progress == 1) && (this->icon == 2 || this->icon == 3))
      {
        uint8_t reverse_steps = round(((static_cast<float>(width) - 8 * static_cast<float>(this->icon)) / static_cast<float>(this->icon + 1)) + 8);

        if (ceil((this->config_->next_action_time - this->config_->get_tick()) / EHMTXv2_SCROLL_INTERVALL) > reverse_steps)
        {
          result = (item + 1 == this->icon) ? width - this->config_->scroll_step : -8 + this->config_->scroll_step;
          if (item == 0 && (item_pos == 32767 || item_pos < target))
          {
            item_pos = result < target ? result : target;
          }
          else if (item + 1 == this->icon && item_pos > target)
          {
            item_pos = result > target ? result : target;
          }
          else if (this->icon == 3 && item == 1)
          {
            item_pos = target;
          }
        }
        else
        {
          if (item == 0)
          {
            item_pos -= is_tick(this->config_->scroll_step, this->sbitmap[item].w);
          }
          else if (item + 1 == this->icon)
          {
            item_pos += is_tick(this->config_->scroll_step, this->sbitmap[item].w);
          }
          else if (this->icon == 3 && item == 1)
          {
            item_pos = target;
          }
        }
      }
      else if ((this->progress == 1) && this->icon == 1)
      {
        item_pos = target;
      }
      else
      {
        uint8_t reverse_steps = round(((static_cast<float>(width) - 8 * static_cast<float>(this->icon)) / static_cast<float>(this->icon + 1)) * this->icon + 8 * (this->icon + 1)) + 8;

        if (ceil((this->config_->next_action_time - this->config_->get_tick()) / EHMTXv2_SCROLL_INTERVALL) > reverse_steps)
        {
          if (item_pos > target)
          {
            item_pos = result > target ? result : target;
          }
        }
        else
        {
          item_pos -= is_tick(this->config_->scroll_step, this->sbitmap[item].w);
        }
      }
      
      c16to8(item_pos, this->sbitmap[item].r, this->sbitmap[item].g);
      result = item_pos;
    }

    return result;
  }

  int EHMTX_queue::ypos()
  {
    #ifdef EHMTXv2_USE_VERTICAL_SCROLL
      if (this->config_->queue_count() <= 1)
      {
        return 0;
      }

      uint8_t height = 8;

      if (ceil((this->config_->next_action_time - this->config_->get_tick()) / EHMTXv2_SCROLL_INTERVALL) > height)
      {
        if (this->config_->vertical_scroll)
        {
          return 0;
        }
        this->config_->vertical_scroll = this->config_->scroll_step >= height;
        return this->config_->scroll_step - height;
      }
      return height - round((this->config_->next_action_time - this->config_->get_tick()) / EHMTXv2_SCROLL_INTERVALL);
    #else
      return 0;
    #endif
  }

  int EHMTX_queue::ypos(uint8_t item)
  {
    uint8_t height = 8;
    
    if ((this->progress == 1) && (this->icon == 1 || (this->icon == 3 && item == 1)))
    {
      if (ceil((this->config_->next_action_time - this->config_->get_tick()) / EHMTXv2_SCROLL_INTERVALL) > height)
      {
        if (this->default_font)
        {
          return 0;
        }
        this->default_font = this->config_->scroll_step >= height;
        return this->config_->scroll_step - height;
      }
      return height - round((this->config_->next_action_time - this->config_->get_tick()) / EHMTXv2_SCROLL_INTERVALL);
    }
    
    return 0;
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

    if (this->mode == MODE_BITMAP_STACK_SCREEN && this->sbitmap != NULL)
    {
      uint32_t average_frame_duration = 0;
      for (uint8_t i = 0; i < this->icon; i++)
      {
        average_frame_duration += this->config_->icons[this->sbitmap[i].b]->frame_duration;
      }
      average_frame_duration = average_frame_duration / this->icon;
      
      if (millis() - this->config_->last_anim_time >= average_frame_duration)
      {
        for (uint8_t i = 0; i < this->icon; i++)
        {
          this->config_->icons[this->sbitmap[i].b]->next_frame();
        }
        this->config_->last_anim_time = millis();
      }
    }
    else if (this->icon < this->config_->icon_count)
    {
      if (millis() - this->config_->last_anim_time >= this->config_->icons[this->icon]->frame_duration)
      {
        this->config_->icons[this->icon]->next_frame();
        this->config_->last_anim_time = millis();
      }
    }
  }

  void EHMTX_queue::draw()
  {
    display::BaseFont *font = this->default_font ? this->config_->default_font : this->config_->special_font;
    display::BaseFont *info_font = this->config_->info_font ? this->config_->default_font : this->config_->special_font;
    
    int8_t yoffset = this->default_font ? EHMTXv2_DEFAULT_FONT_OFFSET_Y : EHMTXv2_SPECIAL_FONT_OFFSET_Y;
    int8_t xoffset = this->default_font ? EHMTXv2_DEFAULT_FONT_OFFSET_X : EHMTXv2_SPECIAL_FONT_OFFSET_X;

    Color color_;

    if (this->config_->is_running)
    {
      this->config_->display->clear();
      switch (this->mode)
      {
      case MODE_BLANK:
        break;

      case MODE_COLOR:
        this->config_->display->fill(this->text_color);
        break;

      case MODE_BITMAP_SCREEN:
#ifndef USE_ESP8266
        #ifdef EHMTXv2_ADV_BITMAP
        if (this->bitmap != NULL)
        {
        #endif
        for (uint8_t x = 0; x < 32; x++)
        {
          for (uint8_t y = 0; y < 8; y++)
          {
            #ifdef EHMTXv2_ADV_BITMAP
              this->config_->display->draw_pixel_at(x, this->ypos() + y, this->bitmap[x + y * 32]);
            #else
              this->config_->display->draw_pixel_at(x, this->ypos() + y, this->config_->bitmap[x + y * 32]);
            #endif
            }
          }
        #ifdef EHMTXv2_ADV_BITMAP
        }
        #endif
#endif
        break;

#ifdef USE_GRAPH
      case MODE_GRAPH_SCREEN:
        if (this->icon == MAXICONS)
        {
          this->config_->display->graph(0, this->ypos(), this->config_->graph);
        }
        else
        {
          this->config_->display->graph(8, this->ypos(), this->config_->graph);
          if (this->icon != BLANKICON)
          {
            this->config_->display->line(8, this->ypos(), 8, this->ypos() + 7, esphome::display::COLOR_OFF);
            if (this->icon == SOLIDICON)
            {
              this->config_->display->filled_rectangle(0, this->ypos(), 8, 8, this->config_->solid_color);
            }
            else if (this->icon == CALENDARICON)
            {
              this->config_->display->filled_rectangle(0, this->ypos(), 8, 8, Color(C_RED, C_GREEN, C_BLUE));
              this->config_->display->filled_rectangle(0, this->ypos(), 8, 2, this->config_->calendar_color);
            }
            else
            {
              this->config_->display->image(0, this->ypos(), this->config_->icons[this->icon]);
            }
          }
        }
        break;
#endif

      case MODE_BITMAP_SMALL:
      case MODE_RAINBOW_BITMAP_SMALL:
#ifndef USE_ESP8266
        color_ = (this->mode == MODE_RAINBOW_BITMAP_SMALL) ? this->config_->rainbow_color : this->text_color;
#ifdef EHMTXv2_USE_RTL
        this->config_->display->print(this->xpos() + xoffset, this->ypos() + yoffset, font, color_, esphome::display::TextAlign::BASELINE_RIGHT,
                                      this->text.c_str());
#else
        this->config_->display->print(this->xpos() + xoffset, this->ypos() + yoffset, font, color_, esphome::display::TextAlign::BASELINE_LEFT,
                                      this->text.c_str());
#endif
        if (this->sbitmap != NULL)
        {
          if (this->config_->display_gauge)
          {
            this->config_->display->line(10, this->ypos(), 10, this->ypos() + 7, esphome::display::COLOR_OFF);
            for (uint8_t x = 0; x < 8; x++)
            {
              for (uint8_t y = 0; y < 8; y++)
              {
                this->config_->display->draw_pixel_at(x + 2, this->ypos() + y, this->sbitmap[x + y * 8]);
              }
            }
          }
          else
          {
            this->config_->display->line(8, this->ypos(), 8, this->ypos() + 7, esphome::display::COLOR_OFF);
            for (uint8_t x = 0; x < 8; x++)
            {
              for (uint8_t y = 0; y < 8; y++)
              {
                this->config_->display->draw_pixel_at(x, this->ypos() + y, this->sbitmap[x + y * 8]);
              }
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
          #ifdef EHMTXv2_ADV_CLOCK
          if (!this->config_->draw_clock(EHMTXv2_TIME_FORMAT_BIG, font, color_, xoffset + 15, this->ypos() + yoffset))
          {
          #endif
            if (this->config_->replace_time_date_active) // check for replace active
            {
              std::string time_new = this->config_->clock->now().strftime(EHMTXv2_TIME_FORMAT).c_str();
              time_new = this->config_->replace_time_date(time_new);
              this->config_->display->printf(xoffset + 15, this->ypos() + yoffset, font, color_, display::TextAlign::BASELINE_CENTER, "%s", time_new.c_str());
            } 
            else 
            {
              this->config_->display->strftime(xoffset + 15, this->ypos() + yoffset, font, color_, display::TextAlign::BASELINE_CENTER, EHMTXv2_TIME_FORMAT,
                                               this->config_->clock->now());                    
            }
            if (this->config_->show_seconds && (this->config_->clock->now().second % 2 == 0))
            {
              this->config_->display->draw_pixel_at(0, 0, color_);
            }
          #ifdef EHMTXv2_ADV_CLOCK
          }
          #endif
          if (this->mode != MODE_RAINBOW_CLOCK)
          {
            this->config_->draw_day_of_week(this->ypos());
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
          #ifdef EHMTXv2_ADV_CLOCK
          if (!this->config_->draw_date(EHMTXv2_DATE_FORMAT, font, color_, xoffset + 15, this->ypos() + yoffset))
          {
          #endif
            if (this->config_->replace_time_date_active) // check for replace active
            {
              std::string time_new = this->config_->clock->now().strftime(EHMTXv2_DATE_FORMAT).c_str();
              time_new = this->config_->replace_time_date(time_new);
              this->config_->display->printf(xoffset + 15, this->ypos() + yoffset, font, color_, display::TextAlign::BASELINE_CENTER, "%s", time_new.c_str());
            }
            else
            {
              this->config_->display->strftime(xoffset + 15, this->ypos() + yoffset, font, color_, display::TextAlign::BASELINE_CENTER, EHMTXv2_DATE_FORMAT,
                                               this->config_->clock->now());                    
            }
            if ((this->config_->clock->now().second % 2 == 0) && this->config_->show_seconds)
            {
              this->config_->display->draw_pixel_at(0, 0, color_);
            }
          #ifdef EHMTXv2_ADV_CLOCK
          }
          #endif
          if (this->mode != MODE_RAINBOW_DATE)
          {
            this->config_->draw_day_of_week(this->ypos());
          }
        }
        else
        {
          this->config_->display->print(xoffset + 15, yoffset, font, this->config_->alarm_color, display::TextAlign::BASELINE_CENTER, "!d!");
        }
        break;

      case MODE_FULL_SCREEN:
        this->config_->display->image(0, this->ypos(), this->config_->icons[this->icon]);
        break;

      case MODE_ICON_CLOCK:
      case MODE_ICON_DATE:
        if (this->config_->clock->now().is_valid()) // valid time
        {
          color_ = this->text_color;
          time_t ts = this->config_->clock->now().timestamp;
          uint8_t offset = (this->config_->icon_to_9 == 3) ? 21 : 20;

          if (this->mode == MODE_ICON_CLOCK)
          {
            #ifdef EHMTXv2_ADV_CLOCK
            if (!this->config_->draw_clock(EHMTXv2_TIME_FORMAT, font, color_, xoffset + offset, this->ypos() + yoffset))
            {
            #endif
              if (this->config_->replace_time_date_active) // check for replace active
              {
                std::string time_new = this->config_->clock->now().strftime(EHMTXv2_TIME_FORMAT).c_str();
                time_new = this->config_->replace_time_date(time_new);
                this->config_->display->printf(xoffset + offset, this->ypos() + yoffset, font, color_, display::TextAlign::BASELINE_CENTER, "%s", time_new.c_str());
              } 
              else 
              {
                this->config_->display->strftime(xoffset + offset, this->ypos() + yoffset, font, color_, display::TextAlign::BASELINE_CENTER, EHMTXv2_TIME_FORMAT,
                                                 this->config_->clock->now());
              }
            #ifdef EHMTXv2_ADV_CLOCK
            }
            #endif
          }
          else
          {
            #ifdef EHMTXv2_ADV_CLOCK
            if (!this->config_->draw_date(EHMTXv2_DATE_FORMAT, font, color_, xoffset + offset, this->ypos() + yoffset))
            {
            #endif
              if (this->config_->replace_time_date_active) // check for replace active
              {
                std::string time_new = this->config_->clock->now().strftime(EHMTXv2_DATE_FORMAT).c_str();
                time_new = this->config_->replace_time_date(time_new);
                this->config_->display->printf(xoffset + offset, this->ypos() + yoffset, font, color_, display::TextAlign::BASELINE_CENTER, "%s", time_new.c_str());
              } 
              else 
              {
                this->config_->display->strftime(xoffset + offset, this->ypos() + yoffset, font, color_, display::TextAlign::BASELINE_CENTER, EHMTXv2_DATE_FORMAT,
                                                 this->config_->clock->now());
              }
            #ifdef EHMTXv2_ADV_CLOCK
            }
            #endif
          }
          if (this->icon != BLANKICON)
          {
            if (this->icon == SOLIDICON)
            {
              if ((this->mode == MODE_ICON_CLOCK && this->config_->icon_to_9 == 1) ||
                  (this->mode == MODE_ICON_DATE  && this->config_->icon_to_9 == 2) ||
                  (this->config_->icon_to_9 == 3))
              {
                this->config_->display->filled_rectangle(0, this->ypos(), 9, 8, this->config_->solid_color); 
              }
              else
              {
                this->config_->display->filled_rectangle(0, this->ypos(), 8, 8, this->config_->solid_color); 
              }
            }
            else if (this->icon == CALENDARICON)
            {
              if ((this->mode == MODE_ICON_CLOCK && this->config_->icon_to_9 == 1) ||
                  (this->mode == MODE_ICON_DATE  && this->config_->icon_to_9 == 2) ||
                  (this->config_->icon_to_9 == 3))
              {
                this->config_->display->filled_rectangle(0, this->ypos(), 9, 8, Color(C_RED, C_GREEN, C_BLUE)); 
                this->config_->display->filled_rectangle(0, this->ypos(), 9, 2, this->config_->calendar_color); 
              }
              else
              {
                this->config_->display->filled_rectangle(0, this->ypos(), 8, 8, Color(C_RED, C_GREEN, C_BLUE)); 
                this->config_->display->filled_rectangle(0, this->ypos(), 8, 2, this->config_->calendar_color); 
              }
            }
            else
            {
              if ((this->mode == MODE_ICON_CLOCK && this->config_->icon_to_9 == 1) ||
                  (this->mode == MODE_ICON_DATE  && this->config_->icon_to_9 == 2) ||
                  (this->config_->icon_to_9 == 3))
              {
                this->config_->display->image(1, this->ypos(), this->config_->icons[this->icon]);
              }
              this->config_->display->image(0, this->ypos(), this->config_->icons[this->icon]);
            }
          }
          this->config_->draw_day_of_week(this->ypos(), true);

          if (this->icon_name.find("day") != std::string::npos || this->icon_name.find("weekday") != std::string::npos)
          {
            int8_t i_y_offset = this->config_->info_y_offset;
            Color i_lcolor = this->config_->info_rcolor;
            Color i_rcolor = this->config_->info_rcolor;

            #ifdef EHMTXv2_ADV_CLOCK
            if (this->mode == MODE_ICON_CLOCK)
            {
              i_y_offset = this->config_->info_clock_y_offset;
              i_lcolor = this->config_->info_clock_rcolor;
              i_rcolor = this->config_->info_clock_rcolor;
              info_font = this->config_->info_clock_font ? this->config_->default_font : this->config_->special_font;
            }
            #endif

            int mode = 0;
            std::size_t pos = icon_name.find("#");
            if (pos != std::string::npos)
            {
              std::string str_mode = icon_name.substr(pos + 1);
              if (str_mode.length())
              {
                mode = std::stoi(str_mode);
              }
            }

            uint8_t x_left = 0;  // Leftmost point
            uint8_t x_right = 9; // Rightmost point

            if (this->icon_name.rfind("day", 0) == 0)
            {
              uint8_t d = this->config_->clock->now().day_of_month;

              // The symbol consists of a visible part, and an empty area to the right with a width of one point.
              uint8_t l_width = this->config_->GetTextWidth(info_font, "%d", d / 10 % 10);
              uint8_t r_width = this->config_->GetTextWidth(info_font, "%d", d % 10);
              switch (mode)
              {
              // To the center
              case 1:
              // To the center, the left one is a pixel higher.
              case 3:
              // To the center, the right one is a pixel higher.
              case 4:
              // To the center, without leading 0
              case 5:
                x_left = (l_width < 5) ? 5 - l_width : 0;
                x_right = 4;
                break;
              // Left to center, Right to edge
              case 2:
                x_left = (l_width < 5) ? 5 - l_width : 0;
                x_right = x_right - r_width;
                break;
              // To the edges
              default:
                x_right = x_right - r_width;
                break;
              }
              if (mode == 5 && (d < 10))
              {
                if (this->config_->icon_to_9 == 3)
                {
                  x_right = 5 - (r_width - 1) / 2;
                }
                else
                {
                  x_right = 4 - (r_width - 1) / 2;
                }
                this->config_->display->printf(x_right, this->ypos() + yoffset + i_y_offset, info_font, i_rcolor, display::TextAlign::BASELINE_LEFT, "%d", d % 10);
              }
              else
              {
                this->config_->display->printf(x_left, this->ypos() + yoffset + i_y_offset - (mode != 3 ? 0 : 1), info_font, i_lcolor, display::TextAlign::BASELINE_LEFT, "%d", d / 10 % 10);
                this->config_->display->printf(x_right, this->ypos() + yoffset + i_y_offset - (mode != 4 ? 0 : 1), info_font, i_rcolor, display::TextAlign::BASELINE_LEFT, "%d", d % 10);
              }
            }
            else // if (this->icon_name.rfind("weekday", 0) == 0)
            {
              uint8_t wd = this->config_->clock->now().day_of_week;

              if (this->config_->weekday_char_count > 7)
              {
                std::string left = this->config_->GetWeekdayChar((wd - 1) * 2);
                std::string right = this->config_->GetWeekdayChar((wd - 1) * 2 + 1);

                // The symbol consists of a visible part, and an empty area to the right with a width of one point.
                uint8_t l_width = this->config_->GetTextWidth(info_font, "%s", left.c_str());
                uint8_t r_width = this->config_->GetTextWidth(info_font, "%s", right.c_str());

                switch (mode)
                {
                // To the center
                case 1:
                // To the center, the left one is a pixel higher.
                case 3:
                // To the center, the right one is a pixel higher.
                case 4:
                  x_left = (l_width < 5) ? 5 - l_width : 0;
                  x_right = 4;
                  break;
                // Left to center, Right to edge
                case 2:
                  x_left = (l_width < 5) ? 5 - l_width : 0;
                  x_right = x_right - r_width;
                  break;
                // To the edges
                default:
                  x_right = x_right - r_width;
                  break;
                }
                this->config_->display->printf(x_left, this->ypos() + yoffset + i_y_offset - (mode != 3 ? 0 : 1), info_font, i_lcolor, display::TextAlign::BASELINE_LEFT, "%s", left.c_str());
                this->config_->display->printf(x_right, this->ypos() + yoffset + i_y_offset - (mode != 4 ? 0 : 1), info_font, i_rcolor, display::TextAlign::BASELINE_LEFT, "%s", right.c_str());
              }
              else
              {
                std::string weekday = this->config_->GetWeekdayChar(wd - 1);

                // The symbol consists of a visible part, and an empty area to the right with a width of one point.
                uint8_t c_width = this->config_->GetTextWidth(info_font, "%s", weekday.c_str());
                if (this->config_->icon_to_9 == 3)
                {
                  x_left = 5 - (c_width - 1) / 2;
                }
                else
                {
                  x_left = 4 - (c_width - 1) / 2;
                }
                this->config_->display->printf(x_left, this->ypos() + yoffset + i_y_offset, info_font, i_lcolor, display::TextAlign::BASELINE_LEFT, "%s", weekday.c_str());
              }
            }
          }
        }
        else
        {
          this->config_->display->print( xoffset + 19, yoffset, font, this->config_->alarm_color, display::TextAlign::BASELINE_CENTER, "!t!");
        }
        break;

      case MODE_ICON_SCREEN:
      case MODE_ALERT_SCREEN:
      case MODE_RAINBOW_ALERT_SCREEN:
      case MODE_RAINBOW_ICON:
      case MODE_ICON_PROGRESS:
      case MODE_PROGNOSIS_SCREEN:
        color_ = (this->mode == MODE_RAINBOW_ICON || this->mode == MODE_RAINBOW_ALERT_SCREEN) ? this->config_->rainbow_color : this->text_color;
#ifdef EHMTXv2_USE_RTL
        this->config_->display->print(this->xpos() + xoffset, this->ypos() + yoffset, font, color_, esphome::display::TextAlign::BASELINE_RIGHT,
                                      this->text.c_str());
#else
        this->config_->display->print(this->xpos() + xoffset, this->ypos() + yoffset, font, color_, esphome::display::TextAlign::BASELINE_LEFT,
                                      this->text.c_str());
#endif
        if (this->mode == MODE_ICON_PROGRESS)
        {
          if (this->icon != BLANKICON)
          {
            this->config_->display->line(8, this->ypos(), 8, this->ypos() + 7, esphome::display::COLOR_OFF);
            if (this->icon == SOLIDICON)
            {
              this->config_->display->filled_rectangle(0, this->ypos(), 8, 8, this->config_->solid_color); 
            }
            else if (this->icon == CALENDARICON)
            {
              this->config_->display->filled_rectangle(0, this->ypos(), 8, 8, Color(C_RED, C_GREEN, C_BLUE)); 
              this->config_->display->filled_rectangle(0, this->ypos(), 8, 2, this->config_->calendar_color); 
            }
            else
            {
              this->config_->display->image(0, this->ypos(), this->config_->icons[this->icon]);
            }
          }

          this->config_->display->line(9, this->ypos() + 7, 31, this->ypos() + 7, this->progressbar_back_color);
          if (this->progress != 0)
          {
            if (this->progressbar_color == esphome::display::COLOR_OFF)
            {
              color_ = esphome::light::ESPHSVColor(this->progress * 96 / 100 + (this->progress < 0 ? 96 : 0), 255, 240).to_rgb();
            }
            else
            {
              color_ = this->progressbar_color;
            }
            this->config_->display->line(9, this->ypos() + 7, 9 + abs(this->progress) * 22 / 100, this->ypos() + 7, color_);
          }
        }
        else if (this->mode == MODE_PROGNOSIS_SCREEN)
        {
          if (this->icon != BLANKICON)
          {
            this->config_->display->line(8, this->ypos(), 8, this->ypos() + 7, esphome::display::COLOR_OFF);
            if (this->icon == SOLIDICON)
            {
              this->config_->display->filled_rectangle(0, this->ypos(), 8, 8, this->config_->solid_color); 
            }
            else if (this->icon == CALENDARICON)
            {
              this->config_->display->filled_rectangle(0, this->ypos(), 8, 8, Color(C_RED, C_GREEN, C_BLUE)); 
              this->config_->display->filled_rectangle(0, this->ypos(), 8, 2, this->config_->calendar_color); 
            }
            else
            {
              this->config_->display->image(0, this->ypos(), this->config_->icons[this->icon]);
            }
          }

          if (this->sbitmap != NULL)
          {
            for (uint8_t x = 0; x < 24; x++)
            {
              this->config_->display->draw_pixel_at(8 + x, this->ypos() + 7, this->sbitmap[x]);
            }
          }
        }
        else
        {
          if (this->config_->display_gauge)
          {
            if (this->icon != BLANKICON)
            {
              if (this->icon == SOLIDICON)
              {
                this->config_->display->filled_rectangle(2, this->ypos(), 8, 8, this->config_->solid_color); 
              }
              else if (this->icon == CALENDARICON)
              {
                this->config_->display->filled_rectangle(2, this->ypos(), 8, 8, Color(C_RED, C_GREEN, C_BLUE)); 
                this->config_->display->filled_rectangle(2, this->ypos(), 8, 2, this->config_->calendar_color); 
              }
              else
              {
                this->config_->display->image(2, this->ypos(), this->config_->icons[this->icon]);
              }
            }
            this->config_->display->line(10, this->ypos(), 10, this->ypos() + 7, esphome::display::COLOR_OFF);
          }
          else
          {
            this->config_->display->line(8, this->ypos(), 8, this->ypos() + 7, esphome::display::COLOR_OFF);
            if (this->icon != BLANKICON)
            {
              if (this->icon == SOLIDICON)
              {
                this->config_->display->filled_rectangle(0, this->ypos(), 8, 8, this->config_->solid_color); 
              }
              else if (this->icon == CALENDARICON)
              {
                this->config_->display->filled_rectangle(0, this->ypos(), 8, 8, Color(C_RED, C_GREEN, C_BLUE)); 
                this->config_->display->filled_rectangle(0, this->ypos(), 8, 2, this->config_->calendar_color); 
              }
              else
              {
                this->config_->display->image(0, this->ypos(), this->config_->icons[this->icon]);
              }
            }
          }
        }
        break;

      case MODE_TEXT_PROGRESS:
        color_ = this->text_color;

        this->config_->display->print(0, this->ypos() + yoffset, font, color_, esphome::display::TextAlign::BASELINE_LEFT, this->icon_name.c_str());

        this->config_->display->line(0, this->ypos() + 7, 31, this->ypos() + 7, this->progressbar_back_color);
        if (this->progress != 0)
        {
          if (this->progressbar_color == esphome::display::COLOR_OFF)
          {
            color_ = esphome::light::ESPHSVColor(this->progress * 120 / 100 + (this->progress < 0 ? 120 : 0), 255, 240).to_rgb();
          }
          else
          {
            color_ = this->progressbar_color;
          }
          this->config_->display->line(0, 7, abs(this->progress) * 31 / 100, 7, color_);
        }

        if (this->icon == 0)
        {
          color_ = this->text_color;
        }
        this->config_->display->print(32, this->ypos() + yoffset, font, color_, esphome::display::TextAlign::BASELINE_RIGHT, this->text.c_str());
        break;

      case MODE_ICON_TEXT_SCREEN:
      case MODE_RAINBOW_ICON_TEXT_SCREEN:
        color_ = (this->mode == MODE_RAINBOW_ICON_TEXT_SCREEN) ? this->config_->rainbow_color : this->text_color;
#ifdef EHMTXv2_USE_RTL
        this->config_->display->print(this->xpos() + xoffset, this->ypos() + yoffset, font, color_, esphome::display::TextAlign::BASELINE_RIGHT,
                                      this->text.c_str());
#else
        this->config_->display->print(this->xpos() + xoffset, this->ypos() + yoffset, font, color_, esphome::display::TextAlign::BASELINE_LEFT,
                                      this->text.c_str());
#endif
        if (this->icon != BLANKICON)
        {
          int x = 0;
          if (this->pixels_ > 23)
          {
            if (this->xpos() > 23)
            {
              x = 24 - this->xpos();
            }
            else
            {
              if (this->xpos() < 9)
              {
                x = this->xpos() - 9;
              }
            }
          }
          this->config_->display->line(x + 8, this->ypos(), x + 8, this->ypos() + 7, esphome::display::COLOR_OFF);
          if (this->icon == SOLIDICON)
          {
            this->config_->display->filled_rectangle(x, this->ypos(), 8, 8, this->config_->solid_color); 
          }
          else if (this->icon == CALENDARICON)
          {
            this->config_->display->filled_rectangle(x, this->ypos(), 8, 8, Color(C_RED, C_GREEN, C_BLUE)); 
            this->config_->display->filled_rectangle(x, this->ypos(), 8, 2, this->config_->calendar_color); 
          }
          else
          {
            this->config_->display->image(x, this->ypos(), this->config_->icons[this->icon]);
          }
        }
        break;

      case MODE_TEXT_SCREEN:
      case MODE_RAINBOW_TEXT:
        color_ = (this->mode == MODE_RAINBOW_TEXT) ? this->config_->rainbow_color : this->text_color;
#ifdef EHMTXv2_USE_RTL
        this->config_->display->print(this->xpos() + xoffset, this->ypos() + yoffset, font, color_, esphome::display::TextAlign::BASELINE_RIGHT,
                                      this->text.c_str());
#else
        this->config_->display->print(this->xpos() + xoffset, this->ypos() + yoffset, font, color_, esphome::display::TextAlign::BASELINE_LEFT,
                                      this->text.c_str());
#endif
        break;

      case MODE_BITMAP_STACK_SCREEN:
#ifndef USE_ESP8266
        if (this->sbitmap != NULL)
        {
          for (uint8_t i = 0; i < this->icon; i++)
          {
            if (this->sbitmap[i].b != BLANKICON)
            {
              if (this->icon == SOLIDICON)
              {
                this->config_->display->filled_rectangle(this->xpos(i), this->ypos(i), 8, 8, this->config_->solid_color); 
              }
              else if (this->icon == CALENDARICON)
              {
                this->config_->display->filled_rectangle(this->xpos(i), this->ypos(), 8, 8, Color(C_RED, C_GREEN, C_BLUE)); 
                this->config_->display->filled_rectangle(this->xpos(i), this->ypos(), 8, 2, this->config_->calendar_color); 
              }
              else
              {
                this->config_->display->image(this->xpos(i), this->ypos(i), this->config_->icons[this->sbitmap[i].b]);
              }
            }
          }
        }
#endif
        break;

#ifdef USE_Fireplugin
      case MODE_FIRE:
      {
        int16_t x = 0;
        int16_t y = 0;

        for (x = 0; x < 32; ++x)
        {
          /* Step 1) Cool down every cell a little bit */
          for (y = 0; y < 8; ++y)
          {
            uint8_t coolDownTemperature = random(0, ((COOLING * 10U) / 8)) + 2U;
            uint32_t heatPos = x + y * 32;

            if (coolDownTemperature >= m_heat[heatPos])
            {
              m_heat[heatPos] = 0U;
            }
            else
            {
              m_heat[heatPos] -= coolDownTemperature;
            }
          }

          /* Step 2) Heat from each cell drifts 'up' and diffuses a little bit */
          for (y = 0; y < (8 - 1U); ++y)
          {
            uint16_t diffusHeat = 0U;

            if ((8 - 2U) > y)
            {
              diffusHeat += m_heat[x + (y + 1) * 32];
              diffusHeat += m_heat[x + (y + 1) * 32];
              diffusHeat += m_heat[x + (y + 2) * 32];
              diffusHeat /= 3U;
            }
            else
            {
              diffusHeat += m_heat[x + (y + 0) * 32];
              diffusHeat += m_heat[x + (y + 0) * 32];
              diffusHeat += m_heat[x + (y + 1) * 32];
              diffusHeat /= 3U;
            }

            m_heat[x + y * 32] = diffusHeat;
          }

          /* Step 3) Randomly ignite new 'sparks' of heat near the bottom */
          if (random(0, 255) < SPARKING)
          {
            uint8_t randValue = random(160, 255);
            uint32_t heatPos = x + (8 - 1U) * 32;
            uint16_t heat = m_heat[heatPos] + randValue;

            if (UINT8_MAX < heat)
            {
              m_heat[heatPos] = 255U;
            }
            else
            {
              m_heat[heatPos] = heat;
            }
          }

          /* Step 4) Map from heat cells to LED colors */
          for (y = 0; y < 8; ++y)
          {
            this->config_->display->draw_pixel_at(x, this->ypos() + y, heatColor(m_heat[x + y * 32]));
          }
        }
      }
      break;
#endif

      default:
        ESP_LOGD(TAG, "no screen to draw!");
        this->config_->next_action_time = 0.0;
        break;
      }
      this->update_screen();
    }
  }

  void EHMTX_queue::hold_slot(uint8_t _sec)
  {
    this->endtime += 1000.0 * _sec;
    ESP_LOGD(TAG, "hold for %d secs", _sec);
  }

  // TODO void EHMTX_queue::set_mode_icon()

  // Screen Text, Screen time in seconds
  void EHMTX_queue::calc_scroll_time(std::string text, uint16_t screen_time)
  {
    int x, y, w, h;
    float display_duration;
    float requested_time = 1000.0 * screen_time;

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
    case MODE_TEXT_SCREEN:
    case MODE_RAINBOW_TEXT:
#ifdef EHMTXv2_SCROLL_SMALL_TEXT
      max_steps = EHMTXv2_SCROLL_COUNT * (width - startx) + EHMTXv2_SCROLL_COUNT * this->pixels_;
      display_duration = static_cast<float>(max_steps * EHMTXv2_SCROLL_INTERVALL);
      this->screen_time_ = (display_duration > requested_time) ? display_duration : requested_time;
#else
      if (this->pixels_ < 32)
      {
        this->screen_time_ = requested_time;
      }
      else
      {
        max_steps = EHMTXv2_SCROLL_COUNT * (width - startx) + EHMTXv2_SCROLL_COUNT * this->pixels_;
        display_duration = static_cast<float>(max_steps * EHMTXv2_SCROLL_INTERVALL);
        this->screen_time_ = (display_duration > requested_time) ? display_duration : requested_time;
      }
#endif
      break;
    case MODE_RAINBOW_ICON:
    case MODE_BITMAP_SMALL:
    case MODE_RAINBOW_BITMAP_SMALL:
    case MODE_ICON_SCREEN:
    case MODE_ALERT_SCREEN:
    case MODE_RAINBOW_ALERT_SCREEN:
    case MODE_ICON_PROGRESS:
    case MODE_PROGNOSIS_SCREEN:
      startx = 8;
      if (this->pixels_ < 23)
      {
        this->screen_time_ = requested_time;
      }
      else
      {
        max_steps = EHMTXv2_SCROLL_COUNT * (width - startx) + EHMTXv2_SCROLL_COUNT * this->pixels_;
        display_duration = static_cast<float>(max_steps * EHMTXv2_SCROLL_INTERVALL);
        this->screen_time_ = (display_duration > requested_time) ? display_duration : requested_time;
      }
      break;
    case MODE_ICON_TEXT_SCREEN:
    case MODE_RAINBOW_ICON_TEXT_SCREEN:
      if (this->pixels_ < 23)
      {
        this->screen_time_ = requested_time;
      }
      else
      {
        max_steps = EHMTXv2_SCROLL_COUNT * (width - startx) + EHMTXv2_SCROLL_COUNT * this->pixels_;
        display_duration = static_cast<float>(max_steps * EHMTXv2_SCROLL_INTERVALL);
        this->screen_time_ = (display_duration > requested_time) ? display_duration : requested_time;
      }
      break;
    default:
      break;
    }

    this->scroll_reset = (width - startx) + this->pixels_;
    ;

    ESP_LOGD(TAG, "calc_scroll_time: mode: %d text: \"%s\" pixels %d calculated: %.1f defined: %d max_steps: %d", this->mode, text.c_str(), this->pixels_, this->screen_time_ / 1000.0, screen_time, this->scroll_reset);
  }

  // Icons count, Screen time in seconds
  void EHMTX_queue::calc_scroll_time(uint8_t icon_count, uint16_t screen_time)
  {
    float display_duration;
    float requested_time = 1000.0 * screen_time;

    uint8_t width = 32;
    uint8_t startx = 0;
    uint16_t max_steps = 0;

    this->pixels_ = 9 * icon_count;

    if (this->pixels_ < 32)
    {
      this->screen_time_ = requested_time;
    }
    else
    {
      max_steps = EHMTXv2_SCROLL_COUNT * (width - startx) + EHMTXv2_SCROLL_COUNT * this->pixels_;
      display_duration = static_cast<float>(max_steps * EHMTXv2_SCROLL_INTERVALL);
      this->screen_time_ = (display_duration > requested_time) ? display_duration : requested_time;
    }

    this->scroll_reset = (width - startx) + this->pixels_;

    ESP_LOGD(TAG, "calc_scroll_time: mode: %d icons count: %d pixels %d calculated: %.1f defined: %d max_steps: %d", this->mode, icon_count, this->pixels_, this->screen_time_ / 1000.0, screen_time, this->scroll_reset);
  }

}
