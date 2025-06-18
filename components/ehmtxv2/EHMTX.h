#ifndef EHMTX_H
#define EHMTX_H
#include "esphome.h"

#include "esphome/components/time/real_time_clock.h"

#ifndef F
  #define F(x) (x)
#endif

#if defined CONFIG_IDF_TARGET_ESP32S3 || defined CONFIG_IDF_TARGET_ESP32
  #pragma warning ( "With IDF-Framework no Fire") 
  #undef USE_Fireplugin
#endif

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
const uint8_t C_BLACK = 0;

const uint8_t D_LIFETIME = 5;
const uint8_t D_SCREEN_TIME = 10;

const uint8_t BLANKICON = MAXICONS + 1;
const uint8_t SOLIDICON = MAXICONS + 3;
const uint8_t CALENDARICON = MAXICONS + 5;
const uint8_t TEXTSCROLLSTART = 8;
const uint8_t TEXTSTARTOFFSET = (32 - 8);

const uint16_t POLLINGINTERVAL = 250;
static const char *const EHMTX_VERSION = "2025.6.0";
static const char *const TAG = "EHMTXv2";

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
  MODE_GRAPH_SCREEN = 17,
  MODE_ICON_DATE = 18,
  MODE_ICON_PROGRESS = 19,
  MODE_RAINBOW_BITMAP_SMALL = 20,
  MODE_ICON_TEXT_SCREEN = 21,
  MODE_RAINBOW_ICON_TEXT_SCREEN = 22,
  MODE_BITMAP_STACK_SCREEN = 23,
  MODE_TEXT_PROGRESS = 24,
  MODE_PROGNOSIS_SCREEN = 25,
  MODE_RAINBOW_ALERT_SCREEN = 26
};

namespace esphome
{
  class EHMTX_queue;
  class EHMTX_Icon;
  class EHMTXNextScreenTrigger;
  class EHMTXEmptyQueueTrigger;
  class EHMTXAddScreenTrigger;
  class EHMTXIconErrorTrigger;
  class EHMTXExpiredScreenTrigger;
  class EHMTXNextClockTrigger;
  class EHMTXStartRunningTrigger;
  class EHMTXShowDisplayTrigger;
  class EHMTXNightModeTrigger;

  class EHMTX : public PollingComponent, public api::CustomAPIDevice
  {
  protected:
    float get_setup_priority() const override { return esphome::setup_priority::LATE; }
    uint8_t brightness_ = 0;
    uint8_t target_brightness_ = 0;
    uint32_t boot_anim = 0;
    uint8_t screen_pointer;
    bool show_day_of_week;

    std::vector<EHMTXNextScreenTrigger *> on_next_screen_triggers_;
    std::vector<EHMTXEmptyQueueTrigger *> on_empty_queue_triggers_;
    std::vector<EHMTXIconErrorTrigger *> on_icon_error_triggers_;
    std::vector<EHMTXExpiredScreenTrigger *> on_expired_screen_triggers_;
    std::vector<EHMTXNextClockTrigger *> on_next_clock_triggers_;
    std::vector<EHMTXStartRunningTrigger *> on_start_running_triggers_;
    std::vector<EHMTXAddScreenTrigger *> on_add_screen_triggers_;
    std::vector<EHMTXShowDisplayTrigger *> on_show_display_triggers_;
    std::vector<EHMTXNightModeTrigger *> on_night_mode_triggers_;
    EHMTX_queue *find_icon_queue_element(uint8_t icon);
    EHMTX_queue *find_mode_queue_element(uint8_t mode);
    EHMTX_queue *find_mode_icon_queue_element(uint8_t mode, std::string name);
    EHMTX_queue *find_free_queue_element();

  public:
    void setup() override;
    EHMTX();

#ifdef USE_Fireplugin
    void fire_screen( int32_t lifetime, int32_t screen_time);
#endif    
    uint8_t hue_ = 0;
    void dump_config();
    bool info_font = true;
    int8_t info_y_offset = 0;
    uint8_t icon_to_9 = 0;
  #ifdef EHMTXv2_ICINDICATOR
    int8_t icon_indicator_y_pos = 7;
    uint8_t icon_indicator_height = 1;
  #endif
  #ifdef EHMTXv2_ADV_CLOCK
    bool info_clock_font = true;
    int8_t info_clock_y_offset = 0;
    bool info_date_font = true;
    int8_t info_date_y_offset = 0;
  #endif
#ifdef USE_ESP32
  PROGMEM Color text_color, alarm_color, today_color, weekday_color, rainbow_color, clock_color, info_lcolor, info_rcolor, solid_color, calendar_color;
  #ifdef EHMTXv2_RCINDICATOR
    PROGMEM Color rcindicator_color;
  #endif
  #ifdef EHMTXv2_RBINDICATOR
    PROGMEM Color rbindicator_color;
  #endif
  #ifdef EHMTXv2_LTINDICATOR
    PROGMEM Color ltindicator_color;
  #endif
  #ifdef EHMTXv2_LCINDICATOR
    PROGMEM Color lcindicator_color;
  #endif
  #ifdef EHMTXv2_LBINDICATOR
    PROGMEM Color lbindicator_color;
  #endif
  #ifndef EHMTXv2_ADV_BITMAP
    PROGMEM Color bitmap[256];
  #endif
  #ifdef EHMTXv2_GAUGE
    PROGMEM Color cgauge[8];
  #endif
  #ifdef EHMTXv2_ICINDICATOR
    PROGMEM Color icon_indicator_color;
  #endif
    PROGMEM EHMTX_Icon *icons[MAXICONS];
  #ifdef EHMTXv2_ADV_CLOCK
    PROGMEM Color hour_color, minutes_color, spacer_color, info_clock_lcolor, info_clock_rcolor, info_date_lcolor, info_date_rcolor;
  #endif
  #ifdef EHMTXv2_ADV_BOOT
  #if defined EHMTXv2_ADV_BOOT_MODE_0 || defined EHMTXv2_ADV_BOOT_MODE_1
        Color*  boot_logo;
      #else
        uint8_t* boot_logo;
      #endif
  #endif
#endif

#ifdef USE_ESP8266
    Color text_color, alarm_color, today_color, weekday_color, rainbow_color, clock_color, info_lcolor, info_rcolor, solid_color, calendar_color;
    EHMTX_Icon *icons[MAXICONS];
  #ifdef EHMTXv2_ADV_CLOCK
    Color hour_color, minutes_color, spacer_color, info_clock_lcolor, info_clock_rcolor, info_date_lcolor, info_date_rcolor;
  #endif
   #ifdef EHMTXv2_RCINDICATOR
    Color rcindicator_color;
  #endif
  #ifdef EHMTXv2_RBINDICATOR
    Color rbindicator_color;
  #endif
  #ifdef EHMTXv2_LTINDICATOR
    Color ltindicator_color;
  #endif
  #ifdef EHMTXv2_LCINDICATOR
    Color lcindicator_color;
  #endif
  #ifdef EHMTXv2_LBINDICATOR
    Color lbindicator_color;
  #endif
  #ifndef EHMTXv2_ADV_BITMAP
    Color bitmap[256];
  #endif
  #ifdef EHMTXv2_GAUGE
    uint8_t gauge_value;
    Color gauge_color, gauge_bgcolor;
  #endif
  #ifdef EHMTXv2_ICINDICATOR
    Color icon_indicator_color;
  #endif
#endif
    display::BaseFont *default_font;
    display::BaseFont *special_font;
    uint8_t ticks_per_second=62;
    bool is_running = false;
    #ifdef EHMTXv2_GAUGE
      bool display_gauge;
    #endif    

    //    | Empty (4)
    //    |       | Icon indicator (8)
    //    |       |  | Left Bottom Indicator (3) - Left
    //    |       |  |  | Left Center Indicator (4)
    //    |       |  |  |   | Left Top Indicator (3)
    //    |       |  |  |   |  | Right Bottom Indicator (3) - Right
    //    |       |  |  |   |  |   | Right Center Indicator (4)
    //    |       |  |  |   |  |   |  | Right Top Indicator (3) - Alarm
    // 00010000000100100100010010001001
    uint32_t display_indicator;


    uint16_t clock_time;
    uint16_t scroll_step;

    EHMTX_queue *queue[MAXQUEUE];
    addressable_light::AddressableLightDisplay *display;
    esphome::time::RealTimeClock *clock;
    #ifdef USE_GRAPH
      void graph_screen(int32_t lifetime = D_LIFETIME, int32_t screen_time = D_SCREEN_TIME);
      void icon_graph_screen(std::string icon, int32_t lifetime = D_LIFETIME, int32_t screen_time = D_SCREEN_TIME);
      graph::Graph *graph;
    #endif

    bool show_seconds;

    uint8_t icon_count; // max iconnumber -1
    unsigned long last_scroll_time;
    unsigned long last_rainbow_time;
    unsigned long last_anim_time;
    float next_action_time = 0.0; // when is the next screen change
    uint32_t tick_next_action = 0; // when is the next screen change
    uint32_t ticks_ = 0; // when is the next screen change
    #ifdef EHMTXv2_USE_VERTICAL_SCROLL
      bool vertical_scroll = false;
    #endif

    void remove_expired_queue_element();
    uint8_t find_oldest_queue_element();
    uint8_t queue_count();
    uint8_t find_icon_in_queue(std::string);
    void force_screen(std::string name, int32_t mode = MODE_ICON_SCREEN);
    void add_icon(EHMTX_Icon *icon);
    bool show_display = false;
    bool night_mode = false;
    bool weekday_accent = false;
    uint8_t find_icon(std::string name);
    uint8_t find_last_clock();
    bool string_has_ending(std::string const &fullString, std::string const &ending);
    void draw_day_of_week(int32_t ypos = 0, bool small = false);
    void show_all_icons();
    float get_tick();
    void tick_next_screen(float ts);
    void tick();
    void draw();
    void get_status();
    void queue_status();
    void skip_screen();
    void hold_screen(int32_t t = 30);
    void set_display(addressable_light::AddressableLightDisplay *disp);
    void set_clock_time(uint16_t t = 10);
    void set_show_day_of_week(bool b=true);
    void set_show_seconds(bool b=false);
    void set_brightness(int32_t b);
    #ifdef EHMTXv2_ADV_BITMAP
      void set_brightness_silent(int32_t b);
    #endif
    void set_display_on();
    void set_display_off();
    void set_night_mode_off();
    void set_night_mode_on();
    void set_weekday_accent_off();
    void set_weekday_accent_on();
    void expand_icon_to_9(int32_t mode=0);
    void set_clock(esphome::time::RealTimeClock *clock);
    #ifdef USE_GRAPH
      void set_graph(esphome::graph::Graph *graph);
    #endif
    void set_default_font(display::BaseFont *font);
    void set_special_font(display::BaseFont *font);

  #ifdef USE_ESP32
    #ifdef EHMTXv2_ADV_BOOT
      void set_boot_logo(std::string logo = "");
    #endif
  #endif

     void show_alarm(int32_t r = CA_RED, int32_t g = CA_GREEN, int32_t b = CA_BLUE, int32_t s = 2);
  #ifdef EHMTXv2_RCINDICATOR
    void show_rcindicator(int32_t r = C_RED, int32_t g = C_GREEN, int32_t b = C_BLUE, int32_t s = 3);
  #endif
  #ifdef EHMTXv2_RBINDICATOR
    void show_rindicator(int32_t r = C_RED, int32_t g = C_GREEN, int32_t b = C_BLUE, int32_t s = 3);
  #endif
  #ifdef EHMTXv2_LTINDICATOR
    void show_ltindicator(int32_t r = C_RED, int32_t g = C_GREEN, int32_t b = C_BLUE, int32_t s = 3);
  #endif
  #ifdef EHMTXv2_LCINDICATOR
    void show_lcindicator(int32_t r = C_RED, int32_t g = C_GREEN, int32_t b = C_BLUE, int32_t s = 3);
  #endif
  #ifdef EHMTXv2_LBINDICATOR
    void show_lindicator(int32_t r = C_RED, int32_t g = C_GREEN, int32_t b = C_BLUE, int32_t s = 3);
  #endif
  #ifdef EHMTXv2_ICINDICATOR
    void show_icon_indicator(int32_t r = C_RED, int32_t g = C_GREEN, int32_t b = C_BLUE, int32_t s = 8, int32_t pos = 7, int32_t h = 1);
  #endif
    void hide_alarm();
  #ifdef EHMTXv2_RCINDICATOR
    void hide_rcindicator();
  #endif
  #ifdef EHMTXv2_RBINDICATOR
    void hide_rindicator();
  #endif
  #ifdef EHMTXv2_LTINDICATOR
    void hide_ltindicator();
  #endif
  #ifdef EHMTXv2_LCINDICATOR
    void hide_lcindicator();
  #endif
  #ifdef EHMTXv2_LBINDICATOR
    void hide_lindicator();
  #endif
  #ifdef EHMTXv2_ICINDICATOR
    void hide_icon_indicator();
  #endif  
    void set_text_color(int32_t r = C_RED, int32_t g = C_GREEN, int32_t b = C_BLUE);
    void set_today_color(int32_t r = C_RED, int32_t g = C_GREEN, int32_t b = C_BLUE);
    void set_weekday_color(int32_t r = CD_RED, int32_t g = CD_GREEN, int32_t b = CD_BLUE);
    void set_clock_color(int32_t r = C_RED, int32_t g = C_GREEN, int32_t b = C_BLUE);
    void set_infotext_color(int32_t lr = CG_GREY, int32_t lg = CG_GREY, int32_t lb = CG_GREY, int32_t rr = CG_GREY, int32_t rg = CG_GREY, int32_t rb = CG_GREY, bool info_font = true, int32_t y_offset = 0);
    void set_solid_color(int32_t r = C_RED, int32_t g = C_GREEN, int32_t b = C_BLUE);
    void set_calendar_color(int32_t r = C_RED, int32_t g = C_BLACK, int32_t b = C_BLACK);
    #ifdef EHMTXv2_ADV_CLOCK
      void set_clock_infotext_color(int32_t lr = CG_GREY, int32_t lg = CG_GREY, int32_t lb = CG_GREY, int32_t rr = CG_GREY, int32_t rg = CG_GREY, int32_t rb = CG_GREY, bool info_font = true, int32_t y_offset = 0);
      void set_date_infotext_color(int32_t lr = CG_GREY, int32_t lg = CG_GREY, int32_t lb = CG_GREY, int32_t rr = CG_GREY, int32_t rg = CG_GREY, int32_t rb = CG_GREY, bool info_font = true, int32_t y_offset = 0);
      void set_adv_clock_color(int32_t hr = C_BLACK, int32_t hg = C_BLACK, int32_t hb = C_BLACK, int32_t mr = C_BLACK, int32_t mg = C_BLACK, int32_t mb = C_BLACK, int32_t sr = C_BLACK, int32_t sg = C_BLACK, int32_t sb = C_BLACK);
      bool draw_clock(std::string format, esphome::display::BaseFont *font, Color color, int32_t xpos = 0, int32_t ypos = 0);
      bool draw_date(std::string format, esphome::display::BaseFont *font, Color color, int32_t xpos = 0, int32_t ypos = 0);
    #endif

    #ifdef EHMTXv2_GAUGE
      #ifndef USE_ESP8266
      void color_gauge(std::string text);
      #endif
      void show_gauge(int32_t v, int32_t r = C_RED, int32_t g = C_GREEN, int32_t b = C_BLUE,int32_t bgr = CG_GREY, int32_t bgg = CG_GREY, int32_t bgb = CG_GREY); // int32_t because of register_service
      void hide_gauge();
    #endif
    void full_screen(std::string icon, int32_t lifetime = D_LIFETIME, int32_t screen_time = D_SCREEN_TIME);
    void icon_screen(std::string icon, std::string text, int32_t lifetime = D_LIFETIME, int32_t screen_time = D_SCREEN_TIME, bool default_font = true, int32_t r = C_RED, int32_t g = C_GREEN, int32_t b = C_BLUE);
    void alert_screen(std::string icon, std::string text, int32_t screen_time = D_SCREEN_TIME, bool default_font = true, int32_t r = CA_RED, int32_t g = CA_GREEN, int32_t b = CA_BLUE);
    void rainbow_alert_screen(std::string icon, std::string text, int32_t screen_time = D_SCREEN_TIME, bool default_font = true);
    void icon_clock(std::string icon, int32_t lifetime = D_LIFETIME, int32_t screen_time = D_SCREEN_TIME, bool default_font = true, int32_t r = C_RED, int32_t g = C_GREEN, int32_t b = C_BLUE);
    void icon_date(std::string icon, int32_t lifetime = D_LIFETIME, int32_t screen_time = D_SCREEN_TIME, bool default_font = true, int32_t r = C_RED, int32_t g = C_GREEN, int32_t b = C_BLUE);
    void text_screen(std::string text, int32_t lifetime = D_LIFETIME, int32_t screen_time = D_SCREEN_TIME, bool default_font = true, int32_t r = C_RED, int32_t g = C_GREEN, int32_t b = C_BLUE);
    void clock_screen(int32_t lifetime = D_LIFETIME, int32_t screen_time = D_SCREEN_TIME, bool default_font = true, int32_t r = C_RED, int32_t g = C_GREEN, int32_t b = C_BLUE);
    void date_screen(int32_t lifetime = D_LIFETIME, int32_t screen_time = D_SCREEN_TIME, bool default_font = true, int32_t r = C_RED, int32_t g = C_GREEN, int32_t b = C_BLUE);
    void blank_screen(int32_t lifetime = D_LIFETIME, int32_t screen_time = D_SCREEN_TIME);
    void color_screen(int32_t lifetime = D_LIFETIME, int32_t screen_time = D_SCREEN_TIME, int32_t r = C_RED, int32_t g = C_GREEN, int32_t b = C_BLUE);
    void text_screen_progress(std::string text, std::string value, int32_t progress, int32_t lifetime = D_LIFETIME, int32_t screen_time = D_SCREEN_TIME, bool default_font = true, bool value_color_as_progress = false, int32_t r = C_RED, int32_t g = C_GREEN, int32_t b = C_BLUE);
    void icon_screen_progress(std::string icon, std::string text, int32_t progress, int32_t lifetime = D_LIFETIME, int32_t screen_time = D_SCREEN_TIME, bool default_font = true, int32_t r = C_RED, int32_t g = C_GREEN, int32_t b = C_BLUE);
    void set_progressbar_color(std::string icon, int32_t mode = MODE_ICON_PROGRESS, int32_t r = C_BLACK, int32_t g = C_BLACK, int32_t b = C_BLACK, int32_t bg_r = C_BLACK, int32_t bg_g = C_BLACK, int32_t bg_b = C_BLACK);

    void icon_text_screen(std::string icon, std::string text, int32_t lifetime = D_LIFETIME, int32_t screen_time = D_SCREEN_TIME, bool default_font = true, int32_t r = C_RED, int32_t g = C_GREEN, int32_t b = C_BLUE);
    void rainbow_icon_text_screen(std::string icon, std::string text, int32_t lifetime = D_LIFETIME, int32_t screen_time = D_SCREEN_TIME, bool default_font = true);

    void bitmap_stack(std::string icons, int32_t lifetime = D_LIFETIME, int32_t screen_time = D_SCREEN_TIME);

    void icon_prognosis_screen(std::string icon, std::string text, std::string prognosis, int32_t lifetime = D_LIFETIME, int32_t screen_time = D_SCREEN_TIME, bool default_font = true);
    void icon_prognosis_screen_rgb(std::string icon, std::string text, std::string prognosis, int32_t lifetime = D_LIFETIME, int32_t screen_time = D_SCREEN_TIME, bool default_font = true, int32_t r = C_RED, int32_t g = C_GREEN, int32_t b = C_BLUE);

    void bitmap_screen(std::string text, int32_t lifetime = D_LIFETIME, int32_t screen_time = D_SCREEN_TIME);
    
    void bitmap_small(std::string icon, std::string text, int32_t lifetime = D_LIFETIME, int32_t screen_time = D_SCREEN_TIME, bool default_font = true, int32_t r = C_RED, int32_t g = C_GREEN, int32_t b = C_BLUE);
    void rainbow_bitmap_small(std::string icon, std::string text, int32_t lifetime = D_LIFETIME, int32_t screen_time = D_SCREEN_TIME, bool default_font = true);
    void rainbow_icon_screen(std::string icon_name, std::string text, int32_t lifetime = D_LIFETIME, int32_t screen_time = D_SCREEN_TIME, bool default_font = true);
    void rainbow_text_screen(std::string text, int32_t lifetime = D_LIFETIME, int32_t screen_time = D_SCREEN_TIME, bool default_font = true);
    void rainbow_clock_screen(int32_t lifetime = D_LIFETIME, int32_t screen_time = D_SCREEN_TIME, bool default_font = true);
    void rainbow_date_screen(int32_t lifetime = D_LIFETIME, int32_t screen_time = D_SCREEN_TIME, bool default_font = true);
    void del_screen(std::string icon, int32_t mode = MODE_ICON_SCREEN);

    
    void draw_alarm();
      #ifdef EHMTXv2_RCINDICATOR
    void draw_rcindicator();
  #endif
  #ifdef EHMTXv2_RBINDICATOR
    void draw_rindicator();
     #endif
  #ifdef EHMTXv2_LTINDICATOR
    void draw_ltindicator();
  #endif
  #ifdef EHMTXv2_LCINDICATOR
    void draw_lcindicator();
  #endif
  #ifdef EHMTXv2_LBINDICATOR
    void draw_lindicator();
     #endif
  #ifdef EHMTXv2_ICINDICATOR 
    void draw_icon_indicator();
 #endif
  #ifdef EHMTXv2_GAUGE
    void draw_gauge();
  #endif
    #ifdef EHMTXv2_RAINBOW_SHIMMER
      void draw_rainbow_text(std::string text, esphome::display::BaseFont *font, int32_t xpos, int32_t ypos);
    #endif
    void draw_text(std::string text, esphome::display::BaseFont *font, Color color, int32_t xpos, int32_t ypos);

    void set_replace_time_date_active(bool b=false);
    void set_weekday_char_count(uint8_t i);
    bool replace_time_date_active;
    std::string replace_time_date(std::string time_date);
    uint8_t weekday_char_count;
    std::string GetWeekdayChar(int32_t position);
    std::string GetTextChar(std::string text, int32_t position);
    #ifdef EHMTXv2_RAINBOW_SHIMMER
      int32_t GetTextCharCount(std::string text);
    #endif

    int32_t GetTextBounds(esphome::display::BaseFont *font, const char *buffer);
    int32_t GetTextWidth(esphome::display::BaseFont *font, const char* formatting, const char raw_char);
    int32_t GetTextWidth(esphome::display::BaseFont *font, const char* formatting, const char *raw_text);
    int32_t GetTextWidth(esphome::display::BaseFont *font, const char* formatting, const int32_t raw_int);
    int32_t GetTextWidth(esphome::display::BaseFont *font, const char* formatting, const float raw_float);
    int32_t GetTextWidth(esphome::display::BaseFont *font, const char* formatting, esphome::ESPTime time);

    uint32_t IntToBits(uint32_t data, uint32_t newBitValues, unsigned nbits, unsigned startbit);
    uint32_t BitsToInt(uint32_t source, unsigned from, unsigned to);

    void add_on_next_screen_trigger(EHMTXNextScreenTrigger *t) { this->on_next_screen_triggers_.push_back(t); }
    void add_on_empty_queue_trigger(EHMTXEmptyQueueTrigger *t) { this->on_empty_queue_triggers_.push_back(t); }
    void add_on_add_screen_trigger(EHMTXAddScreenTrigger *t) { this->on_add_screen_triggers_.push_back(t); }
    void add_on_icon_error_trigger(EHMTXIconErrorTrigger *t) { this->on_icon_error_triggers_.push_back(t); }
    void add_on_expired_screen_trigger(EHMTXExpiredScreenTrigger *t) { this->on_expired_screen_triggers_.push_back(t); }
    void add_on_next_clock_trigger(EHMTXNextClockTrigger *t) { this->on_next_clock_triggers_.push_back(t); }
    void add_on_start_running_trigger(EHMTXStartRunningTrigger *t) { this->on_start_running_triggers_.push_back(t); }
    void add_on_show_display_trigger(EHMTXShowDisplayTrigger *t) { this->on_show_display_triggers_.push_back(t); }
    void add_on_night_mode_trigger(EHMTXNightModeTrigger *t) { this->on_night_mode_triggers_.push_back(t); }
    void update();

    uint8_t get_brightness();
  };

  class EHMTX_queue
  {
  protected:
    EHMTX *config_;
  public:
    uint16_t pixels_;
    float screen_time_;
    bool default_font;
    float endtime;
    float last_time;
    uint8_t icon;
    uint16_t scroll_reset;
    show_mode mode;
    int8_t progress;
    Color* sbitmap;
  #ifdef EHMTXv2_ADV_BITMAP
    Color* bitmap;
  #endif

#ifdef USE_ESP32
    PROGMEM Color text_color, progressbar_color, progressbar_back_color;
    PROGMEM std::string text;
    PROGMEM std::string icon_name;
#endif
#ifdef USE_ESP8266
    Color text_color, progressbar_color, progressbar_back_color;
    std::string text;
    std::string icon_name;
#endif

    EHMTX_queue(EHMTX *config);

    Color heatColor(uint8_t temperature);
    void status();

    void draw();
    bool isfree();
    bool update_slot(uint8_t _icon);
    void update_screen();
    void hold_slot(uint8_t _sec);
    void calc_scroll_time(std::string text, uint16_t screen_time);
    void calc_scroll_time(uint8_t icon_count, uint16_t screen_time);
    int32_t xpos();
    int32_t xpos(uint8_t item);
    int32_t ypos();
    int32_t ypos(uint8_t item);
  };

  class EHMTXNextScreenTrigger : public Trigger<std::string, std::string>
  {
  public:
    explicit EHMTXNextScreenTrigger(EHMTX *parent) { parent->add_on_next_screen_trigger(this); }
    void process(std::string, std::string);
  };

  class EHMTXEmptyQueueTrigger : public Trigger<>
  {
  public:
    explicit EHMTXEmptyQueueTrigger(EHMTX *parent) { parent->add_on_empty_queue_trigger(this); }
    void process();
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

 class EHMTXStartRunningTrigger : public Trigger<>
  {
  public:
    explicit EHMTXStartRunningTrigger(EHMTX *parent) { parent->add_on_start_running_trigger(this); }
    void process();
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

  class EHMTXShowDisplayTrigger : public Trigger<bool>
  {
  public:
    explicit EHMTXShowDisplayTrigger(EHMTX *parent) { parent->add_on_show_display_trigger(this); }
    void process(bool);
  };

  class EHMTXNightModeTrigger : public Trigger<bool>
  {
  public:
    explicit EHMTXNightModeTrigger(EHMTX *parent) { parent->add_on_night_mode_trigger(this); }
    void process(bool);
  };
/**
 * @brief Class for icon handling and type/animation control
 * 
 */
  class EHMTX_Icon : public animation::Animation
  {
  protected:
  /**
   * @brief for pinppong display of animations
   * 
   */
    bool counting_up;

  public:
  /**
   * @brief Construct a new ehmtx icon object
   * 
   * @param data_start 
   * @param width 
   * @param height 
   * @param animation_frame_count 
   * @param type 
   * @param icon_name 
   * @param revers play reverse
   * @param frame_duration time per frame
   * @param transparency
   */
    EHMTX_Icon(const uint8_t *data_start, int32_t width, int32_t height, uint32_t animation_frame_count, esphome::image::ImageType type, std::string icon_name, bool revers, uint16_t frame_duration, esphome::image::Transparency transparency);
#ifdef USE_ESP32
    PROGMEM std::string name;
#endif
#ifdef USE_ESP8266
    std::string name;
#endif
    uint16_t frame_duration;
    void next_frame();
    bool reverse;
  };
}

#endif
