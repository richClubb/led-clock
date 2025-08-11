// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// Released under the GPLv3 license to match the rest of the
// Adafruit NeoPixel library

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// #define PIN        3 // Inner ring 
#define PIN        5 // Outer ring

// How many NeoPixels are attached to the Arduino?
// #define NUMPIXELS 12 // Inner ring
#define NUMPIXELS 24 // Outer ring

// Speeds up time for testing
#define TIME_MOD 60

#define DEG_PER_PIXEL (360 / NUMPIXELS)

#define RED_IDX 0
#define GREEN_IDX 1
#define BLUE_IDX 2

#define SERIAL_DEBUG
#define SERIAL_BAUD 115200

#ifdef SERIAL_DEBUG
  #define DELAY 100
#else
  #define DELAY 10
#endif

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int led_cols[NUMPIXELS][3];

void setup() {

  #ifdef SERIAL_DEBUG
    Serial.begin(SERIAL_BAUD);
  #endif

  for (unsigned int index = 0; index < NUMPIXELS; index++)
  {
    led_cols[index][RED_IDX] = 0;
    led_cols[index][GREEN_IDX] = 0;
    led_cols[index][BLUE_IDX] = 0;
  }

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
}

unsigned int sec_col[3] = {0, 150, 0};
unsigned int min_col[3] = {150, 0, 0};
unsigned int hou_col[3] = {0, 0, 150};

void loop() {
  pixels.clear(); // Set all pixel colors to 'off'

  for (unsigned int index = 0; index < NUMPIXELS; index++)
  {
    led_cols[index][RED_IDX] = 0;
    led_cols[index][GREEN_IDX] = 0;
    led_cols[index][BLUE_IDX] = 0;
  }

  int millis_time = millis() * TIME_MOD;

  int milli_second = (millis_time % 60000);
  int minute = (millis_time) % 3600000;
  int hour = millis_time % (3600000 * NUMPIXELS);

  // Seconds
  float sec_degree = milli_second * 0.006;

  float sec_pri_mod = float(DEG_PER_PIXEL - (int(sec_degree) % DEG_PER_PIXEL)) / DEG_PER_PIXEL;
  float sec_sec_mod = float(int(sec_degree) % DEG_PER_PIXEL) / DEG_PER_PIXEL;
  unsigned int sec_pri_led = sec_degree / DEG_PER_PIXEL ;
  unsigned int sec_sec_led = (sec_pri_led + 1) % NUMPIXELS;

  led_cols[sec_pri_led][RED_IDX] = sec_col[RED_IDX] * sec_pri_mod;
  led_cols[sec_pri_led][GREEN_IDX] = sec_col[GREEN_IDX] * sec_pri_mod;
  led_cols[sec_pri_led][BLUE_IDX] = sec_col[BLUE_IDX] * sec_pri_mod;

  led_cols[sec_sec_led][RED_IDX] = sec_col[RED_IDX] * sec_sec_mod;
  led_cols[sec_sec_led][GREEN_IDX] = sec_col[GREEN_IDX] * sec_sec_mod;
  led_cols[sec_sec_led][BLUE_IDX] = sec_col[BLUE_IDX] * sec_sec_mod;

  // Minutes
  float min_degree = minute * 0.0001;

  float min_pri_mod = float(DEG_PER_PIXEL - (int(min_degree) % DEG_PER_PIXEL)) / DEG_PER_PIXEL;
  float min_sec_mod = float(int(min_degree) % DEG_PER_PIXEL) / DEG_PER_PIXEL;
  unsigned int min_pri_led = min_degree / DEG_PER_PIXEL;
  unsigned int min_sec_led = (min_pri_led + 1) % NUMPIXELS;

  led_cols[min_pri_led][RED_IDX] = min(int(led_cols[min_pri_led][RED_IDX] + min_col[RED_IDX] * min_pri_mod), 255);
  led_cols[min_pri_led][GREEN_IDX] = min(int(led_cols[min_pri_led][GREEN_IDX] + min_col[GREEN_IDX] * min_pri_mod), 255);
  led_cols[min_pri_led][BLUE_IDX] = min(int(led_cols[min_pri_led][BLUE_IDX] + min_col[BLUE_IDX] * min_pri_mod), 255);

  led_cols[min_sec_led][RED_IDX] = min(int(led_cols[min_sec_led][RED_IDX] + min_col[RED_IDX] * min_sec_mod), 255);
  led_cols[min_sec_led][GREEN_IDX] = min(int(led_cols[min_sec_led][GREEN_IDX] + min_col[GREEN_IDX] * min_sec_mod), 255);
  led_cols[min_sec_led][BLUE_IDX] = min(int(led_cols[min_sec_led][BLUE_IDX] + min_col[BLUE_IDX] * min_sec_mod), 255);

  // Hours
  float hour_degree = hour * 0.000004167;

  float hou_pri_mod = float(DEG_PER_PIXEL - (int(hour_degree) % DEG_PER_PIXEL)) / DEG_PER_PIXEL;
  float hou_sec_mod = float(int(hour_degree) % DEG_PER_PIXEL) / DEG_PER_PIXEL;
  unsigned int hou_pri_led = hour_degree / DEG_PER_PIXEL;
  unsigned int hou_sec_led = (hou_pri_led + 1) % NUMPIXELS;

  led_cols[hou_pri_led][RED_IDX] = min(int(led_cols[hou_pri_led][RED_IDX] + hou_col[RED_IDX] * hou_pri_mod), 255);
  led_cols[hou_pri_led][GREEN_IDX] = min(int(led_cols[hou_pri_led][GREEN_IDX] + hou_col[GREEN_IDX] * hou_pri_mod), 255);
  led_cols[hou_pri_led][BLUE_IDX] = min(int(led_cols[hou_pri_led][BLUE_IDX] + hou_col[BLUE_IDX] * hou_pri_mod), 255);

  led_cols[hou_sec_led][RED_IDX] = min(int(led_cols[hou_sec_led][RED_IDX] + hou_col[RED_IDX] * hou_sec_mod), 255);
  led_cols[hou_sec_led][GREEN_IDX] = min(int(led_cols[hou_sec_led][GREEN_IDX] + hou_col[GREEN_IDX] * hou_sec_mod), 255);
  led_cols[hou_sec_led][BLUE_IDX] = min(int(led_cols[hou_sec_led][BLUE_IDX] + hou_col[BLUE_IDX] * hou_sec_mod), 255);

  #ifdef SERIAL_DEBUG
    Serial.print("Debug - Degree: ");
    Serial.print(sec_degree);
    Serial.print(" pri_led: ");
    Serial.print(sec_pri_led);
    Serial.print(" sec_led: ");
    Serial.print(sec_sec_led);
    Serial.print(" pri_mod: ");
    Serial.print(sec_pri_mod);
    Serial.print(" sec_mod: ");
    Serial.print(sec_sec_mod);
    Serial.print(" minute: ");
    Serial.print(minute);
    Serial.println(" ");
  #endif

  for (unsigned int index = 0; index < NUMPIXELS; index ++)
  {
    pixels.setPixelColor(
      index, 
      pixels.Color(
        led_cols[index][RED_IDX], 
        led_cols[index][GREEN_IDX],
        led_cols[index][BLUE_IDX]
      )
    );
  }

  pixels.show();   // Send the updated pixel colors to the hardware.

  #ifdef SERIAL_DEBUG
    delay(DELAY);
  #endif
}
