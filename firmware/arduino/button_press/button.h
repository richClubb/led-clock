#ifndef __BUTTON_H__

#define __BUTTON_H__

#define CONF_DEBOUNCE     0x01
#define CONF_LONG_PRESS   0x02
#define CONF_DOUBLE_PRESS 0x04
#define CONF_PULLUP       0x08
#define CONF_PULLDOWN     0x10
#define CONF_UNDEF_1      0x20
#define CONF_UNDEF_2      0x40
#define CONF_UNDEF_3      0x80

#define NO_PRESS            0u
#define SHORT_PRESS         1u
#define SHORT_PRESS_RELEASE 2u
#define LONG_PRESS          4u
#define DOUBLE_PRESS        8u


typedef struct button{
  unsigned long (*timeFn)(void);
  unsigned char config;
  unsigned long debounce_time_ms;
  unsigned long long_press_time;
  bool state_last;
  bool state_current;
  unsigned long last_state_transition_time;
} button_t;

unsigned int update_button(button_t *button, bool button_state)
{
  if (button->config & CONF_PULLDOWN)
  {
    button->state_current = !button_state;
  }
  else
  {
    button->state_current = button_state;
  }

  if(
    button->state_current && 
    (button->state_current != button->state_last)
  )
  {
    Serial.print("Press_time");
    Serial.println(button->timeFn());
    button->state_last = button->state_current;
    return SHORT_PRESS;
  }

  if(
    !button->state_current &&
    (button->state_current != button->state_last)
  )
  {
    Serial.print("Press_time");
    Serial.println(button->timeFn());
    button->state_last = button->state_current;
    return SHORT_PRESS_RELEASE;
  }

  button->state_last = button->state_current;
  return NO_PRESS;
}

#endif