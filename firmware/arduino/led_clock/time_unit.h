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
    LedStrip *led_strip_orig = NULL;
    
    DS3231 *rtc = NULL;
    
    unsigned int red_colour;
    unsigned int green_colour;
    unsigned int blue_colour;
    
    unsigned int time;
    TimeUnitType unit_type;
    float deg_per_unit;

    bool century;
    bool h12Flag;
    bool pmFlag;
    
public:
  TimeUnit(
    LedStrip *led_strip, 
    DS3231 *rtc, 
    TimeUnitType unit_type, 
    unsigned int red_colour,
    unsigned int green_colour,
    unsigned int blue_colour
  )
  {
    this->led_strip = led_strip;
    this->led_strip_orig = led_strip;
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

  void update(unsigned long millis_offset)
  {
    byte rtc_day, rtc_hour, rtc_minute, rtc_second;
    switch(this->unit_type)
    {
      case Month:
        time = rtc->getMonth(this->century);
        break;
      case Day:
        time = rtc->getDate();
        break;
      case Hour:
        time = rtc->getHour(this->h12Flag, this->pmFlag) * 60 + rtc->getMinute();
        break;
      case Minute:
        time = rtc->getMinute() * 60 + rtc->getSecond();
        break;
      case Second:
        time = rtc->getSecond();
        time = ( time * 1000 ) + ( ( millis()-millis_offset ) % 1000ul );
        break;
    }

    unsigned int num_pixels = led_strip->num_pixels;
    unsigned int deg_per_pixel = 360 / num_pixels;

    unsigned int pri_led = 0;
    unsigned int sec_led = 0;
    float pri_mod = 0;
    float sec_mod = 0;

    unsigned int time_degree = time * this->deg_per_unit;

    pri_led = (time_degree / deg_per_pixel);
    sec_led = (pri_led + 1) % num_pixels;

    pri_mod = float(deg_per_pixel - (int(time_degree) % deg_per_pixel)) / deg_per_pixel;
    sec_mod = float(int(time_degree) % deg_per_pixel) / deg_per_pixel;

    this->led_strip->led_cols[pri_led][RED_COL_INDEX] += this->red_colour * pri_mod;
    this->led_strip->led_cols[pri_led][GREEN_COL_INDEX] += this->green_colour * pri_mod;
    this->led_strip->led_cols[pri_led][BLUE_COL_INDEX] += this->blue_colour * pri_mod;

    this->led_strip->led_cols[sec_led][RED_COL_INDEX] += this->red_colour * sec_mod;
    this->led_strip->led_cols[sec_led][GREEN_COL_INDEX] += this->green_colour * sec_mod;
    this->led_strip->led_cols[sec_led][BLUE_COL_INDEX] += this->blue_colour * sec_mod;
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
    this->led_strip_orig = led_strip;
    this->led_strip = led_strip;
  }

  void set_led_strip_temp(LedStrip *led_strip)
  {
    this->led_strip = led_strip;
  }

  void restore_led_strip()
  {
    this->led_strip = this->led_strip_orig;
  }
};

void set_time_unit(
  TimeUnit *time_unit, 
  LedStrip *led_strip,
  unsigned int red_col, 
  unsigned int green_col, 
  unsigned int blue_col)
{
  if (led_strip != NULL)
  {
    time_unit->set_led_strip(led_strip);
  }

  time_unit->set_colours(red_col, green_col, blue_col);
}

#endif