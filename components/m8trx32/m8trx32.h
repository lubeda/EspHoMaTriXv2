#ifndef M8X32_H
#define M8X32_H
#include "esphome.h"
#include <list>

static const char *const M8X32_VERSION = "2023.9.0";
static const char *const TAG = "M8X32";

enum show_mode : uint8_t
{
  MODE_EMPTY = 0,
  MODE_BLANK = 1,
  MODE_CLOCK = 2,
  MODE_DATE = 3,
  MODE_FULL_SCREEN = 4,
  MODE_ICON_SCREEN = 5,
  MODE_TEXT_SCREEN = 6,
  MODE_RAINBOW_ICON = 7,
  MODE_RAINBOW_TEXT = 8,
  MODE_RAINBOW_CLOCK = 9,
  MODE_RAINBOW_DATE = 10,
  MODE_BITMAP_SCREEN = 11,
  MODE_BITMAP_SMALL = 12,
  MODE_COLOR = 13,
  MODE_FIRE = 14,
  MODE_ICON_CLOCK = 15,
  MODE_ALERT_SCREEN = 16,
  MODE_GRAPH_SCREEN = 17
};

class BaseScreen {
public:
    BaseScreen(const std::string& name,uint16_t lifetime, uint16 screen_time, show_mode mode )  {
        name_= name;
        lifetime_= lifetime;
        screen_time_= screen_time;
        mode_ = mode;
    }
    
    virtual void draw() {
        ESP_LOGD(TAG, "base draw");
    }
    virtual void log() {
        ESP_LOGD(TAG, "base log %s", this->name);
    }
    virtual void update() {
        ESP_LOGD(TAG, "base update");
    }

private:
    std::string name_;
    uint16_t lifetime_=5;
    uint16_t screen_time_=10;
    show_mode mode_;
};

class ClockScreen : public BaseScreen {
public:
    ClockScreen(const std::string& name,uint16_t lifetime, uint16 screen_time, bool default_font, Color color )  
    {
       BaseScreen( name, lifetime, screen_time, MODE_CLOCK )  ;
       default_font_ = default_font;
       color_ = color;
    }

    private:
        bool default_font_;
        Color color_;
    
};

class ColorScreen : public BaseScreen {
public:
    ColorScreen(const std::string& name,uint16_t lifetime, uint16 screen_time, show_mode mode, bool default_font, Color color )  
    {
       BaseScreen( name, lifetime, screen_time, MODE_COLOR )  ;
       default_font_ = default_font;
       color_ = color;
    }

    private:
        bool default_font_;
        Color color_;
};


//    std::list<MyBaseClass*> classList;


  class M8TRX32 : public PollingComponent, public api::CustomAPIDevice
  {
  protected:
    float get_setup_priority() const override { return esphome::setup_priority::LATE; }
    uint8_t brightness_;
    std::list<BaseScreen*> screenList;
    
    std::vector<M8TRX32NextScreenTrigger *> on_next_screen_triggers_;
    std::vector<M8TRX32EmptyQueueTrigger *> on_empty_queue_triggers_;
    std::vector<M8TRX32IconErrorTrigger *> on_icon_error_triggers_;
    std::vector<M8TRX32ExpiredScreenTrigger *> on_expired_screen_triggers_;
    std::vector<M8TRX32NextClockTrigger *> on_next_clock_triggers_;
    std::vector<M8TRX32StartRunningTrigger *> on_start_running_triggers_;
    std::vector<M8TRX32AddScreenTrigger *> on_add_screen_triggers_;

  public:
    void setup() override;
    M8TRX32();

    uint16_t hue_ = 0;
    void dump_config();
    PROGMEM Color text_color, alarm_color, rindicator_color,  lindicator_color, today_color, weekday_color, rainbow_color, clock_color;
    PROGMEM Color bitmap[256];
    PROGMEM Color sbitmap[64];
    PROGMEM Color cgauge[8];

    display::BaseFont *default_font;
    display::BaseFont *special_font;
    uint8_t ticks_per_second=62;
    bool is_running = false;

    addressable_light::AddressableLightDisplay *display;
    esphome::time::RealTimeClock *clock;

    uint8_t icon_count; // max iconnumber -1

    time_t next_action_time = 0; // when is the next screen change
    uint32_t tick_next_action = 0; // when is the next screen change
    uint32_t ticks_ = 0; // when is the next screen change

    void add_icon(M8TRX32_Icon *icon);
    uint8_t find_icon(std::string name);
    uint8_t find_last_clock();
    bool string_has_ending(std::string const &fullString, std::string const &ending);
    void draw_day_of_week(bool small=false);
    void tick();

    void add_on_next_screen_trigger(M8TRX32NextScreenTrigger *t) { this->on_next_screen_triggers_.push_back(t); }
    void add_on_empty_queue_trigger(M8TRX32EmptyQueueTrigger *t) { this->on_empty_queue_triggers_.push_back(t); }
    void add_on_add_screen_trigger(M8TRX32AddScreenTrigger *t) { this->on_add_screen_triggers_.push_back(t); }
    void add_on_icon_error_trigger(M8TRX32IconErrorTrigger *t) { this->on_icon_error_triggers_.push_back(t); }
    void add_on_expired_screen_trigger(M8TRX32ExpiredScreenTrigger *t) { this->on_expired_screen_triggers_.push_back(t); }
    void add_on_next_clock_trigger(M8TRX32NextClockTrigger *t) { this->on_next_clock_triggers_.push_back(t); }
    void add_on_start_running_trigger(M8TRX32StartRunningTrigger *t) { this->on_start_running_triggers_.push_back(t); }
  };

    
class M8TRX32_Icon : public animation::Animation
  {
  protected:
    bool counting_up;

  public:
    M8TRX32_Icon(const uint8_t *data_start, int width, int height, uint32_t animation_frame_count, esphome::image::ImageType type, std::string icon_name, bool revers, uint16_t frame_duration);
    std::string name;
    uint16_t frame_duration;
    void next_frame();
    bool reverse;
  };
}
