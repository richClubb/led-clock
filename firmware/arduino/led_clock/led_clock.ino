#include "led_strip.h"
#include "time_unit.h"
#include "serial_interpreter.h"
#include "timer.h"
#include <DS3231.h>
#include <Wire.h>
#include "Preferences.h"

#define LED_DISPLAY_DELAY 100

#define OUTER_RING_NUM 60
#define OUTER_RING_PIN 3
#define OUTER_RING_INDEX 1
#define MIDDLE_RING_NUM 24
#define MIDDLE_RING_PIN 4
#define MIDDLE_RING_INDEX 2
#define INNER_RING_NUM 12
#define INNER_RING_PIN 5
#define INNER_RING_INDEX 3

LedStrip outer_ring(OUTER_RING_PIN, OUTER_RING_NUM);
LedStrip middle_ring(MIDDLE_RING_PIN, MIDDLE_RING_NUM);
LedStrip inner_ring(INNER_RING_PIN, INNER_RING_NUM);

LedStrip *led_strips[3] = {&outer_ring, &middle_ring, &inner_ring};

DS3231 rtc;
bool century = false;
bool h12Flag;
bool pmFlag;

TimeUnit hour_unit(&inner_ring, &rtc, Hour, 0, 50, 0);
TimeUnit minute_unit(&middle_ring, &rtc, Minute, 50, 0, 0);
TimeUnit second_unit(&middle_ring, &rtc, Second, 0, 0, 50);

ClockTimer clock_timer(&middle_ring, 50, 0, 0);

#define RW_MODE false
#define RO_MODE true

Preferences stcPrefs;

/* ---------------------------------------------------------------------------- */
/* --  CONFIG FUNCTIONS ------------------------------------------------------- */
/* ---------------------------------------------------------------------------- */

void set_time(
  byte year, 
  byte month, 
  byte day, 
  byte hour, 
  byte minute, 
  byte second
)
{
  rtc.setYear(year);
  rtc.setMonth(month);
  rtc.setDate(day);
  rtc.setHour(hour);
  rtc.setMinute(minute);
  rtc.setSecond(second);
}
void (*setTimePtr)(byte, byte, byte, byte, byte, byte) = &set_time;

void set_time_unit(
  byte time_unit,
  byte ring,
  byte red,
  byte green,
  byte blue
)
{
  TimeUnit *time_unit_ptr;

  stcPrefs.begin("STCPrefs", RW_MODE);
  switch(time_unit)
  {
    case 1:
      // Hour
      time_unit_ptr = &hour_unit;
      stcPrefs.putBool("hourSet", true);
      stcPrefs.putUChar("hourRed", red);
      stcPrefs.putUChar("hourGreen", green);
      stcPrefs.putUChar("hourBlue", blue);
      stcPrefs.putUChar("hourRing", ring-1); // this is skanky
      break;
    case 2:
      // Minute
      time_unit_ptr = &minute_unit;
      stcPrefs.putBool("minuteSet", true);
      stcPrefs.putUChar("minuteRed", red);
      stcPrefs.putUChar("minuteGreen", green);
      stcPrefs.putUChar("minuteBlue", blue);
      stcPrefs.putUChar("minuteRing", ring-1);
      break;
    case 3:
      // Second
      time_unit_ptr = &second_unit;
      stcPrefs.putBool("secondSet", true);
      stcPrefs.putUChar("secondRed", red);
      stcPrefs.putUChar("secondGreen", green);
      stcPrefs.putUChar("secondBlue", blue);
      stcPrefs.putUChar("secondRing", ring-1);
      break;
  }
  stcPrefs.end();
  
  switch(ring)
  {
    case OUTER_RING_INDEX:
      time_unit_ptr->set_led_strip(&outer_ring);
      break;
    case MIDDLE_RING_INDEX:
      time_unit_ptr->set_led_strip(&middle_ring);
      break;
    case INNER_RING_INDEX:
      time_unit_ptr->set_led_strip(&inner_ring);
      break;
    default:
      Serial.println("Unknown ring");
      break;
  }
  
  time_unit_ptr->set_colours(red, green, blue);
}
void (*setTimeUnitPtr)(byte, byte, byte, byte, byte) = &set_time_unit;

void set_timer(unsigned int seconds)
{
  clock_timer.set_timer(seconds);
  clock_timer.start_timer();
}
void (*setTimerPtr)(unsigned int) = &set_timer;

void set_timer_config(byte ring, byte red, byte green, byte blue)
{
  clock_timer.set_colours(red, green, blue);
  
  switch(ring)
  {
    case 1:
      clock_timer.set_led_strip(&outer_ring);
      break;
    case 2:
      clock_timer.set_led_strip(&middle_ring);
      break;
    case 3:
      clock_timer.set_led_strip(&inner_ring);
      break;
    default:
      Serial.println("Unknown ring");
      break;
  }
}
void (*setTimerConfigPtr)(byte, byte, byte, byte) = &set_timer_config;

void set_timer_control(unsigned int mode)
{
  switch(mode)
  {
    case 1:
      clock_timer.pause_timer();
      break;
    case 2:
      clock_timer.reset_timer();
      break;
    case 3:
      clock_timer.resume_timer();
      break;
    default:
      break;
  }
}
void(*setTimerControlPtr)(unsigned int) = &set_timer_control;


/* ---------------------------------------------------------------------------- */
/* --  MAIN FUNCTIONS --------------------------------------------------------- */
/* ---------------------------------------------------------------------------- */

void setup()
{
  Serial.begin(115200);
  while(!Serial);

  Wire.begin(7, 6);

  stcPrefs.begin("STCPrefs", RO_MODE);
  bool tpInit = stcPrefs.isKey("nvsInit");

  if (tpInit == true)
  {
    Serial.println("Preferences initialised");

    if (stcPrefs.isKey("hourSet"))
    {
      Serial.print("Hour: ");
      Serial.println(stcPrefs.getUChar("hourRing"));
      hour_unit.set_colours(stcPrefs.getUChar("hourRed"), stcPrefs.getUChar("hourGreen"), stcPrefs.getUChar("hourBlue"));
      hour_unit.set_led_strip(led_strips[stcPrefs.getUChar("hourRing")]);
    }

    if (stcPrefs.isKey("minuteSet"))
    {
      Serial.print("Minute: ");
      Serial.println(stcPrefs.getUChar("minuteRing"));
      minute_unit.set_colours(stcPrefs.getUChar("minuteRed"), stcPrefs.getUChar("minuteGreen"), stcPrefs.getUChar("minuteBlue"));
      minute_unit.set_led_strip(led_strips[stcPrefs.getUChar("minuteRing")]);
    }

    if (stcPrefs.isKey("secondSet"))
    {
      Serial.print("Second: ");
      Serial.println(stcPrefs.getUChar("secondRing"));
      second_unit.set_colours(stcPrefs.getUChar("secondRed"), stcPrefs.getUChar("secondGreen"), stcPrefs.getUChar("secondBlue"));
      second_unit.set_led_strip(led_strips[stcPrefs.getUChar("secondRing")]);
    }

    stcPrefs.end();
  }
  else
  {
    stcPrefs.end();
    stcPrefs.begin("STCPrefs", RW_MODE);
    stcPrefs.putBool("nvsInit", true);
    stcPrefs.end();
  }
}

unsigned int led_index = 0;

unsigned long led_display_last = 0;
void loop()
{
  unsigned long loop_millis = millis();
  // Serial interpreter check
  if (Serial.available())
  {
    process_serial_data();
  }

  // LED Display loop
  if ( (loop_millis - led_display_last) > LED_DISPLAY_DELAY )
  {
    middle_ring.clear();
    inner_ring.clear();

    if(clock_timer.is_active())
    {
      hour_unit.set_led_strip_temp(&inner_ring);
      minute_unit.set_led_strip_temp(&inner_ring);
      second_unit.set_led_strip_temp(&inner_ring);
    }

    if(clock_timer.is_finished())
    {
      hour_unit.restore_led_strip();
      minute_unit.restore_led_strip();
      second_unit.restore_led_strip();
    }

    hour_unit.update();
    minute_unit.update();
    second_unit.update();
    clock_timer.update();

    middle_ring.update();
    inner_ring.update();
    led_display_last = loop_millis;
  }
}