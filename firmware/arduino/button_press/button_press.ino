#include "button.h"


#define DEC_PIN  8
#define P_S_PIN  9
#define INC_PIN 10

button_t dec_button;
button_t p_s_button;
button_t inc_button;

void setup() {

  Serial.begin(115200);

  // put your setup code here, to run once:
  pinMode(DEC_PIN, INPUT);
  pinMode(P_S_PIN, INPUT);
  pinMode(INC_PIN, INPUT);

  dec_button.timeFn = &millis;
  dec_button.config |= CONF_PULLUP;

  p_s_button.timeFn = &millis;
  p_s_button.config |= CONF_PULLUP;
  
  inc_button.timeFn = &millis;
  inc_button.config |= CONF_PULLUP;
}



void loop() {
  // put your main code here, to run repeatedly:
  update_button(&dec_button, digitalRead(DEC_PIN));


}
