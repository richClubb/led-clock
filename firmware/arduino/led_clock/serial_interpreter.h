#ifndef __SERIAL_INTERPRETER_H__

#define __SERIAL_INTERPRETER_H__

#include "stdlib.h"
#include "string.h"

#define SERIAL_MESSAGE_SIZE 50

#define COMMAND_SIZE_MIN 3

#define CMD_INVALID 0

unsigned int serial_msg_index = 0;
char serial_msg[SERIAL_MESSAGE_SIZE];

extern void (*setTimePtr)(byte, byte, byte, byte, byte, byte);

/*
  Time command format should be:
  [Year]-[Month]-[Day]T[Hour]:[Minute]:[Second]

  Where: 
  [Year] - 4 digits numerical [ 0000 - 9999 ]
  [Month] - 2 digits numerical [ 1 - 12 ]
  [Day] - 2 digits numerical [ 1 - 31 ]
  [Hour] - 2 digits numerical [ 00 - 24 ]
  [Minute] - 2 digits numerical [ 00 - 59 ]
  [Second] - 2 digits numerical [ 00 - 59 ]
*/
#define TIME_COMMAND_MAX_SIZE 19
#define TIME_COMMAND_MIN_SIZE 19

#define TIME_COMMAND_YEAR_START_POS 0
#define TIME_COMMAND_MONTH_START_POS 5
#define TIME_COMMAND_DAY_START_POS 8
#define TIME_COMMAND_HOUR_START_POS 11
#define TIME_COMMAND_MINUTE_START_POS 14
#define TIME_COMMAND_SECOND_START_POS 17

#define TIME_COMMAND_MONTH_DASH_POS 4
#define TIME_COMMAND_DAY_DASH_POS 7
#define TIME_COMMAND_TIME_T_POS 10
#define TIME_COMMAND_HOUR_COLON_POS 13
#define TIME_COMMAND_MINUTE_COLON_POS 16

#define TIME_CMD 1U
#define TIME_CMD_DATA_OFFSET 2

// 1 character longer to account for the \0
#define YEAR_STR_LEN 5
#define MONTH_STR_LEN 3
#define DAY_STR_LEN 3
#define HOUR_STR_LEN 3
#define MINUTE_STR_LEN 3
#define SECOND_STR_LEN 3

#define YEAR_VAL_MIN 0
#define YEAR_VAL_MAX 9999
#define MONTH_VAL_MIN 1
#define MONTH_VAL_MAX 12
#define DAY_VAL_MIN 1
#define DAY_VAL_MAX 31
#define HOUR_VAL_MIN 0
#define HOUR_VAL_MAX 23
#define MINUTE_VAL_MIN 0
#define MINUTE_VAL_MAX 59
#define SECOND_VAL_MIN 0
#define SECOND_VAL_MAX 59

#define JANUARY 1
#define FEBRUARY 2
#define MARCH 3
#define APRIL 4
#define MAY 5
#define JUNE 6
#define JULY 7
#define AUGUST 8
#define SEPTEMBER 9
#define OCTOBER 10
#define NOVEMBER 11
#define DECEMBER 12

bool validate_month_day(unsigned int month, unsigned int day)
{
  if (month < MONTH_VAL_MIN || month > MONTH_VAL_MAX) return false;
  if (day < DAY_VAL_MIN || day > DAY_VAL_MAX) return false;

  // I should hash define this but I CBA...
  switch(month)
  {
    case JANUARY:
      if (day < 1 || day > 31) return false;
      break;
    case FEBRUARY:
      // should I bother to do the leap year calculation
      if (day < 1 || day > 29) return false;
      break;
    case MARCH:
      if (day < 1 || day > 31) return false;
      break;
    case APRIL:
      if (day < 1 || day > 30) return false;
      break;
    case MAY:
      if (day < 1 || day > 31) return false;
      break;
    case JUNE:
      if (day < 1 || day > 30) return false;
      break;
    case JULY:
      if (day < 1 || day > 31) return false;
      break;
    case AUGUST:
      if (day < 1 || day > 31) return false;
      break;
    case SEPTEMBER:
      if (day < 1 || day > 30) return false;
      break;
    case OCTOBER:
      if (day < 1 || day > 31) return false;
      break;
    case NOVEMBER:
      if (day < 1 || day > 30) return false;
      break;
    case DECEMBER:
      if (day < 1 || day > 31) return false;
      break;
  }

  return true;
}

bool validate_time_command(
  char *ptr, 
  unsigned int size,
  byte *year,
  byte *month,
  byte *day,
  byte *hour,
  byte *minute,
  byte *second
)
{
  char year_str[YEAR_STR_LEN];
  char month_str[MONTH_STR_LEN];
  char day_str[DAY_STR_LEN];
  char hour_str[HOUR_STR_LEN];
  char minute_str[MINUTE_STR_LEN];
  char second_str[SECOND_STR_LEN];
  char *endptr;

  long year_l, month_l, day_l, hour_l, minute_l, second_l;

  // 2025-01-01T10:10:10
  if (
    (size < TIME_COMMAND_MIN_SIZE) || 
    (size > TIME_COMMAND_MAX_SIZE)
  ) return false;

  // 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18
  // 2 0 2 5 - 0 1 - 0 1  T  1  0  :  1  0  :  1  0
  if (ptr[TIME_COMMAND_MONTH_DASH_POS] != '-') return false;

  if (ptr[TIME_COMMAND_DAY_DASH_POS] != '-') return false;

  if (ptr[TIME_COMMAND_TIME_T_POS] != 'T') return false;

  if (ptr[TIME_COMMAND_HOUR_COLON_POS] != ':') return false;

  if (ptr[TIME_COMMAND_MINUTE_COLON_POS] != ':') return false;

  memset(year_str, '\0', YEAR_STR_LEN);
  memset(month_str, '\0', MONTH_STR_LEN);
  memset(day_str, '\0', DAY_STR_LEN);
  memset(hour_str, '\0', HOUR_STR_LEN);
  memset(minute_str, '\0', MINUTE_STR_LEN);
  memset(second_str, '\0', SECOND_STR_LEN);
  
  // subtracting 1 to only copy the actual digit
  memcpy(  year_str,                                     ptr,   YEAR_STR_LEN - 1);
  memcpy( month_str,   ptr + TIME_COMMAND_MONTH_DASH_POS + 1,  MONTH_STR_LEN - 1);
  memcpy(   day_str,     ptr + TIME_COMMAND_DAY_DASH_POS + 1,    DAY_STR_LEN - 1);
  memcpy(  hour_str,       ptr + TIME_COMMAND_TIME_T_POS + 1,   HOUR_STR_LEN - 1);
  memcpy(minute_str,   ptr + TIME_COMMAND_HOUR_COLON_POS + 1, MINUTE_STR_LEN - 1);
  memcpy(second_str, ptr + TIME_COMMAND_MINUTE_COLON_POS + 1, SECOND_STR_LEN - 1);

  year_l   = strtol(  year_str, &endptr, 10);
  month_l  = strtol( month_str, &endptr, 10);
  day_l    = strtol(   day_str, &endptr, 10);
  hour_l   = strtol(  hour_str, &endptr, 10);
  minute_l = strtol(minute_str, &endptr, 10);
  second_l = strtol(second_str, &endptr, 10);

  if ( (  year_l < YEAR_VAL_MIN)   || (year_l   > YEAR_VAL_MAX)   ) return false;
  if ( validate_month_day(month_l, day_l)) return false;
  if ( (  hour_l < HOUR_VAL_MIN)   || (hour_l   > HOUR_VAL_MAX)   ) return false;
  if ( (minute_l < MINUTE_VAL_MIN) || (minute_l > MINUTE_VAL_MAX) ) return false;
  if ( (second_l < SECOND_VAL_MIN) || (second_l > SECOND_VAL_MAX) ) return false;

  *year   = (byte)year_l % 100;
  *month  = (byte)month_l;
  *day    = (byte)day_l;
  *hour   = (byte)hour_l;
  *minute = (byte)minute_l;
  *second = (byte)second_l;

  return true;
}

bool process_time_command(char *ptr, unsigned int size)
{
  byte year;
  byte month;
  byte day;
  byte hour;
  byte minute;
  byte second;
  bool result = true;

  result = validate_time_command(
    ptr, 
    size,
    &year,
    &month,
    &day,
    &hour,
    &minute,
    &second);

  if (result == false)
  {
    return false;
  } 

  if (setTimePtr == NULL)
  {
    return false;
  }

  setTimePtr(year, month, day, hour, minute, second);

  return true;
}


extern void (*setTimeUnitPtr)(byte, byte, byte, byte, byte);
/*
  Hour command format should be:
  [ring 1-3],[red 000-255],[green 000-255],[blue 000-255]
*/
#define HOUR_CMD 2U
#define HOUR_CMD_DATA_OFFSET 2

#define MINUTE_CMD 3U
#define MINUTE_CMD_DATA_OFFSET 2

#define SECOND_CMD 4U
#define SECOND_CMD_DATA_OFFSET 2

#define TIME_UNIT_COMMAND_LEN 13
#define RING_STR_LEN 2
#define COL_STR_LEN 4
bool validate_time_unit_command(char *ptr, unsigned int size, byte *led_ring, byte *red, byte *green, byte *blue)
{
  Serial.println("Validating time unit command");

  char ring_str[RING_STR_LEN];
  char red_str[COL_STR_LEN];
  char green_str[COL_STR_LEN];
  char blue_str[COL_STR_LEN];
  char *endptr;

  unsigned long ring_l, red_l, green_l, blue_l;

  // 0 , 0 0 0 , 0 0 0 ,  0  0  0
  // 0 1 2 3 4 5 6 7 8 9 10 11 12

  if (strlen(ptr) != TIME_UNIT_COMMAND_LEN){
    Serial.println("Invalid length");
    return false;
  } 

  if (ptr[1] != ',') return false;

  if (ptr[5] != ',') return false;

  if (ptr[9] != ',') return false;

  memset(ring_str, '\0', RING_STR_LEN);
  memset(red_str, '\0', COL_STR_LEN);
  memset(green_str, '\0', COL_STR_LEN);
  memset(blue_str, '\0', COL_STR_LEN);

  memcpy(ring_str, ptr, 1);
  memcpy(red_str, ptr+2, 3);
  memcpy(green_str, ptr+6, 3);
  memcpy(blue_str, ptr+10, 3);

  ring_l  = strtol(ring_str, &endptr, 10);
  red_l   = strtol(red_str, &endptr, 10);
  green_l = strtol(green_str, &endptr, 10);
  blue_l  = strtol(blue_str, &endptr, 10);

  if ( (ring_l < 1) || ring_l > 4)    return false;
  if (  (red_l < 0) || red_l > 255)   return false;
  if ((green_l < 0) || green_l > 255) return false;
  if ( (blue_l < 0) || blue_l > 255)  return false;

  *led_ring = (byte)ring_l;
  *red      = (byte)red_l;
  *green    = (byte)green_l;
  *blue     = (byte)blue_l;

  return true;
}

bool process_time_unit_command(char *ptr, unsigned int size, byte time_unit)
{
  bool result = true;

  byte led_ring, red, green, blue;

  result = validate_time_unit_command(ptr, size, &led_ring, &red, &green, &blue);

  if (!result)
  {
    return false;
  }

  setTimeUnitPtr(time_unit, led_ring, red, green, blue);

  return result;
}

extern void (*setTimerPtr)(unsigned int);
/*
  Timer command format should be:
  [hours]:[minutes]:[seconds]
*/
#define TIMER_CMD 5U
#define TIMER_CMD_DATA_OFFSET 2
bool validate_timer_command(char *ptr, unsigned int size, unsigned int *time)
{
  char unit_strs[3][3];
  char *endptr;

  memset(unit_strs, '\0', 3 * 3);

  unsigned int current_state = 0;
  unsigned int current_index = 0;

  if ( strlen(ptr) != size ) return false;

  for (unsigned int index = 0; index < size; index++)
  {
    char current_char = ptr[index];

    if(current_index > 2) return false;

    if (current_char == ':')
    {
      current_state++;
      current_index = 0;
      continue;
    }
    unit_strs[current_state][current_index] = current_char;
    current_index++;
  }

  switch(current_state)
  {
    case 0:
      *time = strtol(unit_strs[0], &endptr, 10);
      break;
    case 1:
      *time = strtol(unit_strs[0], &endptr, 10) * 60 + strtol(unit_strs[1], &endptr, 10);
      break;
    case 2:
      *time = strtol(unit_strs[0], &endptr, 10) * 60 * 60 + strtol(unit_strs[1], &endptr, 10) * 60 + strtol(unit_strs[2], &endptr, 10);
      break;
  }
  return true;
}

bool process_timer_command(char *ptr, unsigned int size)
{
  bool result = true;
  unsigned int time;

  result = validate_timer_command(ptr, size, &time);

  if (!result)
  {
    return false;
  }

  if (setTimerPtr == NULL)
  {
    return false;
  }

  Serial.println(time);
  setTimerPtr(time);

  return true;
}

extern void (*setTimerConfigPtr)(byte, byte, byte, byte);
bool process_timer_config_command(char *ptr, unsigned int size)
{
  bool result = true;

  byte led_ring, red, green, blue;

  result = validate_time_unit_command(ptr, size, &led_ring, &red, &green, &blue);

  if (!result)
  {
    return false;
  }

  setTimerConfigPtr(led_ring, red, green, blue);
  return true;
}

bool validate_timer_control_command(char *ptr, unsigned int size, unsigned int *mode)
{

  unsigned int ptr_len = strlen(ptr);

  if (ptr_len != size) return false;

// This doesn't seem to work for some reason
//  if ((ptr_len == 5) && strncmp(ptr, "pause", 5))
  if(*ptr == '1')
  {
    *mode = 1;
    return true;
  }

//  if ((ptr_len == 5) && strncmp(ptr, "reset", 5))
  if(*ptr == '2')
  {
    *mode = 2;
    return true;
  }

//  if ((ptr_len == 5) && strncmp(ptr, "reset", 5))
  if(*ptr == '3')
  {
    *mode = 3;
    return true;
  }

  return false;
}


extern void (*setTimerControlPtr)(unsigned int);
bool process_timer_control_command(char *ptr, unsigned int size)
{
  unsigned int mode;

  if (!validate_timer_control_command(ptr, size, &mode))
  {
    return false;
  }

  if (setTimerControlPtr == NULL)
  {
    return false;
  }

  setTimerControlPtr(mode);
  return true;
}

/*
  Command format should be: [command_type]=[command];

  Where command type is a single ASCII char A-Z, a-z, 0-9
*/
bool process_command(char *ptr, unsigned int size, unsigned int *command_type_out)
{
  Serial.println("Processing command");
  // probably too defensive but meh
  if (size > SERIAL_MESSAGE_SIZE) return false;
  
  if (size < COMMAND_SIZE_MIN) return false;

  if (ptr[1] != '=') return false;

  char command_type = ptr[0];

  bool result = false;
  switch(command_type)
  {
    case 'T':
      Serial.println("Found Set Time command");
      result = process_time_command(ptr + TIME_CMD_DATA_OFFSET, size - TIME_CMD_DATA_OFFSET);
      break;
    case 'H':
      Serial.println("Found Hour command");
      result = process_time_unit_command(ptr + HOUR_CMD_DATA_OFFSET, size - HOUR_CMD_DATA_OFFSET, 1);
      break;
    case 'M':
      Serial.println("Found Minute command");
      result = process_time_unit_command(ptr + MINUTE_CMD_DATA_OFFSET, size - MINUTE_CMD_DATA_OFFSET, 2);
      break;
    case 'S':
      Serial.println("Found Second command");
      result = process_time_unit_command(ptr + SECOND_CMD_DATA_OFFSET, size - SECOND_CMD_DATA_OFFSET, 3);
      break;
    case 'I':
      Serial.println("Found Timer command");
      result = process_timer_command(ptr + TIMER_CMD_DATA_OFFSET, size - TIMER_CMD_DATA_OFFSET);
      break;
    case 'E':
      Serial.println("Found Timer Config command");
      result = process_timer_config_command(ptr + TIMER_CMD_DATA_OFFSET, size - TIMER_CMD_DATA_OFFSET);
      break;
    case 'R':
      Serial.println("Found Timer Control command");
      result = process_timer_control_command(ptr + TIMER_CMD_DATA_OFFSET, size - TIMER_CMD_DATA_OFFSET);
      break;
    default:
      return false;
  }

  if (result == false)
  {
    return false;
  }  

  return result;
}

void process_serial_data() {
  unsigned int command_type;

  // put your main code here, to run repeatedly:
  while (Serial.available() > 0)
  {
    // buffer overflow check
    if (serial_msg_index + 1 > SERIAL_MESSAGE_SIZE)
    {
      Serial.println("current command overflowed buffer, flushing buffer\n");
      serial_msg_index = 0;
      memset(serial_msg, '\0', SERIAL_MESSAGE_SIZE);
      /* should I try and intelligently flag that the current command is dirty? */
    }

    char current_char = Serial.read();
    if (current_char == '\n') continue;

    if (current_char == ';')
    {
      Serial.println("Found complete command");
      if (!process_command(serial_msg, serial_msg_index, &command_type))
      {
        Serial.println("Command invalid");
        memset(serial_msg, '\0', SERIAL_MESSAGE_SIZE);
        serial_msg_index = 0;
      }

      serial_msg_index = 0;
      memset(serial_msg, '\0', SERIAL_MESSAGE_SIZE);
      continue;
    }

    serial_msg[serial_msg_index++] = current_char;
  }
}

#endif