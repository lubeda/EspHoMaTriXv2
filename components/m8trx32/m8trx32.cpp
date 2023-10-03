#include "esphome.h"

namespace esphome
{

    void M8TRX32::tick()
    {
        this->hue_++;
        if (this->hue_ == 360)
        {
            this->hue_ = 0;
        }
        
        if (this->clock->now().is_valid())
        {
            
            if (ts > this->next_action_time)
            {
                this->remove_expired_queue_element();
                this->screen_pointer = this->find_last_clock();
                this->scroll_step = 0;
                this->ticks_ = 0;

                if (this->screen_pointer == MAXQUEUE)
                {
                    this->screen_pointer = find_oldest_queue_element();
                }

                if (this->screen_pointer != MAXQUEUE)
                {
                    this->queue[this->screen_pointer]->last_time = ts + this->queue[this->screen_pointer]->screen_time_;
                    // todo nur bei animationen
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
                            ESP_LOGD(TAG, "on_next_screen trigger");
                            t->process(this->queue[this->screen_pointer]->icon_name, this->queue[this->screen_pointer]->text);
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
            if ((this->ticks_ <= EHMTXv2_BLEND_STEPS))
            {
                uint8_t b = this->brightness_;
                float br = lerp((float)this->ticks_ / EHMTXv2_BLEND_STEPS, 0, (float)b / 255);
                this->display->get_light()->set_correction(br, br, br);
            }
#endif
            this->ticks_++;
        }
        else
        {
            uint8_t w = 2 + ((uint8_t)(32 / 16) * (this->boot_anim / 16)) % 32;
            uint8_t l = 32 / 2 - w / 2;
            this->display->rectangle(l, 2, w, 4, this->rainbow_color);
            this->boot_anim++;
        }
    }

}