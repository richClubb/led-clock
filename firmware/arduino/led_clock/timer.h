#ifndef __CLOCK_TIMER_H__

#define __CLOCK_TIMER_H__

#include "led_strip.h"

#define FLASH_COUNT 11
#define FLASH_DELAY 500

enum ClockTimerState{
  Idle,
  Active,
  Paused,
  Flashing
};

class ClockTimer
{
private:
  LedStrip *led_strip;
  byte col[3] = {0, 255, 0};
  float progress;
  unsigned int timer_time_millis;
  unsigned long start_time_millis;
  unsigned long elapsed_time;

  bool active = false;

  unsigned int flash_index = 0;
  unsigned long flash_time_last = 0;
  unsigned int flash_state_last = 0;
  bool flashing = false;

  ClockTimerState last_state = Idle;
  ClockTimerState state = Idle;

public:
  ClockTimer(LedStrip *led_strip, byte red, byte green, byte blue)
  {
    this->led_strip = led_strip;
    this->active = false;
    this->col[0] = red;
    this->col[1] = green;
    this->col[2] = blue;
    this->state = Idle;
  }

  void set_timer(unsigned long seconds)
  {
    this->timer_time_millis = 1000 * seconds;
  }

  void set_led_strip(LedStrip *led_strip)
  {
    this->led_strip = led_strip;
  }

  void set_colours(byte red, byte green, byte blue)
  {
    this->col[0] = red;
    this->col[1] = green;
    this->col[2] = blue;
  }

  // start displays the timer output on the led strips
  void start_timer()
  {
    this->start_time_millis = millis();
    this->state = Active;
    this->progress = 0;
  }

  // pause stops the timer from ticking but leaves the display state
  void pause_timer()
  { 
    this->state = Paused;
    if (this->last_state != Paused)
    {
      this->elapsed_time = millis() - this->start_time_millis;
    }
  }

  void resume_timer()
  {
    if (this->last_state == Paused)
    {
      this->start_time_millis = millis() - this->elapsed_time;
    }

    this->state = Active;
  }

  // reset clears the output
  void reset_timer()
  {
    this->state = Idle;
    return;
  }

  void state_idle()
  {
    this->last_state = Idle;
    return;
  }

  // In the flashing state the timer ring should be constantly flashing
  void state_flashing()
  {  
      if(this->last_state != Flashing)
      {
        this->flash_state_last = 0;
        this->flash_time_last = 0;
        this->flash_index = 0;
      }

      if(flash_index > FLASH_COUNT)
      {
        this->state = Idle;
        return;
      }

      if( (millis() - this->flash_time_last) > FLASH_DELAY)
      {
        this->flash_state_last = !this->flash_state_last;
        this->flash_time_last = millis();
        this->flash_index++;
      }

      if(this->flash_state_last)
      {
        for(unsigned int index = 0; index < this->led_strip->num_pixels; index++)
        {
          this->led_strip->increment_led_colour(index, this->col[0], this->col[1], this->col[2]);
        }
      }

      this->last_state = Flashing;
      return;
  }

  void state_active()
  {
    unsigned long current_millis = millis();
    if ((current_millis - this->start_time_millis) > this->timer_time_millis) 
    { 
      this->state = Flashing;
      flash_time_last = millis();
      return;
    }

    float progress = float(current_millis - this->start_time_millis) / this->timer_time_millis;

    unsigned int progress_led = progress * this->led_strip->num_pixels;

    for(unsigned int index = 0; index < progress_led; index++)
    {
      this->led_strip->increment_led_colour(index, this->col[0], this->col[1], this->col[2]);
    }

    this->last_state = Active;
  }

  void state_paused()
  {

    float progress = float(this->elapsed_time) / this->timer_time_millis;
    unsigned int progress_led = progress * this->led_strip->num_pixels;

    for(unsigned int index = 0; index < progress_led; index++)
    {
      this->led_strip->increment_led_colour(index, this->col[0], this->col[1], this->col[2]);
    }
    this->last_state = Paused;
  }

  void update()
  {
    switch(this->state)
    {
      case Idle:
        state_idle();
        break;
      case Active:
        state_active();
        break;
      case Flashing:
        state_flashing();
        break;
      case Paused:
        state_paused();
        break;
    }
  }

  bool is_active()
  {
    if (
      (this->state == Active) || 
      (this->state == Flashing)
    ) return true;

    return false;
  }

  bool is_finished()
  {
    if (
      (this->state == Idle) && 
      (this->state != this->last_state)
    )
    {
      return true;
    }

    return false;
  }
};

#endif

