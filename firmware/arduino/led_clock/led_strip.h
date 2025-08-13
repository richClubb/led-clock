#ifndef __LED_STRIP_H__

#define __LED_STRIP_H__

#include <Adafruit_NeoPixel.h>

#define MAX_NUM_PIXELS 60

#define RED_COL_INDEX 0
#define GREEN_COL_INDEX 1
#define BLUE_COL_INDEX 2

class LedStrip
{
private:
  Adafruit_NeoPixel *pixels = NULL;
  unsigned int led_cols[MAX_NUM_PIXELS][3];

public:
  LedStrip(unsigned int pin, unsigned int num_pixels)
  {
    this->pixels = new Adafruit_NeoPixel(num_pixels, pin, NEO_GRB + NEO_KHZ800);
    
    if (num_pixels > MAX_NUM_PIXELS)
    {
      this->num_pixels = MAX_NUM_PIXELS;
    }
    else
    {
      this->num_pixels = num_pixels;
    }
    this->pixels->begin();
    this->pixels->clear();
  }

  void clear()
  {
    memset( this->led_cols, 0, MAX_NUM_PIXELS * 3 * sizeof(unsigned int) );
  }

  void update()
  {
    this->pixels->clear();
    for (unsigned int index = 0; index < this->num_pixels; index++)
    {
      this->pixels->setPixelColor(
        index,
        min(255u, this->led_cols[index][RED_COL_INDEX]), 
        min(255u, this->led_cols[index][GREEN_COL_INDEX]), 
        min(255u, this->led_cols[index][BLUE_COL_INDEX])
      );
    }
    this->pixels->show();   // Send the updated pixel colors to the hardware.
  }

  void set_led_colour(unsigned int led_index, unsigned int red, unsigned int green, unsigned int blue)
  {
    this->led_strip->led_cols[led_index][RED_COL_INDEX]   = min(255u, red);
    this->led_strip->led_cols[led_index][GREEN_COL_INDEX] = min(255u, green);
    this->led_strip->led_cols[led_index][BLUE_COL_INDEX]  = min(255u, blue);
  }

  void increment_led_colour(unsigned int led_index, unsigned int red, unsigned int green, unsigned int blue)
  {
    this->led_strip->led_cols[led_index][RED_COL_INDEX]   = min(255u, this->led_strip->led_cols[led_index][RED_COL_INDEX]   + red);
    this->led_strip->led_cols[led_index][GREEN_COL_INDEX] = min(255u, this->led_strip->led_cols[led_index][GREEN_COL_INDEX] + green);
    this->led_strip->led_cols[led_index][BLUE_COL_INDEX]  = min(255u, this->led_strip->led_cols[led_index][BLUE_COL_INDEX]  + blue);
  }

  unsigned int num_pixels;
};

#endif