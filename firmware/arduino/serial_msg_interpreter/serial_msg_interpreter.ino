#include "stdlib.h"
#include "string.h"

#define SERIAL_MESSAGE_SIZE 50

#define COMMAND_SIZE_MIN 3

#define CMD_INVALID 0

unsigned int serial_msg_index = 0;
char serial_msg[SERIAL_MESSAGE_SIZE];

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
bool validate_time_command(char *ptr, unsigned int size)
{
  char year_str[5];
  char month_str[3];
  char day_str[3];
  char hour_str[3];
  char minute_str[3];
  char second_str[3];
  char *endptr;

  long year, month, day, hour, minute, second;

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

  memset(year_str, '\0', 5);
  memset(month_str, '\0', 3);
  memset(day_str, '\0', 3);
  memset(hour_str, '\0', 3);
  memset(minute_str, '\0', 3);
  memset(second_str, '\0', 3);
  
  memcpy(year_str, ptr, 4);
  memcpy(month_str, ptr + 5, 2);
  memcpy(day_str, ptr + 8, 2);
  memcpy(hour_str, ptr + 11, 2);
  memcpy(minute_str, ptr + 14, 2);
  memcpy(second_str, ptr + 17, 2);

  year = strtol(year_str, &endptr, 10);
  month = strtol(month_str, &endptr, 10);
  day = strtol(day_str, &endptr, 10);
  hour = strtol(hour_str, &endptr, 10);
  minute = strtol(minute_str, &endptr, 10);
  second = strtol(second_str, &endptr, 10);

  if ( (year < 0) || (year > 9999) ) return false;
  if ( (month < 1) || (month > 12) ) return false;
  if ( (day < 1) || (day > 31) ) return false;
  if ( (hour < 00) || (hour > 59) ) return false;
  if ( (minute < 00) || (minute > 59) ) return false;
  if ( (second < 00) || (second > 59) ) return false;

  return true;
}

/*
  Hour command format should be:
  [red],[green],[blue],[ring]
*/
#define HOUR_CMD 2U
#define HOUR_CMD_DATA_OFFSET 2
bool validate_hour_command(char *ptr, unsigned int size)
{
  Serial.println("Validating hour command");

  return false;
}

/*
  Minute command format should be:
  [red],[green],[blue],[ring]
*/
#define MINUTE_CMD 2U
#define MINUTE_CMD_DATA_OFFSET 2
bool validate_minute_command(char *ptr, unsigned int size)
{
  Serial.println("Validating minute command");

  return false;
}

/*
  Second command format should be:
  [red],[green],[blue],[ring]
*/
#define SECOND_CMD 2U
#define SECOND_CMD_DATA_OFFSET 2
bool validate_second_command(char *ptr, unsigned int size)
{
  Serial.println("Validating second command");

  return false;
}

/*
  Timer command format should be:
  [hours]:[minutes]:[seconds]
*/
#define TIMER_CMD 2U
#define TIMER_CMD_DATA_OFFSET 2
bool validate_timer_command(char *ptr, unsigned int size)
{
  Serial.println("Validating timer command");

  return false;
}

/*
  Command format should be: [command_type]=[command];

  Where command type is a single ASCII char A-Z, a-z, 0-9
*/
bool validate_command(char *ptr, unsigned int size, unsigned int *command_type_out)
{
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
      result = validate_time_command(ptr + TIME_CMD_DATA_OFFSET, size - TIME_CMD_DATA_OFFSET);
      *command_type_out = TIME_CMD;
      break;
    case 'H':
      Serial.println("Found Hour command");
      result = validate_hour_command(ptr + HOUR_CMD_DATA_OFFSET, size - HOUR_CMD_DATA_OFFSET);
      *command_type_out = HOUR_CMD;
      break;
    case 'M':
      Serial.println("Found Minute command");
      result = validate_minute_command(ptr + MINUTE_CMD_DATA_OFFSET, size - MINUTE_CMD_DATA_OFFSET);
      *command_type_out = MINUTE_CMD;
      break;
    case 'S':
      Serial.println("Found Second command");
      result = validate_second_command(ptr + SECOND_CMD_DATA_OFFSET, size - SECOND_CMD_DATA_OFFSET);
      *command_type_out = SECOND_CMD;
      break;
    case 'I':
      Serial.println("Found Timer command");
      result = validate_timer_command(ptr + TIMER_CMD_DATA_OFFSET, size - TIMER_CMD_DATA_OFFSET);
      *command_type_out = TIMER_CMD;
      break;
    default:
      return false;
  }

  if (result == false)
  {
    *command_type_out = CMD_INVALID;
    return false;
  }  

  return result;
}

void set_time(
  char *ptr,
  byte *century,
  byte *year, 
  byte *month,
  byte *day,
  byte *hour,
  byte *minute,
  byte *second)
{
  char year_str[5];
  char month_str[3];
  char day_str[3];
  char hour_str[3];
  char minute_str[3];
  char second_str[3];
  char *endptr;

  long year_int, month_int, day_int, hour_int, minute_int, second_int;

  memset(year_str, '\0', 5);
  memset(month_str, '\0', 3);
  memset(day_str, '\0', 3);
  memset(hour_str, '\0', 3);
  memset(minute_str, '\0', 3);
  memset(second_str, '\0', 3);
  
  memcpy(year_str, ptr, 4);
  memcpy(month_str, ptr + 5, 2);
  memcpy(day_str, ptr + 8, 2);
  memcpy(hour_str, ptr + 11, 2);
  memcpy(minute_str, ptr + 14, 2);
  memcpy(second_str, ptr + 17, 2);

  year_int = strtol(year_str, &endptr, 10);
  month_int = strtol(month_str, &endptr, 10);
  day_int = strtol(day_str, &endptr, 10);
  hour_int = strtol(hour_str, &endptr, 10);
  minute_int = strtol(minute_str, &endptr, 10);
  second_int = strtol(second_str, &endptr, 10);

  *century = (byte)(year_int / 100);
  *year = (byte)(year_int % 100);
  *month = (byte)month_int;
  *day = (byte)day_int;
  *hour = (byte)hour_int;
  *minute = (byte)minute_int;
  *second = (byte)second_int;
}

void setup() {
  Serial.begin(115200);
  memset(serial_msg, '\0', SERIAL_MESSAGE_SIZE);
}

void loop() {
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
      if (!validate_command(serial_msg, serial_msg_index, &command_type))
      {
        Serial.println("Command invalid");
        memset(serial_msg, '\0', SERIAL_MESSAGE_SIZE);
        serial_msg_index = 0;
      }

      switch(command_type)
      {
        case CMD_INVALID:
          break;
        case TIME_CMD:
          Serial.println("Setting time");
          byte century, year, month, day, hour, minute, second;
          // command should have been validated at this point
          set_time(serial_msg + TIME_CMD_DATA_OFFSET, &century, &year, &month, &day, &hour, &minute, &second);
          Serial.print("DEBUG main: Century: ");
          Serial.print(century);
          Serial.print(", year: ");
          Serial.print(year);
          Serial.print(", month: ");
          Serial.print(month);
          Serial.print(", day: ");
          Serial.print(day);
          Serial.print(", hour: ");
          Serial.print(hour);
          Serial.print(", minute: ");
          Serial.print(minute);
          Serial.print(", second: ");
          Serial.print(second);
          Serial.print("\n");
          break;
        case HOUR_CMD: // hour
          break;
        case 3: // minute
          break;
        case 4: // second
          break;
        default:
          Serial.println("Command type not supported");
          break;
      }

      serial_msg_index = 0;
      memset(serial_msg, '\0', SERIAL_MESSAGE_SIZE);
      continue;
    }

    serial_msg[serial_msg_index++] = current_char;
  }
}