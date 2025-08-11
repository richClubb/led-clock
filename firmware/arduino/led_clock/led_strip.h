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
    for (unsigned int index = 0; index < this->num_pixels; index++)
    {
      this->led_cols[index][RED_COL_INDEX] = 0;
      this->led_cols[index][GREEN_COL_INDEX] = 0;
      this->led_cols[index][BLUE_COL_INDEX] = 0;
    }
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

  unsigned int num_pixels;
  unsigned int led_cols[MAX_NUM_PIXELS][3];
};

#endif