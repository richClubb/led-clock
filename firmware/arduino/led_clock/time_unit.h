#ifndef __TIME_UNIT_H__

#define __TIME_UNIT_H__

#include <DS3231.h>

#include "led_strip.h"

enum TimeUnitType
{
  Month = 0,
  Day,
  Hour,
  Minute,
  Second
};

class TimeUnit
{

private:
    LedStrip *led_strip = NULL;
    LedStrip *led_strip_alt = NULL;
    
    DS3231 *rtc = NULL;
    
    unsigned int red_colour;
    unsigned int green_colour;
    unsigned int blue_colour;
    
    unsigned int time;
    unsigned int last_time;
    unsigned long millis_offset;
    TimeUnitType unit_type;
    float deg_per_unit;

    bool century;
    bool h12Flag;
    bool pmFlag;
    
public:
  TimeUnit(
    LedStrip *led_strip,
    LedStrip *led_strip_alt,
    DS3231 *rtc, 
    TimeUnitType unit_type, 
    unsigned int red_colour,
    unsigned int green_colour,
    unsigned int blue_colour
  )
  {
    this->led_strip = led_strip;
    this->led_strip_alt = led_strip_alt;
    this->rtc = rtc;
    this->unit_type = unit_type;

    this->red_colour = red_colour;
    this->green_colour = green_colour;
    this->blue_colour = blue_colour;

    switch(this->unit_type)
    {
      case Month:
        Serial.println("Not implemented deg_per for month");
        break;
      case Day:
        Serial.println("Not implemented deg_per for Day");
        break;
      case Hour:
        this->deg_per_unit = 360.0 / (24 * 60);
        break;
      case Minute:
        this->deg_per_unit = 360.0 / (60 * 60);
        break;
      case Second:
        this->deg_per_unit = 360.0 / (60 * 1000);
        break;
    }
  }

  void update()
  {
    unsigned long time_int;
    this->last_time = this->time;
    byte rtc_day, rtc_hour, rtc_minute, rtc_second;
    switch(this->unit_type)
    {
      case Month:
        this->time = rtc->getMonth(this->century);
        time_int = this->time;
        break;
      case Day:
        this->time = rtc->getDate();
        time_int = this->time;
        break;
      case Hour:
        this->time = rtc->getHour(this->h12Flag, this->pmFlag) * 60 + rtc->getMinute();
        time_int = this->time;
        break;
      case Minute:
        this->time = rtc->getMinute() * 60 + rtc->getSecond();
        time_int = this->time;
        break;
      case Second:
        this->time = rtc->getSecond();
        if (this->last_time != this->time)
        {
          this->millis_offset = millis();
        }
        time_int = ( time * 1000 ) + ( ( millis() - this->millis_offset ) % 1000ul );
        break;
    }

    unsigned int num_pixels = led_strip->num_pixels;
    unsigned int deg_per_pixel = 360 / num_pixels;

    unsigned int pri_led = 0;
    unsigned int sec_led = 0;
    float pri_mod = 0;
    float sec_mod = 0;

    unsigned int time_degree = time_int * this->deg_per_unit;

    pri_led = (time_degree / deg_per_pixel);
    sec_led = (pri_led + 1) % num_pixels;

    pri_mod = float(deg_per_pixel - (int(time_degree) % deg_per_pixel)) / deg_per_pixel;
    sec_mod = float(int(time_degree) % deg_per_pixel) / deg_per_pixel;

    this->led_strip->increment_led_colour(
      pri_led, 
      this->red_colour * pri_mod, 
      this->green_colour * pri_mod, 
      this->blue_colour * pri_mod
    );

    this->led_strip->increment_led_colour(
      sec_led, 
      this->red_colour * sec_mod, 
      this->green_colour * sec_mod, 
      this->blue_colour * sec_mod
    );
  }

  void set_colours(
    unsigned int red,
    unsigned int green,
    unsigned int blue
  )
  {
    this->red_colour = red;
    this->green_colour = green;
    this->blue_colour = blue;
  }

  void set_led_strip(LedStrip *led_strip)
  {
    this->led_strip = led_strip;
  }

  void set_led_strip_alt(LedStrip *led_strip)
  {
    this->led_strip_alt = led_strip;
  }

  void switch_alt_led_strip()
  {
    LedStrip *temp = led_strip;
    this->led_strip = this->led_strip_alt;
    this->led_strip_alt = temp;
  }

  void restore_led_strip()
  {
    LedStrip *temp = led_strip;
    this->led_strip = this->led_strip_alt;
    this->led_strip_alt = temp;
  }
};

#endif