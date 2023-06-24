#ifndef EHMTX_H
#define EHMTX_H
#include "esphome.h"
#include "esphome/components/time/real_time_clock.h"

const uint8_t MAXQUEUE = 24;
const uint8_t C_RED = 240; // default
const uint8_t C_BLUE = 240;
const uint8_t C_GREEN = 240;
const uint8_t CD_RED = 100; // dim
const uint8_t CD_BLUE = 100;
const uint8_t CD_GREEN = 100;
const uint8_t CA_RED = 200; // alarm
const uint8_t CA_BLUE = 50;
const uint8_t CA_GREEN = 50;
const uint8_t CG_GREY = 50;

const uint8_t D_LIFETIME = 5;
const uint8_t D_SCREEN_TIME = 10;

const uint8_t MAXICONS = 90;
const uint8_t TEXTSCROLLSTART = 8;
const uint8_t TEXTSTARTOFFSET = (32 - 8);

const uint16_t POLLINGINTERVAL = 250;
static const char *const EHMTX_VERSION = "2023.6.5";
static const char *const TAG = "EHMTXv2";
#ifndef USE_ESP8266
static const char *const EHMTX_LOGO = "[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,63519,63519,63519,63519,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,63519,0,0,0,0,2016,0,0,0,0,0,0,0,0,0,0,31,0,0,0,0,0,0,0,0,0,63488,0,63488,0,0,0,63519,0,0,0,0,2016,2016,0,0,0,65514,0,65514,0,0,0,31,0,0,0,64512,0,0,64512,0,63488,63488,0,63488,63488,0,0,63519,63519,63519,0,0,2016,0,2016,0,65514,0,65514,0,65514,0,31,31,31,0,0,0,64512,64512,0,0,63488,63488,63488,63488,63488,0,0,63519,0,0,0,0,2016,0,2016,0,65514,0,65514,0,65514,0,0,31,0,0,0,0,64512,64512,0,0,0,63488,63488,63488,0,0,0,63519,63519,63519,63519,0,2016,0,2016,0,65514,0,65514,0,65514,0,0,0,31,31,0,64512,0,0,64512,0,0,0,63488,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]";
static const char *const EHMTX_SLOGO = "[2016,0,0,0,2016,0,0,0,2016,0,0,0,2016,0,0,0,2016,0,0,0,2016,0,0,0,0,2016,0,2016,0,31,31,0,0,0,2016,0,31,0,0,31,0,0,0,0,0,0,31,0,0,0,0,0,0,31,0,0,0,0,0,0,31,31,31,31]";
#endif
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
  MODE_BITMAP_SMALL = 12
};

namespace esphome
{
  class EHMTX_queue;
  class EHMTX_Icon;
  class EHMTXNextScreenTrigger;
  class EHMTXAddScreenTrigger;
  class EHMTXIconErrorTrigger;
  class EHMTXExpiredScreenTrigger;
  class EHMTXNextClockTrigger;

  class EHMTX : public PollingComponent, public api::CustomAPIDevice
  {
  protected:
    float get_setup_priority() const override { return esphome::setup_priority::LATE; }
    uint8_t brightness_;
    uint32_t boot_anim = 0;
    uint8_t screen_pointer;
    bool show_day_of_week;

    std::vector<EHMTXNextScreenTrigger *> on_next_screen_triggers_;
    std::vector<EHMTXIconErrorTrigger *> on_icon_error_triggers_;
    std::vector<EHMTXExpiredScreenTrigger *> on_expired_screen_triggers_;
    std::vector<EHMTXNextClockTrigger *> on_next_clock_triggers_;
    std::vector<EHMTXAddScreenTrigger *> on_add_screen_triggers_;
    EHMTX_queue *find_icon_queue_element(uint8_t icon);
    EHMTX_queue *find_free_queue_element();

  public:
    void setup() override;
    EHMTX();

    uint16_t hue_ = 0;
    void dump_config();
#ifdef USE_ESP32
    PROGMEM Color text_color, alarm_color, rindicator_color,  lindicator_color,clock_color, today_color, weekday_color, rainbow_color;
    PROGMEM Color bitmap[256];
    PROGMEM Color sbitmap[64];
    PROGMEM Color cgauge[8];
    PROGMEM EHMTX_Icon *icons[MAXICONS];
#endif

#ifdef USE_ESP8266
    Color text_color, alarm_color, gauge_color, gauge_bgcolor,rindicator_color,lindicator_color, clock_color, today_color, weekday_color, rainbow_color;
    EHMTX_Icon *icons[MAXICONS];
    uint8_t gauge_value;
#endif
    display::Font *default_font;
    display::Font *special_font;
    int display_rindicator;
    int display_lindicator;
    int display_alarm;
    uint8_t ticks_per_second=62;
    bool display_gauge;
    bool is_running = false;
    bool show_date;
    
    uint16_t clock_time;
    uint16_t scroll_step;

    EHMTX_queue *queue[MAXQUEUE];
    addressable_light::AddressableLightDisplay *display;
    esphome::time::RealTimeClock *clock;

    bool show_seconds;
    uint16_t hold_time; // seconds display of screen_time to extend

    uint8_t icon_count; // max iconnumber -1
    unsigned long last_scroll_time;
    unsigned long last_rainbow_time;
    unsigned long last_anim_time;
    time_t next_action_time = 0; // when is the next screen change
    uint32_t tick_next_action = 0; // when is the next screen change
    uint32_t ticks_ = 0; // when is the next screen change

    void remove_expired_queue_element();
    uint8_t find_oldest_queue_element();
    uint8_t find_icon_in_queue(std::string);
    void force_screen(std::string name, int mode = MODE_ICON_SCREEN);
    void add_icon(EHMTX_Icon *icon);
    bool show_display = false;
    uint8_t find_icon(std::string name);
    uint8_t find_last_clock();
    bool string_has_ending(std::string const &fullString, std::string const &ending);
    void draw_day_of_week();
    void show_all_icons();
    void tick();
    void draw();
    void get_status();
    void queue_status();
    void skip_screen();
    void hold_screen(int t = 30);
    void set_display(addressable_light::AddressableLightDisplay *disp);
    void set_hold_time(uint16_t t = 30);
    void set_clock_time(uint16_t t = 10);
    void set_show_day_of_week(bool b=true);
    void set_show_seconds(bool b=false);
    void set_show_date(bool b=true);
    void set_brightness(int b);
    void set_display_on();
    void set_display_off();
    void set_clock(esphome::time::RealTimeClock *clock);
    void set_default_font(display::Font *font);
    void set_special_font(display::Font *font);
    void show_rindicator(int r = C_RED, int g = C_GREEN, int b = C_BLUE, int s = 3);
    void show_lindicator(int r = C_RED, int g = C_GREEN, int b = C_BLUE, int s = 3);
    void set_clock_color(int r = C_RED, int g = C_GREEN, int b = C_BLUE);
    void set_today_color(int r, int g, int b);
    void set_weekday_color(int r, int g, int b);
    void show_alarm(int r = CA_RED, int g = CA_GREEN, int b = CA_BLUE, int s = 2);
    void show_gauge(int v, int r = C_RED, int g = C_GREEN, int b = C_BLUE,int bgr = CG_GREY, int bgg = CG_GREY, int bgb = CG_GREY); // int because of register_service
    void hide_gauge();
    void hide_rindicator();
    void hide_lindicator();
    void hide_alarm();
    void full_screen(std::string icon, int lifetime = D_LIFETIME, int screen_time = D_SCREEN_TIME);
    void icon_screen(std::string icon, std::string text, int lifetime = D_LIFETIME, int screen_time = D_SCREEN_TIME, bool default_font = true, int r = C_RED, int g = C_GREEN, int b = C_BLUE);
    void text_screen(std::string text, int lifetime = D_LIFETIME, int screen_time = D_SCREEN_TIME, bool default_font = true, int r = C_RED, int g = C_GREEN, int b = C_BLUE);
    void clock_screen(int lifetime = D_LIFETIME, int screen_time = D_SCREEN_TIME, bool default_font = true, int r = C_RED, int g = C_GREEN, int b = C_BLUE);
    void date_screen(int lifetime = D_LIFETIME, int screen_time = D_SCREEN_TIME, bool default_font = true, int r = C_RED, int g = C_GREEN, int b = C_BLUE);
    void blank_screen(int lifetime = D_LIFETIME, int screen_time = D_SCREEN_TIME);

    void bitmap_screen(std::string text, int lifetime = D_LIFETIME, int screen_time = D_SCREEN_TIME);
    void color_gauge(std::string text);
    void bitmap_small(std::string, std::string,int lifetime = D_LIFETIME, int screen_time = D_SCREEN_TIME, bool default_font = true, int r = C_RED, int g = C_GREEN, int b = C_BLUE);
    void rainbow_icon_screen(std::string icon_name, std::string text, int lifetime = D_LIFETIME, int screen_time = D_SCREEN_TIME, bool default_font = true);
    void rainbow_text_screen(std::string text, int lifetime = D_LIFETIME, int screen_time = D_SCREEN_TIME, bool default_font = true);
    void rainbow_clock_screen(int lifetime = D_LIFETIME, int screen_time = D_SCREEN_TIME, bool default_font = true);
    void rainbow_date_screen(int lifetime = D_LIFETIME, int screen_time = D_SCREEN_TIME, bool default_font = true);
    void del_screen(std::string icon, int mode = MODE_ICON_SCREEN);

    void draw_gauge();
    void draw_alarm();
    void draw_rindicator();
    void draw_lindicator();

    void add_on_next_screen_trigger(EHMTXNextScreenTrigger *t) { this->on_next_screen_triggers_.push_back(t); }
    void add_on_add_screen_trigger(EHMTXAddScreenTrigger *t) { this->on_add_screen_triggers_.push_back(t); }
    void add_on_icon_error_trigger(EHMTXIconErrorTrigger *t) { this->on_icon_error_triggers_.push_back(t); }
    void add_on_expired_screen_trigger(EHMTXExpiredScreenTrigger *t) { this->on_expired_screen_triggers_.push_back(t); }
    void add_on_next_clock_trigger(EHMTXNextClockTrigger *t) { this->on_next_clock_triggers_.push_back(t); }

    void update();
    uint8_t get_brightness();
  };

  class EHMTX_queue
  {
  protected:
    EHMTX *config_;

  public:
    uint16_t pixels_;
    uint16_t screen_time_;
    bool default_font;
    time_t endtime;
    time_t last_time;
    uint8_t icon;
    uint16_t scroll_reset;
    Color text_color;
    show_mode mode;

#ifdef USE_ESP32
    PROGMEM std::string text;
    PROGMEM std::string icon_name;
#endif
#ifdef USE_ESP8266
    std::string text;
    std::string icon_name;
#endif

    EHMTX_queue(EHMTX *config);

    void status();
    void draw();
    bool isfree();
    bool update_slot(uint8_t _icon);
    void update_screen();
    void hold_slot(uint8_t _sec);
    void calc_scroll_time(std::string, uint16_t);
    int xpos();
  };

  class EHMTXNextScreenTrigger : public Trigger<std::string, std::string>
  {
  public:
    explicit EHMTXNextScreenTrigger(EHMTX *parent) { parent->add_on_next_screen_trigger(this); }
    void process(std::string, std::string);
  };

  class EHMTXAddScreenTrigger : public Trigger<std::string, uint8_t>
  {
  public:
    explicit EHMTXAddScreenTrigger(EHMTX *parent) { parent->add_on_add_screen_trigger(this); }
    void process(std::string, uint8_t);
  };

  class EHMTXIconErrorTrigger : public Trigger<std::string>
  {
  public:
    explicit EHMTXIconErrorTrigger(EHMTX *parent) { parent->add_on_icon_error_trigger(this); }
    void process(std::string);
  };

  class EHMTXExpiredScreenTrigger : public Trigger<std::string, std::string>
  {
  public:
    explicit EHMTXExpiredScreenTrigger(EHMTX *parent) { parent->add_on_expired_screen_trigger(this); }
    void process(std::string, std::string);
  };

  class EHMTXNextClockTrigger : public Trigger<>
  {
  public:
    explicit EHMTXNextClockTrigger(EHMTX *parent) { parent->add_on_next_clock_trigger(this); }
    void process();
  };

  class EHMTX_Icon : public display::Animation
  {
  protected:
    bool counting_up;

  public:
    EHMTX_Icon(const uint8_t *data_start, int width, int height, uint32_t animation_frame_count, display::ImageType type, std::string icon_name, bool revers, uint16_t frame_duration);
    std::string name;
    uint16_t frame_duration;
    void next_frame();
    bool reverse;
  };
}

#endif
